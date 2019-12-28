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

#ifndef __LIBBLUECHERRY_WATERMARKER_H
#define __LIBBLUECHERRY_WATERMARKER_H

#ifdef WATERMARKER_ENABLED

#include "stream_elements.h"

extern "C"
{
#include <libavfilter/avfilter.h>
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

	bool init_watermarker(const char *dvrname, const char *cameraname, const AVCodecContext *dec_ctx);

	void push_frame(AVFrame *in);
	AVFrame *watermarked_frame();

	void reinitialize(const AVCodecContext *updated_ctx);

private:
	const char *name_string;
	const char *dvr_name;
	const char *camera_name;
	AVFilterContext *buffersink_ctx;
	AVFilterContext *buffersrc_ctx;
	AVFilterGraph *filter_graph;
	const AVCodecContext *decoder_ctx;
	AVFrame *out_frame;
	ASS_Track    *track;
	char timestamp_buf[128];
	time_t last_timestamp;

	bool software_decoding;

	void release_watermarker();
	void find_asstrack();
};
#endif
#endif
