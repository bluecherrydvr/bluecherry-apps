/*
 * Copyright (C) 2017 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
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

	const stream_packet &streaming_packet() const;
	const stream_packet &recording_packet() const;
private:
	decoder *dec;
	watermarker *wmr;
	encoder *enc_streaming;
	encoder *enc_recording;
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
	bool stats_collected;
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
};

#endif

