/*
 * Copyright (C) 2011 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __RTP_DEVICE_H
#define __RTP_DEVICE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "libbluecherry.h"

#define RTP_NUM_STREAMS 3

struct rtp_stream_data {
	int64_t pts_base, last_pts, last_pts_diff;
	int was_last_diff_skipped;
};

class rtp_device : public input_device
{
public:
	explicit rtp_device(const char *url);
	virtual ~rtp_device();

	virtual int start();
	virtual void stop();
	virtual void reset();

	virtual int buf_get();
	virtual void *buf_data();
	virtual unsigned int buf_size();

	virtual int is_key_frame();
	virtual int is_video_frame();

	virtual bool has_audio() const { return audio_stream_index >= 0; }

	const char *get_error_message() const { return error_message; }

	// XXX: Refactor to remove
	const AVPacket *current_frame() const { return &frame; }
	AVStream *video_stream() const { return ctx->streams[video_stream_index]; }
	AVStream *audio_stream() const { return ctx->streams[audio_stream_index]; }
	const char *stream_info();

	/* Decode the current packet, if it is a video packet, and put the result in
	 * frame. frame must be allocated with avcodec_alloc_frame prior to calling.
	 * Returns -1 on error, 0 on no-picture (but no error), and 1 when frame
	 * contains a valid picture. */
	int decode_video(AVFrame *frame);

	virtual int setup_output(AVFormatContext *out_ctx);

private:
	char url[1024];
	char error_message[512];

	AVFormatContext *ctx;
	/* -1 for no stream */
	int video_stream_index, audio_stream_index;
	AVPacket frame;

	struct rtp_stream_data stream_data[RTP_NUM_STREAMS];

	void set_current_pts(int64_t pts);
};

#endif /* __RTP_DEVICE_H */
