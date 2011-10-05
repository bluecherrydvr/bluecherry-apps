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
	if (!rs->ctx)
		return;

	av_free_packet(&rs->frame);
	av_close_input_file(rs->ctx);
	rs->ctx = 0;
	rs->video_stream_index = rs->audio_stream_index = -1;
}

int rtp_session_start(struct rtp_session *rs)
{
	int i, re;

	if (rs->ctx)
		return 0;

	bc_log("Opening RTSP session from URL: %s", rs->url);

	if ((re = av_open_input_file(&rs->ctx, rs->url, NULL, 0, NULL)) != 0) {
		av_strerror(re, rs->error_message, sizeof(rs->error_message));
		return -1;
	}

	if ((re = av_find_stream_info(rs->ctx)) < 0) {
		rtp_session_stop(rs);
		av_strerror(re, rs->error_message, sizeof(rs->error_message));
		return -1;
	}
	
	for (i = 0; i < rs->ctx->nb_streams; ++i) {
		if (rs->ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			if (rs->video_stream_index >= 0) {
				bc_log("RTSP session for %s has multiple video streams. Only the "
				       "first stream will be recorded.", rs->url);
				continue;
			}
			rs->video_stream_index = i;
		} else if (rs->ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			if (rs->audio_stream_index >= 0) {
				bc_log("RTSP session for %s has multiple audio streams. Only the "
				       "first stream will be recorded.", rs->url);
				continue;
			}
			rs->audio_stream_index = i;
		}
	}
	
	if (rs->video_stream_index < 0) {
		rtp_session_stop(rs);
		strcpy(rs->error_message, "RTSP session contains no valid video stream");
		return -1;
	}
	
	return 0;
}

int rtp_session_read(struct rtp_session *rs)
{
	if (!rs->ctx) {
		strcpy(rs->error_message, "No active RTSP session");
		return -1;
	}

	av_free_packet(&rs->frame);
	int re = av_read_frame(rs->ctx, &rs->frame);
	if (re < 0) {
		av_strerror(re, rs->error_message, sizeof(rs->error_message));
		return -1;
	}
	
	//bc_log("RTP read frame pts=%lld dts=%lld size=%d stream_index=%d %s", rs->frame.pts, rs->frame.dts, rs->frame.size, rs->frame.stream_index, (rs->frame.flags & AV_PKT_FLAG_KEY) ? "key" : "");
	return 0;
}

int rtp_session_frame_is_keyframe(struct rtp_session *rs)
{
	return (rs->frame.flags & AV_PKT_FLAG_KEY) == AV_PKT_FLAG_KEY;
}

int rtp_session_setup_output(struct rtp_session *rs, AVFormatContext *out_ctx)
{
	if (!rs->ctx) {
		strcpy(rs->error_message, "No active RTSP session");
		return -1;
	}

	AVStream *vst = av_new_stream(out_ctx, 0);
	if (!vst) {
		strcpy(rs->error_message, "Cannot add new stream");
		return -1;
	}

	AVCodecContext *ic = rs->ctx->streams[rs->video_stream_index]->codec;
	vst->codec->codec_id = ic->codec_id;
	vst->codec->codec_type = ic->codec_type;
	vst->codec->pix_fmt = ic->pix_fmt;
	vst->codec->width = ic->width;
	vst->codec->height = ic->height;
	vst->codec->time_base = ic->time_base;

	if (out_ctx->oformat->flags & AVFMT_GLOBALHEADER)
		vst->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	if (rs->audio_stream_index >= 0) {
		/* Audio recording errors are non-fatal; video will still be captured */
		AVStream *ast = av_new_stream(out_ctx, 1);
		if (!ast) {
			av_log(out_ctx, AV_LOG_ERROR, "Cannot add audio stream to output context");
			return 0;
		}
		
		ic = rs->ctx->streams[rs->audio_stream_index]->codec;
		ast->codec->codec_id = ic->codec_id;
		ast->codec->codec_type = ic->codec_type;
		ast->codec->bit_rate = ic->bit_rate;
		ast->codec->sample_rate = ic->sample_rate;
		ast->codec->sample_fmt = ic->sample_fmt;
		ast->codec->channels = ic->channels;
		ast->codec->time_base = (AVRational){1, ic->sample_rate};
		
		if (out_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			ast->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	return 0;
}

