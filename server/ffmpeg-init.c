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

void bc_ffmpeg_init()
{
	avformat_network_init();
	avdevice_register_all();

	av_log_set_callback(av_log_cb);
}

void bc_ffmpeg_teardown()
{
	;
}
