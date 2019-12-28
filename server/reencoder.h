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
#include "watermarker.h"

extern "C"
{
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}


class reencoder
{
public:
	reencoder(int streaming_bitrate, int out_frame_w, int out_frame_h, bool watermarking = false);
	~reencoder();
	void update_streaming_bitrate(int new_bitrate);
	void set_dvr_name(const char *dvrname) { dvr_name = dvrname; };
	void set_camera_name(const char *cameraname) { camera_name = cameraname; };

	bool push_packet(const stream_packet &packet, bool reencode_for_streaming);

	bool run_loop();

	bool read_streaming_packet();
	bool read_recording_packet();
	const stream_packet &streaming_packet() const;
	const stream_packet &recording_packet() const;
private:
	decoder *dec;
#ifdef WATERMARKER_ENABLED
	watermarker *wmr;
#else
	void *wmr;
#endif
	encoder *enc_streaming;
	encoder *enc_streaming_todelete;
	encoder *enc_recording;
	encoder *enc_recording_todelete;

	scaler *scl;
	/* intermediary hw context between watermarker and scaler/encoder */
	AVBufferRef *hw_frames_ctx;

	int last_decoded_fw;
	int last_decoded_fh;
	enum AVPixelFormat last_decoded_pixfmt;

	int out_frame_w;
	int out_frame_h;

	int streaming_bitrate;
	int recording_bitrate;

	void update_stats(const stream_packet &packet);

	int64_t incoming_bitrate_avg;
	uint32_t bitrate_calc_period_sec;
	uint32_t incoming_fps_ctr;
	int64_t first_packet_dts;

	bool watermarking;

	enum reencoder_mode
	{
		REENC_MODE_WMR_ONLY,
		REENC_MODE_WMR_AND_STREAMING,
		REENC_MODE_STREAMING_ONLY
	}
	mode;

	bool init_enc_streaming(AVFrame *in);
	bool init_enc_recording(AVFrame *in);

	const char *dvr_name;
	const char *camera_name;
	int motion_flags_ctr;
};

#endif

