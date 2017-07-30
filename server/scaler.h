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

	bool init_scaler(int out_width, int out_height, AVBufferRef *hwframe_ctx, AVCodecContext *dec_ctx);

	void push_frame(AVFrame *in);
	AVFrame *scaled_frame();

private:
	AVFilterContext *buffersink_ctx;
	AVFilterContext *buffersrc_ctx;
	AVFilterGraph *filter_graph;
	AVBufferRef *hw_frame_ctx;
};

#endif
