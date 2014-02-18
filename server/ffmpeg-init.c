/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

/* This file contains initialization code for ffmpeg */

#include <pthread.h>
#include <bsd/string.h>
#include <libavformat/avformat.h>
#include "logc.h"

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

static AVCodec fake_h264_encoder = {
	.name = "fakeh264",
	.long_name = "Fake H.264 Encoder for RTP Muxing",
	.type = AVMEDIA_TYPE_VIDEO,
	.id = AV_CODEC_ID_H264,
	.priv_data_size = 0,
	.init = fake_h264_init,
	.encode2 = fake_h264_frame,
	.close = fake_h264_close,
	.pix_fmts = (const enum PixelFormat[]) {
		AV_PIX_FMT_YUV420P,
		AV_PIX_FMT_YUVJ420P,
		AV_PIX_FMT_NONE
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

static int bc_av_lockmgr(void **mutex_p, enum AVLockOp op)
{
	pthread_mutex_t **mutex = (pthread_mutex_t**)mutex_p;
	switch (op) {
		case AV_LOCK_CREATE:
			*mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
			if (!*mutex)
				return 1;
			return !!pthread_mutex_init(*mutex, NULL);

		case AV_LOCK_OBTAIN:
			return !!pthread_mutex_lock(*mutex);

		case AV_LOCK_RELEASE:
			return !!pthread_mutex_unlock(*mutex);

		case AV_LOCK_DESTROY:
			pthread_mutex_destroy(*mutex);
			free(*mutex);
			return 0;
	}

	return 1;
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
