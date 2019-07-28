/*
 * Copyright (C) 2019 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __LIBBLUECHERRY_WATERMARKER_H
#define __LIBBLUECHERRY_WATERMARKER_H

#include "stream_elements.h"

extern "C"
{
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/log.h>
}

#include <ass/ass.h>

class watermarker
{
public:
	watermarker();
	~watermarker();

	bool init_watermarker(const char *dvrname, const AVCodecContext *dec_ctx);

	void push_frame(AVFrame *in);
	AVFrame *watermarked_frame();

	void reinitialize(const AVCodecContext *updated_ctx);

private:
	const char *name_string;
	AVFilterContext *buffersink_ctx;
	AVFilterContext *buffersrc_ctx;
	AVFilterGraph *filter_graph;
	const AVCodecContext *decoder_ctx;
	AVFrame *out_frame;
	ASS_Track    *track;

	bool software_decoding;

	void release_watermarker();
	void find_asstrack();
};

#endif
