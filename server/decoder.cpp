
extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "decoder.h"
#include "vaapi.h"

decoder::decoder(int media_type)
	: decoder_ctx(0), frame(0), type(media_type)
{
}

decoder::~decoder()
{
	if (decoder_ctx)
		release_decoder();
}

bool decoder::init_decoder()
{
	enum AVCodecID codec_id;
	AVCodecContext *ctx;
	int ret;

	if (type == AVMEDIA_TYPE_VIDEO)
		codec_id = saved_properties.get()->video.codec_id;
	else if (type == AVMEDIA_TYPE_AUDIO)
		codec_id = saved_properties.get()->audio.codec_id;

	AVCodec *codec = avcodec_find_decoder(codec_id);

	if (!codec)
		return false;

	ctx = avcodec_alloc_context3(codec);

	if (!ctx)
		return false;

	saved_properties.get()->video.apply(ctx);

	if (type == AVMEDIA_TYPE_VIDEO)
	{
		ctx->get_format = vaapi_hwaccel::get_format;
		ctx->get_buffer2 = vaapi_hwaccel::get_buffer;
	}

	AVDictionary *decoder_opts = NULL;
        av_dict_set(&decoder_opts, "refcounted_frames", "1", 0);
        ret = avcodec_open2(ctx, codec, &decoder_opts);
        av_dict_free(&decoder_opts);

        if (ret < 0)
	{
		avcodec_free_context(&ctx);
		return false;
	}

	frame = av_frame_alloc();

	if (!frame)
	{
		avcodec_free_context(&ctx);
		return false;
	}

	decoder_ctx = ctx;
	return true;
}

void decoder::release_decoder()
{
	if (decoder_ctx)
	{
		avcodec_close(decoder_ctx);
		avcodec_free_context(&decoder_ctx);
	}

	if (frame)
		av_frame_free(&frame);
}

AVFrame *decoder::decoded_frame()
{
	int ret;

	ret = avcodec_receive_frame(decoder_ctx, frame);

	if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
        {
		print_av_errormsg(ret);
		return 0;
	}

	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		return 0;

	return frame;
}


void decoder::print_av_errormsg(int ret)
{
	char error[512];
	av_strerror(ret, error, sizeof(error));
	bc_log(Error, "decoder error: %s", error);
}

void decoder::push_packet(const stream_packet &pkt)
{
	if (pkt.type != type)
		return;

	if (pkt.properties() != saved_properties)
	{
		saved_properties = pkt.properties();
		release_decoder();
	}

	if (!decoder_ctx)
	{
		if (!init_decoder())
		{
			bc_log(Error, "Cannot initialize decoder!");
			return;
		}
	}

	AVPacket avpkt;
	av_init_packet(&avpkt);
	avpkt.flags = pkt.flags;
	avpkt.pts   = pkt.pts;
	avpkt.dts   = pkt.dts;
	avpkt.data  = const_cast<uint8_t*>(pkt.data());
	avpkt.size  = pkt.size;

	int ret = avcodec_send_packet(decoder_ctx, &avpkt);

	if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
	{
		print_av_errormsg(ret);
	}
}

