#include "encoder.h"
#include "vaapi.h"

encoder::encoder()
	: encoder_ctx(0), type(0), next_packet_seq(0)
{
}

encoder::~encoder()
{
	if (encoder_ctx)
	{
		avcodec_close(encoder_ctx);
		avcodec_free_context(&encoder_ctx);
	}
}

void encoder::push_frame(AVFrame* frame)
{
	int ret;

	if (!encoder_ctx)
	{
		bc_log(Error, "encoder is not initializd, push_frame() failed!");
		return;
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
	AVPacket pkt;

	av_init_packet(&pkt);

	ret = avcodec_receive_packet(encoder_ctx, &pkt);

	if (ret < 0)
	{
		if (ret != AVERROR(EAGAIN))
		{
			bc_log(Error, "Failed to get encoded packet");
		}

		return false;
	}

	//set stream_packet
	uint8_t *buf = new uint8_t[pkt.size + FF_INPUT_BUFFER_PADDING_SIZE];
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
	///

	av_packet_unref(&pkt);
	return true;
}

bool encoder::init_encoder(int media_type, int codec_id, int bitrate, int width, int height, AVBufferRef* hw_frames_ctx)
{
	AVCodec *encoder;

	type = media_type;

	stream_properties *p = new stream_properties;

	if (media_type == AVMEDIA_TYPE_VIDEO)
	{
		p->video.codec_id = (enum AVCodecID)codec_id;
		p->video.pix_fmt = AV_PIX_FMT_VAAPI;
                p->video.width = width;
                p->video.height = height;
                //p->video.time_base = ic->time_base;
                //p->video.profile = ic->profile;
	}

	props = std::shared_ptr<stream_properties>(p);


	if (type == AVMEDIA_TYPE_VIDEO && props->video.codec_id == AV_CODEC_ID_H264)
	{
		encoder = avcodec_find_encoder_by_name("h264_vaapi");
	}

	if (!encoder)
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
		encoder_ctx->gop_size = 2;
		encoder_ctx->hw_frames_ctx = hw_frames_ctx;
	}

	//audio ...

	if (avcodec_open2(encoder_ctx, encoder, NULL) < 0)
	{
		bc_log(Error, "Failed to open encoder");
		encoder_ctx->hw_frames_ctx = 0;
		avcodec_close(encoder_ctx);
		avcodec_free_context(&encoder_ctx);
		return false;
	}

	return true;
}

