/*
 * Copyright (C) 2017 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __LIBBLUECHERRY_SCALER_H
#define __LIBBLUECHERRY_SCALER_H

#include "stream_elements.h"

extern "C"
{
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}


class scaler
{
public:
	scaler();
	~scaler();

	bool init_scaler(int out_width, int out_height, const AVCodecContext *dec_ctx, AVBufferRef *hwctx);

	void push_frame(AVFrame *in);
	AVFrame *scaled_frame();

	void reinitialize(const AVCodecContext *updated_ctx, AVBufferRef *hwctx);

private:
	AVFilterContext *buffersink_ctx;
	AVFilterContext *buffersrc_ctx;
	AVFilterGraph *filter_graph;
	AVBufferRef *hw_frames_ctx;
	AVFrame *out_frame;

	int scaled_width;
	int scaled_height;
	bool software_decoding;

	void release_scaler();
};

#endif
