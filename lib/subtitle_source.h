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

	int get_stream_packet(stream_packet *out, const char *in);

private:
	AVCodec *encoder;
	AVCodecContext *enc_ctx;
	bool is_started;
};

#endif
 
