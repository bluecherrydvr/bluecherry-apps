/*
 * Copyright (C) 2011 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include "rtp-session.h"

void rtp_session_init(struct rtp_session *rs, const char *url)
{
	memset(rs, 0, sizeof(*rs));
	rs->video_stream_index = rs->audio_stream_index = -1;
	
	strncpy(rs->url, url, sizeof(rs->url));
	rs->url[sizeof(rs->url)-1] = '\0';
}

void rtp_session_stop(struct rtp_session *rs)
{
	av_free_packet(&rs->frame);
	av_close_input_file(rs->ctx);
}

int rtp_session_start(struct rtp_session *rs)
{
	int i;

	if (av_open_input_file(&rs->ctx, rs->url, NULL, 0, NULL) != 0) {
		/* XXX error handling */
		fprintf(stderr, "Could not open URI\n");
		return -1;
	}

	/* XXX Return value may be useful (AVERROR) */
	if (av_find_stream_info(rs->ctx) < 0) {
		fprintf(stderr, "Could not find stream info\n");
		av_close_input_file(rs->ctx);
		return -1;
	}
	
	for (i = 0; i < rs->ctx->nb_streams; ++i) {
		if (rs->ctx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
			if (rs->video_stream_index >= 0) {
				fprintf(stderr, "Warning: session has multiple video streams\n");
				continue;
			}
			rs->video_stream_index = i;
		} else if (rs->ctx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
			if (rs->audio_stream_index >= 0) {
				fprintf(stderr, "Warning: session has multiple audio streams\n");
				continue;
			}
			rs->audio_stream_index = i;
		}
	}
	
	if (rs->video_stream_index < 0 && rs->audio_stream_index < 0) {
		fprintf(stderr, "Session opened successfully, but contains no valid media streams\n");
		av_close_input_file(rs->ctx);
		return -1;
	}
	
	return 0;
}

int rtp_session_read(struct rtp_session *rs)
{
	av_free_packet(&rs->frame);
	int re = av_read_frame(rs->ctx, &rs->frame);
	if (re < 0) {
		fprintf(stderr, "av_read_frame returned %d\n", re);	
		return -1;
	}
	return 0;
}

int rtp_session_frame_is_keyframe(struct rtp_session *rs)
{
	return (rs->frame.flags & AV_PKT_FLAG_KEY) == AV_PKT_FLAG_KEY;
}

