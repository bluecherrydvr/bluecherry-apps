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
#include <bsd/string.h>

#include "libbluecherry.h"
#include "rtp-session.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
}

void rtp_device_init(struct rtp_device *rs, const char *url)
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

void rtp_device_stop(struct rtp_device *rs)
{
	int i;

	if (!rs->ctx)
		return;

	av_free_packet(&rs->frame);
	av_init_packet(&rs->frame);

	for (i = 0; i < rs->ctx->nb_streams; ++i) {
		if (rs->ctx->streams[i]->codec->codec)
			avcodec_close(rs->ctx->streams[i]->codec);
	}

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

int rtp_device_start(struct rtp_device *rs)
{
	int i, re;
	AVDictionary *avopt = NULL;
	char tmp[24];

	if (rs->ctx)
		return 0;

	av_log(NULL, AV_LOG_INFO, "Opening RTSP session from URL: %s\n", rs->url);

	snprintf(tmp, sizeof(tmp), "%lld", (long long int)(0.7*AV_TIME_BASE));
	av_dict_set(&avopt, "max_delay", tmp, 0);
	av_dict_set(&avopt, "allowed_media_types", rs->want_audio ? "-data" : "-audio-data", 0);
	av_dict_set(&avopt, "threads", "1", 0);

	AVDictionary *opt_copy = 0;
	av_dict_copy(&opt_copy, avopt, 0);

	if ((re = avformat_open_input(&rs->ctx, rs->url, NULL, &avopt)) != 0) {
		av_strerror(re, rs->error_message, sizeof(rs->error_message));
		rs->ctx = 0;
		av_dict_free(&avopt);
		av_dict_free(&opt_copy);
		return -1;
	}

	if (av_dict_get(avopt, "", NULL, 0))
		av_log(rs->ctx, AV_LOG_WARNING, "Unable to set format options");

	av_dict_free(&avopt);

	/* avformat_find_stream_info takes an array of AVDictionary ptrs for each stream */
	AVDictionary **opt_si = new AVDictionary*[rs->ctx->nb_streams];
	for (i = 0; i < rs->ctx->nb_streams; ++i) {
		opt_si[i] = 0;
		av_dict_copy(&opt_si[i], opt_copy, 0);
	}

	re = avformat_find_stream_info(rs->ctx, opt_si);

	for (i = 0; i < rs->ctx->nb_streams; ++i)
		av_dict_free(&opt_si[i]);
	delete[] opt_si;
	av_dict_free(&opt_copy);

	if (re < 0) {
		rtp_device_stop(rs);
		av_strerror(re, rs->error_message, sizeof(rs->error_message));
		return -1;
	}

	for (i = 0; i < rs->ctx->nb_streams && i < RTP_NUM_STREAMS; ++i) {
		AVStream *stream = rs->ctx->streams[i];

		if (stream->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			if (rs->video_stream_index >= 0) {
				bc_log("RTSP session for %s has multiple video streams. Only the "
				       "first stream will be recorded.", rs->url);
				continue;
			}
			rs->video_stream_index = i;

			if (stream->time_base.num != 1 || stream->time_base.den != 90000) {
				av_log(rs->ctx, AV_LOG_WARNING, "Video stream timebase is unusual (%d/%d). "
				       "This could cause timing issues.", stream->time_base.num,
				       stream->time_base.den);
			}
		} else if (stream->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			if (rs->audio_stream_index >= 0) {
				bc_log("RTSP session for %s has multiple audio streams. Only the "
				       "first stream will be recorded.", rs->url);
				continue;
			} else if (stream->codec->codec_id == CODEC_ID_NONE) {
				av_log(rs->ctx, AV_LOG_ERROR, "No matching audio codec for stream; ignoring audio");
				continue;
			}
			rs->audio_stream_index = i;
		}
	}

	if (rs->video_stream_index < 0) {
		rtp_device_stop(rs);
		strcpy(rs->error_message, "RTSP session contains no valid video stream");
		return -1;
	}

	return 0;
}

/* Workaround because we cannot take the address of this function
 * without breaking the link with the PHP extension (which does
 * not know of libav). Ugly. */
static void wrap_av_destruct_packet(AVPacket *pkt)
{
	av_destruct_packet(pkt);
}

int rtp_device_read(struct rtp_device *rs)
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

	/* ACTi B2 frames are badly specified; they are MPEG4 user data elements which
	 * almost always contain a sequence of 23 0 bits, which is misinterpreted by
	 * generic MPEG4 parsers. Detect these frames and strip them off to avoid breaking
	 * everything. */
	if (rs->ctx->streams[rs->frame.stream_index]->codec->codec_id == CODEC_ID_MPEG4) {
		uint8_t b2_header[] = { 0x00, 0x00, 0x01, 0xb2 };
		if (rs->frame.size >= 47 && rs->frame.stream_index == rs->video_stream_index
		    && memcmp(rs->frame.data, b2_header, sizeof(b2_header)) == 0
		    && memcmp(rs->frame.data+44, b2_header, 3) == 0)
		{
			void *tmp = av_malloc(rs->frame.size); /* Let the extra 44 be buffer padding */
			int size = rs->frame.size - 44;
			memcpy(tmp, rs->frame.data+44, size);
			av_free_packet(&rs->frame);
			rs->frame.data = (uint8_t*)tmp;
			rs->frame.size = size;
			rs->frame.destruct = wrap_av_destruct_packet;
		}
	}

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
			       (long long int) rs->frame.pts - streamdata->last_pts,
			       (long long int) streamdata->last_pts_diff);
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
					       (long long int)newptsdiff, (long long int)(newptsdiff - streamdata->last_pts_diff));
					streamdata->was_last_diff_skipped = 1;
				} else {
					av_log(rs->ctx, AV_LOG_WARNING, "PTS interval on stream %d (type %d) dropped "
					       "to %lld (delta %lld) twice; accepting new interval. Could cause "
					       "framerate or desynchronization issues.", rs->frame.stream_index,
					       rs->ctx->streams[rs->frame.stream_index]->codec->codec_type,
					       (long long int)newptsdiff,
					       (long long int)(newptsdiff - streamdata->last_pts_diff));
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

int rtp_device_frame_is_keyframe(struct rtp_device *rs)
{
	return (rs->frame.flags & AV_PKT_FLAG_KEY) == AV_PKT_FLAG_KEY;
}

int rtp_device_setup_output(struct rtp_device *rs, AVFormatContext *out_ctx)
{
	if (!rs->ctx) {
		strcpy(rs->error_message, "No active RTSP session");
		return -1;
	}

	AVStream *vst = avformat_new_stream(out_ctx, NULL);
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
	vst->codec->profile = ic->profile;
	if (ic->extradata && ic->extradata_size) {
		vst->codec->extradata_size = ic->extradata_size;
		vst->codec->extradata = (uint8_t*)av_malloc(ic->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
		memcpy(vst->codec->extradata, ic->extradata, ic->extradata_size);
	}

	if (out_ctx->oformat->flags & AVFMT_GLOBALHEADER)
		vst->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	if (rs->audio_stream_index >= 0) {
		/* Audio recording errors are non-fatal; video will still be captured */
		AVStream *ast = avformat_new_stream(out_ctx, NULL);
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
		ast->codec->profile = ic->profile;
		if (ic->extradata && ic->extradata_size) {
			ast->codec->extradata_size = ic->extradata_size;
			ast->codec->extradata = (uint8_t*)av_malloc(ic->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
			memcpy(ast->codec->extradata, ic->extradata, ic->extradata_size);
		}

		if (out_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			ast->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	return 0;
}

void rtp_device_set_current_pts(struct rtp_device *rs, int64_t pts)
{
	int64_t offset;
	int i;

	/* Adjust PTS offsets so that the current frame has a PTS of 'pts', and
	 * all other streams are adjusted accordingly. */
	if (rs->frame.stream_index < 0 || rs->frame.stream_index >= RTP_NUM_STREAMS)
		return;

	if (rs->frame.pts == AV_NOPTS_VALUE) {
		av_log(rs->ctx, AV_LOG_INFO, "Current frame has no PTS, so PTS reset to %lld cannot occur\n",
		       (long long int)pts);
		return;
	}

	offset = rs->frame.pts - pts;

	av_log(rs->ctx, AV_LOG_INFO, "Adjusted pts_base by %"PRId64" to reset PTS on stream %d to %"PRId64"\n",
	       offset, rs->frame.stream_index, pts);

	for (i = 0; i < rs->ctx->nb_streams && i < RTP_NUM_STREAMS; ++i) {
		rs->stream_data[i].pts_base += av_rescale_q(offset, rs->ctx->streams[rs->frame.stream_index]->time_base, rs->ctx->streams[i]->time_base);
	}

	rs->frame.pts = pts;
}

const char *rtp_device_stream_info(struct rtp_device *rs)
{
	char *buf = rs->error_message;
	int size = sizeof(rs->error_message);

	if (rs->video_stream_index < 0) {
		strlcpy(buf, "No streams", size);
		return buf;
	}

	AVStream *stream = rs->ctx->streams[rs->video_stream_index];

	/* Borrow the error_message field */
	avcodec_string(buf, size, stream->codec, 0);

	int off = strlen(buf);
	off += snprintf(buf+off, size-off, ", %d/%d", stream->time_base.num,
                        stream->time_base.den);

	if (rs->audio_stream_index >= 0 && size - off > 2) {
		stream = rs->ctx->streams[rs->audio_stream_index];
		buf[off++] = ';';
		buf[off++] = ' ';
		avcodec_string(buf+off, size-off, stream->codec, 0);
	}

	buf[size-1] = 0;
	return buf;
}

int rtp_device_decode_video(struct rtp_device *rs, AVFrame *frame)
{
	AVStream *stream;
	int re;
	int have_picture = 0;

	if (rs->frame.stream_index != rs->video_stream_index)
		return -1;

	stream = rs->ctx->streams[rs->frame.stream_index];

	if (!stream->codec->codec) {
		AVCodec *codec = avcodec_find_decoder(stream->codec->codec_id);
		AVDictionary *opt = 0;
		av_dict_set(&opt, "threads", "1", 0);

		re = avcodec_open2(stream->codec, codec, &opt);
		av_dict_free(&opt);
		if (re < 0) {
			av_strerror(re, rs->error_message, sizeof(rs->error_message));
			return -1;
		}
	}

	re = avcodec_decode_video2(stream->codec, frame, &have_picture, &rs->frame);
	if (re < 0) {
		av_strerror(re, rs->error_message, sizeof(rs->error_message));
		return -1;
	}

	return have_picture;
}

