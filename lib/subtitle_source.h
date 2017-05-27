/*
 * Copyright (C) 2017 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __LIBBLUECHERRY_SUBTITLESOURCE_H
#define __LIBBLUECHERRY_SUBTITLESOURCE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class stream_packet;

class subtitle_source
{
public:
	subtitle_source();
	~subtitle_source();

	void stop();
	int start();

	int write_packet(const char *in);
	const stream_packet &packet() const { return current_packet; }
	virtual bool has_subtitles() const { return true; }

private:
	AVCodec *encoder;
	AVCodecContext *enc_ctx;
	bool is_started;

	uint8_t *subtitle_out;
	int packet_counter;
	stream_packet current_packet;
	std::shared_ptr<stream_properties> current_properties;
};

#endif
 
