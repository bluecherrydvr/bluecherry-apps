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


class subtitle_source
{
public:
	subtitle_source();
	~subtitle_source();
private:
	AVCodec *encoder;
	AVCodecContext *enc_ctx;
};

#endif
 
