/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

/* This file contains initialization code for libav */

#include <bsd/string.h>
#include "bc-server.h"

/* Fake H.264 encoder for libavcodec. We're only muxing video, never reencoding,
 * so a real encoder isn't neeeded, but one must be present for the process to
 * succeed. ffmpeg does not support h264 encoding without libx264, which is GPL.
 */
static int fake_h264_init(AVCodecContext *ctx)
{
	(void)ctx;
	return 0;
}

static int fake_h264_close(AVCodecContext *ctx)
{
	(void)ctx;
	return 0;
}

static int fake_h264_frame(AVCodecContext *ctx, AVPacket *avpkt,
		const AVFrame *frame, int *got_packet_ptr)
{
	(void)ctx;
	(void)avpkt;
	(void)frame;
	(void)got_packet_ptr;
	return -1;
}

static AVCodec fake_h264_encoder;


/* Warning: Must be reentrant; this may be called from many device threads at
 * once */
static void av_log_cb(void *avcl, int level, const char *fmt, va_list ap)
{
	(void)avcl;

	log_level bc_level = Info;
	switch (level) {
		case AV_LOG_PANIC: bc_level = Fatal; break;
		case AV_LOG_FATAL: bc_level = Error; break;
		case AV_LOG_ERROR: bc_level = Warning; break;
		case AV_LOG_WARNING:
		case AV_LOG_INFO: bc_level = Info; break;
#ifdef LIBAV_DEBUG
		case AV_LOG_DEBUG:
#endif
		case AV_LOG_VERBOSE: bc_level = Debug; break;
		default: return;
	}

	const log_context &context = bc_log_context();
	if (!context.level_check(bc_level))
		return;

	char msg[1024] = "[libav] ";
	strlcat(msg, fmt, sizeof(msg));
	context.vlog(bc_level, msg, ap);
}


void bc_libav_init()
{
	if (av_lockmgr_register(bc_av_lockmgr)) {
		bc_log(Fatal, "libav lock registration failed: %m");
		exit(1);
	}

	fake_h264_encoder.name = "fakeh264";
	fake_h264_encoder.type = AVMEDIA_TYPE_VIDEO;
	fake_h264_encoder.id = AV_CODEC_ID_H264;
	fake_h264_encoder.priv_data_size = 0;
	fake_h264_encoder.init = fake_h264_init;
	fake_h264_encoder.encode2 = fake_h264_frame;
	fake_h264_encoder.close = fake_h264_close;
	fake_h264_encoder.pix_fmts = (const enum PixelFormat[]) { AV_PIX_FMT_YUV420P,
		AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_NONE };
	fake_h264_encoder.long_name = "Fake H.264 Encoder for RTP Muxing";
	avcodec_register(&fake_h264_encoder);
	av_register_all();
	avformat_network_init();

	av_log_set_callback(av_log_cb);
}

void bc_libav_term()
{
	av_lockmgr_register(NULL);
}
