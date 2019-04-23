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

#ifndef __LIBBLUECHERRY_REENCODER_H
#define __LIBBLUECHERRY_REENCODER_H

#include "decoder.h"
#include "encoder.h"
#include "scaler.h"

extern "C"
{
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}

class reencoder
{
public:
	reencoder(int bitrate, int out_frame_w, int out_frame_h);
	~reencoder();
	void update_bitrate(int new_bitrate);

	bool push_packet(const stream_packet &packet);

	bool run_loop();

	const stream_packet &packet() const;
private:
	decoder *dec;
	encoder *enc;
	scaler *scl;

	int last_decoded_fw;
	int last_decoded_fh;

	int bitrate;
	int out_frame_w;
	int out_frame_h;
};

#endif

