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

	bool init_scaler(int out_width, int out_height, const AVCodecContext *dec_ctx);

	void push_frame(AVFrame *in);
	AVFrame *scaled_frame();

	void reinitialize(const AVCodecContext *updated_ctx);

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
