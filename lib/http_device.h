/*
 * Copyright (C) 2011 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __HTTP_DEVICE_H
#define __HTTP_DEVICE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "libbluecherry.h"

#define HTTP_NUM_STREAMS 3

struct http_stream_data {
	int64_t pts_base, last_pts, last_pts_diff;
	int was_last_diff_skipped;
};

class http_device : public input_device
{
public:
	explicit http_device(const char *url);
	virtual ~http_device();

	virtual int start();
	virtual void stop();

	virtual int read_packet();
	virtual const stream_packet &packet() const { return current_packet; }

	virtual bool has_audio() const { return audio_stream_index >= 0; }

	const char *get_error_message() const { return error_message; }
	const char *stream_info();

private:
	char url[1024];
	char error_message[512];

	AVFormatContext *ctx;
	/* -1 for no stream */
	int video_stream_index, audio_stream_index;
	AVPacket frame;
	stream_packet current_packet;

	struct http_stream_data stream_data[HTTP_NUM_STREAMS];

	void create_stream_packet(AVPacket *src);
	void set_current_pts(int64_t pts);

	void update_properties();
};

#endif /* __HTTP_DEVICE_H */
