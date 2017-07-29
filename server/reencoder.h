/*
 * Copyright (C) 2017 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __LIBBLUECHERRY_REENCODER_H
#define __LIBBLUECHERRY_REENCODER_H

#include "decoder.h"
#include "encoder.h"

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
	reencoder();
	~reencoder();

	bool push_packet(const stream_packet &packet);

	bool run_loop();

	const stream_packet &packet() const;
private:
	decoder *dec;
	encoder *enc;
	bool software_decoding;
	AVBufferRef *hwframe_ctx;
};

#endif

