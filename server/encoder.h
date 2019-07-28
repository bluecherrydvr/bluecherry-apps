/*
 * Copyright (C) 2017 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __LIBBLUECHERRY_ENCODER_H
#define __LIBBLUECHERRY_ENCODER_H

#include "stream_elements.h"

class encoder
{
public:
	encoder();
	~encoder();

	void push_frame(AVFrame* frame);
	bool encode();

	const stream_packet &packet() const { return current_packet; }

	void update_bitrate(int new_bitrate) { current_bitrate = new_bitrate; }
	void update_framerate(AVRational fps) { current_fps = fps; }

private:
	struct AVCodecContext *encoder_ctx;
	int type;
	stream_packet current_packet;
	std::shared_ptr<stream_properties> props;
	int next_packet_seq;
	int current_bitrate;
	AVRational current_fps;

	void release_encoder();
public:
	bool init_encoder(int media_type, int codec_id, int bitrate, int width, int height, AVBufferRef* hw_frames_ctx);
};

#endif

