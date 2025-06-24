#include "encoder.h"
#include "vaapi.h"

encoder::encoder()
	: encoder_ctx(0), encoder_ctx_todelete(0), type(0), next_packet_seq(0),
	current_fps(AVRational{5, 1}), motion_flags_ctr(0)
{
}

encoder::~encoder()
{
	release_encoder(&encoder_ctx_todelete);
	release_encoder(&encoder_ctx);
}

void encoder::release_encoder(AVCodecContext **ctx)
{
	if (*ctx)
	{
		avcodec_close(*ctx);
		avcodec_free_context(ctx);
		*ctx = nullptr;
	}
}

void encoder::push_frame(AVFrame* frame, int motion_flag)
{
	int ret;

	if (!encoder_ctx)
	{
		bc_log(Error, "encoder is not initialized, push_frame() failed!");
		return;
	}

	if (motion_flag)
		motion_flags_ctr++;

	if (motion_flags_ctr > 250)
		bc_log(Error, "encoder: motion flags counter value grows too big");

	/* Check if frame size is changed since initialization or bitrate is updated */
	if (frame && (frame->width != encoder_ctx->width || frame->height != encoder_ctx->height
		|| current_bitrate != encoder_ctx->bit_rate
		|| current_fps.num != encoder_ctx->framerate.num))
	{
		int codec_id;
		AVBufferRef* hw_frames_ctx;

		codec_id = encoder_ctx->codec_id;

		release_encoder(&encoder_ctx_todelete);

		encoder_ctx_todelete = encoder_ctx;
		encoder_ctx = nullptr;

		avcodec_send_frame(encoder_ctx_todelete, NULL);

		/* reinitialize encoder with new frame size or bitrate */
		if (!init_encoder(type, codec_id, current_bitrate, frame->width, frame->height, frame->hw_frames_ctx))
		{
			bc_log(Error, "failed to reinitialize encoder");
			return;
		}

		bc_log(Debug, "Encoder is reinitialized for frame size %dx%d, output bitrate %d", frame->width, frame->height, current_bitrate);
	}

	ret = avcodec_send_frame(encoder_ctx, frame);

	if (ret < 0)
	{
		bc_log(Error, "Failed to encode frame");
	}
}

bool encoder::encode()
{
	int ret;
	AVPacket pkt = {0};
	AVCodecContext *ctx;

	ctx = encoder_ctx_todelete ? encoder_ctx_todelete : encoder_ctx;

	ret = avcodec_receive_packet(ctx, &pkt);

	if (ret < 0)
	{
		if (encoder_ctx_todelete && ret == AVERROR_EOF)
		{
			release_encoder(&encoder_ctx_todelete);
			return false;
		}

		if (ret != AVERROR(EAGAIN))
		{
			bc_log(Error, "Failed to get encoded packet");
		}

		return false;
	}

	//set stream_packet
	uint8_t *buf = new uint8_t[pkt.size + AV_INPUT_BUFFER_PADDING_SIZE];
	memcpy(buf, pkt.data, pkt.size);

	current_packet = stream_packet(buf, props);
	current_packet.seq      = next_packet_seq++;
	current_packet.size     = pkt.size;
	current_packet.ts_clock = time(NULL);
	current_packet.pts      = pkt.pts;
	current_packet.dts      = pkt.dts;
	if (pkt.flags & AV_PKT_FLAG_KEY)
		current_packet.flags |= stream_packet::KeyframeFlag;
	current_packet.ts_monotonic = bc_gettime_monotonic();

	current_packet.type = type;
	if (motion_flags_ctr > 0)
	{
		current_packet.flags |= stream_packet::MotionFlag;
		motion_flags_ctr--;
	}

	av_packet_unref(&pkt);
	return true;
}

bool encoder::init_encoder(int media_type, int codec_id, int bitrate, int width, int height, AVBufferRef* hw_frames_ctx)
{
	const AVCodec *encoder = NULL;

	type = media_type;
	current_bitrate = bitrate;

	stream_properties *p = new stream_properties;

	if (media_type == AVMEDIA_TYPE_VIDEO)
	{
		p->video.codec_id = (enum AVCodecID)codec_id;
		p->video.pix_fmt = AV_PIX_FMT_VAAPI;
                p->video.width = width;
                p->video.height = height;
                //p->video.time_base = ic->time_base;
                p->video.profile = 100; //H264 High profile
	}

	props = std::shared_ptr<stream_properties>(p);


	if (type == AVMEDIA_TYPE_VIDEO && props->video.codec_id == AV_CODEC_ID_H264)
	{
		encoder = avcodec_find_encoder_by_name("h264_vaapi");
	}

	if (encoder == NULL)
	{
		bc_log(Error, "Failed to find encoder");
		return false;
	}


	encoder_ctx = avcodec_alloc_context3(encoder);

	if (type == AVMEDIA_TYPE_VIDEO && props->video.codec_id == AV_CODEC_ID_H264)
	{
		props->video.apply(encoder_ctx);
		encoder_ctx->time_base = AVRational{1, 1000};

		encoder_ctx->bit_rate = bitrate;
		encoder_ctx->rc_max_rate = bitrate * 2;
		encoder_ctx->gop_size = 20;
		encoder_ctx->hw_frames_ctx = av_buffer_ref(hw_frames_ctx);
		encoder_ctx->framerate = current_fps;
		encoder_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}

	AVDictionary *options = nullptr;

	//av_dict_set(&options, "rc_mode", "AVBR", 0);

	if (avcodec_open2(encoder_ctx, encoder, &options) < 0)
	{
		bc_log(Error, "Failed to open encoder");
		encoder_ctx->hw_frames_ctx = 0;
		avcodec_close(encoder_ctx);
		avcodec_free_context(&encoder_ctx);
		encoder_ctx = 0;
		av_dict_free(&options);
		return false;
	}

	av_dict_free(&options);

	if (encoder_ctx->extradata && encoder_ctx->extradata_size)
		p->video.extradata.assign(encoder_ctx->extradata, encoder_ctx->extradata + encoder_ctx->extradata_size);


	return true;
}

