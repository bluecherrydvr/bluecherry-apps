/*
 * Copyright (C) 2017 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
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
	vaapi_hwaccel(AVBufferRef *frctx);

	static bool init(const char *device);

	//AVCodecContext callbacks
	static enum AVPixelFormat get_format(AVCodecContext *s, const enum AVPixelFormat *pix_fmts);
	static int get_buffer(AVCodecContext *s, AVFrame *frame, int flags);

	bool hwupload_frame(AVCodecContext *s, AVFrame *frame);
	bool hwdownload_frame(AVCodecContext *s, AVFrame *frame);

	AVBufferRef *frame_ctx() const { return frame_context; }
private:
	static AVBufferRef *device_context;
	static bool is_initialised;
	static int init_decoder(AVCodecContext *s);

	AVBufferRef *frame_context;
};

#endif

