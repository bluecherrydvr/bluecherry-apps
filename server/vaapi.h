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

#ifndef __LIBBLUECHERRY_VAAPI_H
#define __LIBBLUECHERRY_VAAPI_H

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/hwcontext.h>
#include <libavutil/pixfmt.h>
#include <libavutil/buffer.h>
}

class vaapi_hwaccel
{
public:
	vaapi_hwaccel();

	static bool init(const char *device);

	//AVCodecContext callbacks
	static enum AVPixelFormat get_format(AVCodecContext *s, const enum AVPixelFormat *pix_fmts);
	static int get_buffer(AVCodecContext *s, AVFrame *frame, int flags);

	static bool hwupload_frame(AVBufferRef *hwframe_ctx, AVFrame *input);
	static bool hwdownload_frame(AVCodecContext *s, AVFrame *frame);

	static AVBufferRef *alloc_frame_ctx(const AVCodecContext *s);
private:
	static AVBufferRef *device_context;
	static bool is_initialised;
	static int init_decoder(AVCodecContext *s);

};

#endif

