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

	void push_frame(AVFrame* frame, int motion_flag);
	bool encode();

	const stream_packet &packet() const { return current_packet; }

	AVRational get_fps() const { return current_fps; }
	void update_bitrate(int new_bitrate) { current_bitrate = new_bitrate; }
	void update_framerate(AVRational fps) { current_fps = fps; }

private:
	struct AVCodecContext *encoder_ctx;
	struct AVCodecContext *encoder_ctx_todelete;
	int type;
	stream_packet current_packet;
	std::shared_ptr<stream_properties> props;
	int next_packet_seq;
	int current_bitrate;
	AVRational current_fps;
	int motion_flags_ctr;

	void release_encoder(AVCodecContext **ctx);
public:
	bool init_encoder(int media_type, int codec_id, int bitrate, int width, int height, AVBufferRef* hw_frames_ctx);
};

#endif

