/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

/* This file contains initialization code for libav */

#include <stdlib.h>
#include <bsd/string.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include "logc.h"

int bc_av_lockmgr(void **mutex, enum AVLockOp op);

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

static int fake_h264_encode2(AVCodecContext *ctx, AVPacket *pkt,
			     const AVFrame *frame, int *gotpkt)
{
	(void)ctx;
	(void)pkt;
	(void)frame;
	(void)gotpkt;
	return -1;
}

static AVCodec fake_h264_encoder = {
	.name		= "fakeh264",
	.long_name	= "Fake H.264 Encoder for RTP Muxing",
	.type		= AVMEDIA_TYPE_VIDEO,
	.id		= AV_CODEC_ID_H264,
	.init		= fake_h264_init,
	.encode2	= fake_h264_encode2,
	.close		= fake_h264_close,
	.decode		= 0,
	.next		= 0,
	.flush		= 0,
	.capabilities	= CODEC_CAP_DELAY,
	.priv_data_size	= 0,
	.supported_framerates = 0,
	.pix_fmts	= (const enum PixelFormat[]) {
		PIX_FMT_YUV420P,
		PIX_FMT_YUVJ420P,
		PIX_FMT_NONE,
	},
};


/* Warning: Must be reentrant; this may be called from many device threads at
 * once */
static void av_log_cb(void *avcl, int level, const char *fmt, va_list ap)
{
	(void)avcl;

	enum log_level bc_level = Info;
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

	char msg[1024] = "[libav] ";
	strlcat(msg, fmt, sizeof(msg));
	bc_vlog(bc_level, msg, ap);
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
