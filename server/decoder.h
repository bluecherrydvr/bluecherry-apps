/*
 * Copyright (C) 2017 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __LIBBLUECHERRY_DECODER_H
#define __LIBBLUECHERRY_DECODER_H

#include "stream_elements.h"

class decoder
{
public:
	decoder(const stream_properties &prop);
	~decoder();

private:
	struct AVCodecContext *decoder_ctx;
	std::shared_ptr<const stream_properties> saved_properties;
};

#endif

