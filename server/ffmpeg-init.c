/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* This file contains initialization code for ffmpeg */

#include <pthread.h>
#include <bsd/string.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/avcodec.h>
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
	.pix_fmts = (const enum AVPixelFormat[]) {
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

	enum log_level bc_level = Debug;
	const char *level_prefix;
	switch (level) {
		case AV_LOG_PANIC:   level_prefix = "panic"; break;
		case AV_LOG_FATAL:   level_prefix = "fatal"; break;
		case AV_LOG_ERROR:   level_prefix = "error"; break;
		case AV_LOG_WARNING: level_prefix = "warning"; break;
		case AV_LOG_INFO:    level_prefix = "info"; break;
		case AV_LOG_VERBOSE: level_prefix = "verbose"; break;
		case AV_LOG_DEBUG:   level_prefix = "debug"; break;
		default:             level_prefix = "unknown";
	}

	char msg[1024] = "[libav] ";
	snprintf(msg, sizeof(msg), "[libav] %s %s", level_prefix, fmt);
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

void bc_ffmpeg_init()
{
	if (av_lockmgr_register(bc_av_lockmgr)) {
		bc_log(Fatal, "libav lock registration failed");
		exit(1);
	}

	avcodec_register(&fake_h264_encoder);
	av_register_all();
	avfilter_register_all();
	avformat_network_init();
	avdevice_register_all();
	avcodec_register_all();

	av_log_set_callback(av_log_cb);
}

void bc_ffmpeg_teardown()
{
	av_lockmgr_register(NULL);
}
