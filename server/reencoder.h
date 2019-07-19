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
#include <libavfilter/avfiltergraph.h>
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

	int last_decoded_fw;
	int last_decoded_fh;

	int out_frame_w;
	int out_frame_h;

	int streaming_bitrate;
	int recording_bitrate;

	int incoming_bitrate_avg;
	int avg_window_size;
	time_t avg_time;
	int incoming_packets_cnt;

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
};

#endif

