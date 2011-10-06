/*
 * Copyright (C) 2011 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __RTP_SESSION_H
#define __RTP_SESSION_H

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

struct rtp_session {
	char url[1024];
	char error_message[512];

	AVFormatContext *ctx;
	/* -1 for no stream. External code may set audio_stream_index to -1 after start to disable audio. */
	int video_stream_index, audio_stream_index;
	AVPacket frame;
	int64_t pts_base;
};

void rtp_session_init(struct rtp_session *rs, const char *url);
void rtp_session_stop(struct rtp_session *rs);
int rtp_session_start(struct rtp_session *rs);
int rtp_session_read(struct rtp_session *rs);
int rtp_session_setup_output(struct rtp_session *rs, AVFormatContext *out_ctx);
int rtp_session_frame_is_keyframe(struct rtp_session *rs);

#endif /* __RTP_SESSION_H */
