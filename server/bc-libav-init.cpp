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

static int fake_h264_frame(AVCodecContext *ctx, uint8_t *buf, int bufsize,
			   void *data)
{
	(void)ctx;
	(void)buf;
	(void)bufsize;
	(void)data;
	return -1;
}

static AVCodec fake_h264_encoder = {
	"fakeh264", /* name */
	AVMEDIA_TYPE_VIDEO, /* type */
	CODEC_ID_H264, /* id */
	0, /* priv_data_size */
	fake_h264_init, /* init */
	fake_h264_frame, /* encode */
	fake_h264_close, /* close */
	0, /* decode */
	CODEC_CAP_DELAY, /* capabilities */
	0, /* next */
	0, /* flush */
	0, /* suipported_framerates */
	(const enum PixelFormat[]) { PIX_FMT_YUV420P, PIX_FMT_YUVJ420P,
				     PIX_FMT_NONE }, /* pix_fmts */
	"Fake H.264 Encoder for RTP Muxing", /* long_name */
};


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

	avcodec_register(&fake_h264_encoder);
	av_register_all();
	avformat_network_init();

	av_log_set_callback(av_log_cb);
}

void bc_libav_term()
{
	av_lockmgr_register(NULL);
}
