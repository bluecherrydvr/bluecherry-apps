/*
 * Copyright (C) 2011 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __RTP_DEVICE_H
#define __RTP_DEVICE_H

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#define RTP_NUM_STREAMS 3

struct rtp_stream_data {
	int64_t pts_base, last_pts, last_pts_diff;
	int was_last_diff_skipped;
};

struct rtp_device {
	char url[1024];
	char error_message[512];

	AVFormatContext *ctx;
	int want_audio;
	/* -1 for no stream */
	int video_stream_index, audio_stream_index;
	AVPacket frame;

	struct rtp_stream_data stream_data[RTP_NUM_STREAMS];
};

void rtp_device_init(struct rtp_device *rs, const char *url);
void rtp_device_stop(struct rtp_device *rs);
int rtp_device_start(struct rtp_device *rs);
int rtp_device_read(struct rtp_device *rs);
int rtp_device_setup_output(struct rtp_device *rs, AVFormatContext *out_ctx);
void rtp_device_set_current_pts(struct rtp_device *rs, int64_t pts);
int rtp_device_frame_is_keyframe(struct rtp_device *rs);
const char *rtp_device_stream_info(struct rtp_device *rs);
/* Decode the current packet, if it is a video packet, and put the result in
 * frame. frame must be allocated with avcodec_alloc_frame prior to calling.
 * Returns -1 on error, 0 on no-picture (but no error), and 1 when frame
 * contains a valid picture. */
int rtp_device_decode_video(struct rtp_device *rs, AVFrame *frame);

#endif /* __RTP_DEVICE_H */
