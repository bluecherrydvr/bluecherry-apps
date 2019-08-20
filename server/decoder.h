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
	decoder(int media_type);
	~decoder();

	void push_packet(const stream_packet &pkt);
	//const char *get_error_message();
	//bool has_error() const;

	bool decode();
	AVFrame *decoded_frame();
	const struct AVCodecContext *get_ctx() const { return decoder_ctx; }
	bool properties_changed();

private:
	struct AVCodecContext *decoder_ctx;
	AVFrame *frame;
	int type;
	std::shared_ptr<const stream_properties> saved_properties;
	bool props_changed;

	bool init_decoder();
	void release_decoder();
	void print_av_errormsg(int ret);
};

#endif

