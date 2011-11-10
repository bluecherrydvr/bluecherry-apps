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
#include <libavutil/opt.h>

#include "rtp-session.h"

void rtp_session_init(struct rtp_session *rs, const char *url)
{
	int i;
	memset(rs, 0, sizeof(*rs));
	rs->video_stream_index = rs->audio_stream_index = -1;

	strncpy(rs->url, url, sizeof(rs->url));
	rs->url[sizeof(rs->url)-1] = '\0';

	/* Currently, av_init_packet cannot be used here because it
	 * is undefined in the php module. We don't really need it,
	 * anyway. */
	rs->frame.pts = AV_NOPTS_VALUE;

	for (i = 0; i < RTP_NUM_STREAMS; ++i)
		rs->stream_data[i].last_pts = AV_NOPTS_VALUE;
}

void rtp_session_stop(struct rtp_session *rs)
{
	int i;

	if (!rs->ctx)
		return;

	av_free_packet(&rs->frame);
	av_init_packet(&rs->frame);
	av_close_input_file(rs->ctx);
	rs->ctx = 0;
	rs->video_stream_index = rs->audio_stream_index = -1;

	for (i = 0; i < RTP_NUM_STREAMS; ++i) {
		rs->stream_data[i].pts_base = 0;
		rs->stream_data[i].last_pts = AV_NOPTS_VALUE;
		rs->stream_data[i].last_pts_diff = 0;
		rs->stream_data[i].was_last_diff_skipped = 0;
	}
}

int rtp_session_start(struct rtp_session *rs)
{
	int i, re;
	AVDictionary *avopt = NULL;

	if (rs->ctx)
		return 0;

	av_log(NULL, AV_LOG_INFO, "Opening RTSP session from URL: %s\n", rs->url);

	av_dict_set(&avopt, "allowed_media_types", rs->want_audio ? "-data" : "-audio-data", 0);

	if ((re = avformat_open_input(&rs->ctx, rs->url, NULL, &avopt)) != 0) {
		av_strerror(re, rs->error_message, sizeof(rs->error_message));
		rs->ctx = 0;
		av_dict_free(&avopt);
		return -1;
	}

	av_dict_free(&avopt);

	if ((re = av_find_stream_info(rs->ctx)) < 0) {
		rtp_session_stop(rs);
		av_strerror(re, rs->error_message, sizeof(rs->error_message));
		return -1;
	}

	for (i = 0; i < rs->ctx->nb_streams && i < RTP_NUM_STREAMS; ++i) {
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
			} else if (rs->ctx->streams[i]->codec->codec_id == CODEC_ID_NONE) {
				av_log(rs->ctx, AV_LOG_ERROR, "No matching audio codec for stream; ignoring audio");
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
	int re;
	struct rtp_stream_data *streamdata = 0;
	if (!rs->ctx) {
		strcpy(rs->error_message, "No active RTSP session");
		return -1;
	}

	av_free_packet(&rs->frame);
	re = av_read_frame(rs->ctx, &rs->frame);
	if (re < 0) {
		av_strerror(re, rs->error_message, sizeof(rs->error_message));
		return -1;
	}

	if (rs->frame.stream_index >= 0 && rs->frame.stream_index < RTP_NUM_STREAMS)
		streamdata = &rs->stream_data[rs->frame.stream_index];

	/* Don't run offset logic against frames with no PTS */
	if (!streamdata || rs->frame.pts == AV_NOPTS_VALUE)
		return 0;

	/* Correct the stream's PTS to provide an even, monotonic set of frames for
	 * recording, regardless of network factors. Notably, this often comes into
	 * effect after receiving RTCP packets, which may alter the PTS to adjust
	 * inter-stream synchronization.
	 *
	 * We assume that the difference between the PTS of the last two frames,
	 * lastptsdiff, is an accurate guess about the difference between this frame
	 * and the previous, and adjust the base PTS accordingly. Calculation is
	 * done against the raw, unadjusted PTS to prevent some feedback loops. pts_base
	 * may be altered externally (but only by delta), because the recording logic wants
	 * to start with the first frame of each recording as PTS 0.
	 *
	 * If the difference between the PTS of the last frame and the current is more
	 * than 4x the difference of the previous two frames, we assume a forward jump
	 * and correct in the same manner as with negative differences. I believe this
	 * threshold is reasonable, but it may prove to cause false positives for real-world
	 * cases and require adjustment. If the threshold is too high, we're risking a
	 * relatively small gap in playback (and possibly accompanying audio
	 * desynchronization).
	 *
	 * If the PTS of any two otherwise valid frames is less than 1/4th (i.e. the above
	 * threshold) of the normal rate, we may never recover, and would adjust all future
	 * frames to match that too-fast rate. To help prevent that, especially in regards
	 * to audio streams (where intervals are less even), we don't update the interval
	 * when it is below 1/4th of the current unless it happens twice consecutively.
	 * A reverse of this logic may also be desirable, to bypass forward jump detection
	 * if we're seeing consecutive, consistently larger gaps than we expect.
	 *
	 * If it is valid under any codec to have two frames with equal PTS, that will fail.
	 * Variable framerates could be catastrophic.
	 *
	 * This logic isn't used for single video only streams, because libav uses the
	 * reliable RTP timestamp in that situation (#983).
	 *
	 * Test hardware is an AirLive OD-325HD, MPEG4 over TCP; and ACTi ACM-4200 over UDP.
	 */
	if (streamdata->last_pts != AV_NOPTS_VALUE && rs->ctx->nb_streams > 1) {
		if (rs->frame.pts <= streamdata->last_pts || (streamdata->last_pts_diff &&
		    (rs->frame.pts - streamdata->last_pts) >= (streamdata->last_pts_diff*4)))
		{
			av_log(rs->ctx, AV_LOG_INFO, "Inconsistent PTS on stream %d (type %d), "
			       "delta %lld. Adjusting based on last interval of %lld.",
			       rs->frame.stream_index,
			       rs->ctx->streams[rs->frame.stream_index]->codec->codec_type,
			       rs->frame.pts - streamdata->last_pts, streamdata->last_pts_diff);
			streamdata->pts_base -= (streamdata->last_pts - rs->frame.pts)
			                        + streamdata->last_pts_diff;
		} else {
			int64_t newptsdiff = rs->frame.pts - streamdata->last_pts;
			/* Don't update last_pts_diff when this interval is 1/4th or less of the
			 * last interval, corrosponding to the forward jump detection above.
			 *
			 * If we apply intervals this low, normal and valid frames after will be
			 * seen as forward jumps, and we can never recover. Singular drops in the
			 * interval are not uncommon, especially in audio streams, but consecutive
			 * low intervals have not been observed as occuring normally. So, we will
			 * update last_pts_diff, but only if two frames in a row have less than
			 * 1/4th of the last interval. */
			if (newptsdiff < streamdata->last_pts_diff &&
			    (streamdata->last_pts_diff/newptsdiff) >= 4)
			{
				if (!streamdata->was_last_diff_skipped) {
					av_log(rs->ctx, AV_LOG_INFO, "PTS interval on stream %d (type %d) dropped "
					       "to %lld (delta %lld); ignoring interval change unless repeated",
					       rs->frame.stream_index,
					       rs->ctx->streams[rs->frame.stream_index]->codec->codec_type,
					       newptsdiff, (newptsdiff - streamdata->last_pts_diff));
					streamdata->was_last_diff_skipped = 1;
				} else {
					av_log(rs->ctx, AV_LOG_WARNING, "PTS interval on stream %d (type %d) dropped "
					       "to %lld (delta %lld) twice; accepting new interval. Could cause "
					       "framerate or desynchronization issues.", rs->frame.stream_index,
					       rs->ctx->streams[rs->frame.stream_index]->codec->codec_type,
					       newptsdiff, (newptsdiff - streamdata->last_pts_diff));
					streamdata->was_last_diff_skipped = 0;
					streamdata->last_pts_diff = newptsdiff;
				}
			} else {
				streamdata->was_last_diff_skipped = 0;
				streamdata->last_pts_diff = newptsdiff;
			}
		}
	}

	streamdata->last_pts = rs->frame.pts;
	rs->frame.pts -= streamdata->pts_base;

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
	vst->codec->extradata = ic->extradata;
	vst->codec->extradata_size = ic->extradata_size;
	vst->codec->profile = ic->profile;

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
		ast->codec->extradata = ic->extradata;
		ast->codec->extradata_size = ic->extradata_size;
		ast->codec->profile = ic->profile;
		
		if (out_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			ast->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	return 0;
}

void rtp_session_set_current_pts(struct rtp_session *rs, int64_t pts)
{
	int64_t offset;
	int i;

	/* Adjust PTS offsets so that the current frame has a PTS of 'pts', and
	 * all other streams are adjusted accordingly. */
	if (rs->frame.stream_index < 0 || rs->frame.stream_index >= RTP_NUM_STREAMS)
		return;

	if (rs->frame.pts == AV_NOPTS_VALUE) {
		av_log(rs->ctx, AV_LOG_INFO, "Current frame has no PTS, so PTS reset to %lld cannot occur\n",
		       pts);
		return;
	}

	offset = rs->frame.pts - pts;

	av_log(rs->ctx, AV_LOG_INFO, "Adjusted pts_base by %lld to reset PTS on stream %d to %lld\n",
	       offset, rs->frame.stream_index, pts);

	for (i = 0; i < rs->ctx->nb_streams && i < RTP_NUM_STREAMS; ++i) {
		av_log(rs->ctx, AV_LOG_INFO, "Rescaled set_current_pts offset for stream %d is %lld\n",
		       i, av_rescale_q(offset, rs->ctx->streams[rs->frame.stream_index]->time_base, rs->ctx->streams[i]->time_base));
		rs->stream_data[i].pts_base += av_rescale_q(offset, rs->ctx->streams[rs->frame.stream_index]->time_base, rs->ctx->streams[i]->time_base);
	}

	rs->frame.pts = pts;
}

const char *rtp_session_stream_info(struct rtp_session *rs)
{
	if (rs->video_stream_index < 0) {
		strcpy(rs->error_message, "No streams");
		return rs->error_message;
	}

	/* Borrow the error_message field */
	avcodec_string(rs->error_message, sizeof(rs->error_message)-2,
	               rs->ctx->streams[rs->video_stream_index]->codec, 0);

	if (rs->audio_stream_index >= 0) {
		int off = strlen(rs->error_message);
		rs->error_message[off++] = ';';
		rs->error_message[off++] = ' ';
		avcodec_string(rs->error_message+off, sizeof(rs->error_message)-off,
		               rs->ctx->streams[rs->audio_stream_index]->codec, 0);
	}

	return rs->error_message;
}
