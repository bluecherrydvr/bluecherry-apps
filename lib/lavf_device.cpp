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
#include <assert.h>
#include <bsd/string.h>

#include "libbluecherry.h"
#include "lavf_device.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
}

lavf_device::lavf_device(const char *u, bool rtp_prefer_tcp)
	: ctx(0), video_stream_index(-1), audio_stream_index(-1)
		, rtp_prefer_tcp(rtp_prefer_tcp)
{
	strlcpy(url, u, sizeof(url));

	memset(error_message, 0, sizeof(error_message));

	/* Currently, av_init_packet cannot be used here because it
	 * is undefined in the php module. We don't really need it,
	 * anyway. */
	memset(&frame, 0, sizeof(frame));
	frame.pts = AV_NOPTS_VALUE;

	memset(&stream_data, 0, sizeof(stream_data));
	for (int i = 0; i < MAX_STREAMS; ++i)
		stream_data[i].last_pts = AV_NOPTS_VALUE;
}

lavf_device::~lavf_device()
{
	stop();
}

void lavf_device::stop()
{
	_started = false;

	if (!ctx)
		return;

	current_packet = stream_packet();
	current_properties.reset();
	av_free_packet(&frame);
	av_init_packet(&frame);

	for (unsigned int i = 0; i < ctx->nb_streams; ++i) {
		if (ctx->streams[i]->codec->codec)
			avcodec_close(ctx->streams[i]->codec);
	}

	avformat_close_input(&ctx);
	ctx = 0;
	video_stream_index = audio_stream_index = -1;

	for (unsigned int i = 0; i < MAX_STREAMS; ++i) {
		stream_data[i].pts_base = 0;
		stream_data[i].last_pts = AV_NOPTS_VALUE;
		stream_data[i].last_pts_diff = 0;
		stream_data[i].was_last_diff_skipped = 0;
	}
}

int lavf_device::start()
{
	int re;
	AVDictionary *avopt_open_input = NULL;
	AVDictionary *avopt_find_stream_info = NULL;
	AVInputFormat *input_fmt = NULL;

	if (ctx)
		return 0;

	bc_log(Debug, "Opening session from URL: %s", url);

	av_dict_set(&avopt_open_input, "max_delay", "700000", 0);
	av_dict_set(&avopt_open_input, "allowed_media_types", audio_enabled() ? "-data" : "-audio-data", 0);
	/* No input on socket, or no writability for thus many microseconds is treated as failure */
	av_dict_set(&avopt_open_input, "stimeout", "10000000" /* 10 s */, 0);
	if (rtp_prefer_tcp && !strncmp(url, "rtsp://", 7))
		av_dict_set(&avopt_open_input, "rtsp_flags", "+prefer_tcp", 0);
	if (!strncmp(url, "http://", 7)) {
		/* For MJPEG streams, generate timestamps from system time */
		av_dict_set(&avopt_open_input, "use_wallclock_as_timestamps", "1", 0);
		/* Declare format explicitly, as auto-recognition of MJPEG doesn't work on some models */
		input_fmt = av_find_input_format("mjpeg");
		if (!input_fmt) {
			assert(0);
			return -1;
		}
	}

	re = avformat_open_input(&ctx, url, input_fmt, &avopt_open_input);
	av_dict_free(&avopt_open_input);
	if (re != 0) {
		av_strerror(re, error_message, sizeof(error_message));
		bc_log(Error, "Failed to open stream. Error: %d (%s)", re, error_message);
		ctx = 0;
		return -1;
	}

	av_dict_set(&avopt_find_stream_info, "threads", "1", 0);
	/* avformat_find_stream_info takes an array of AVDictionary ptrs for each stream */
	AVDictionary **opt_si = new AVDictionary*[ctx->nb_streams];
	for (unsigned int i = 0; i < ctx->nb_streams; ++i) {
		opt_si[i] = 0;
		av_dict_copy(&opt_si[i], avopt_find_stream_info, 0);
	}

	re = avformat_find_stream_info(ctx, opt_si);

	for (unsigned int i = 0; i < ctx->nb_streams; ++i)
		av_dict_free(&opt_si[i]);
	delete[] opt_si;
	av_dict_free(&avopt_find_stream_info);

	if (re < 0) {
		stop();
		av_strerror(re, error_message, sizeof(error_message));
		bc_log(Error, "Failed to analyze input stream. Error: %d (%s)", re, error_message);
		return -1;
	}

	for (unsigned int i = 0; i < ctx->nb_streams && i < MAX_STREAMS; ++i) {
		AVStream *stream = ctx->streams[i];

		if (stream->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			if (video_stream_index >= 0) {
				bc_log(Warning, "Session for %s has multiple video streams. Only the "
				       "first stream will be recorded.", url);
				continue;
			}
			video_stream_index = i;

			if (stream->time_base.num != 1 || stream->time_base.den != 90000) {
				bc_log(Info, "Video stream timebase is unusual (%d/%d). This could cause "
				       "timing issues.", stream->time_base.num, stream->time_base.den);
			}
		} else if (stream->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			if (audio_stream_index >= 0) {
				bc_log(Warning, "Session for %s has multiple audio streams. Only the "
				       "first stream will be recorded.", url);
				continue;
			} else if (stream->codec->codec_id == AV_CODEC_ID_NONE) {
				bc_log(Error, "No matching audio codec for stream; ignoring audio");
				continue;
			}
			audio_stream_index = i;
		}
	}

	if (video_stream_index < 0) {
		stop();
		strcpy(error_message, "Session contains no valid video stream");
		return -1;
	}

	update_properties();
	_started = true;

	return 0;
}

/* Workaround because we cannot take the address of this function
 * without breaking the link with the PHP extension (which does
 * not know of libav). Ugly. */
static void wrap_av_destruct_packet(AVPacket *pkt)
{
	av_destruct_packet(pkt);
}

int lavf_device::read_packet()
{
	int re;
	struct rtp_stream_data *streamdata = 0;
	if (!ctx) {
		strcpy(error_message, "No active session");
		return -1;
	}

	av_free_packet(&frame);
	re = av_read_frame(ctx, &frame);
	if (re < 0) {
		av_strerror(re, error_message, sizeof(error_message));
		return -1;
	}

	if (frame.stream_index >= 0 && frame.stream_index < MAX_STREAMS)
		streamdata = &stream_data[frame.stream_index];

	/* ACTi B2 frames are badly specified; they are MPEG4 user data elements which
	 * almost always contain a sequence of 23 0 bits, which is misinterpreted by
	 * generic MPEG4 parsers. Detect these frames and strip them off to avoid breaking
	 * everything. */
	if (ctx->streams[frame.stream_index]->codec->codec_id == AV_CODEC_ID_MPEG4) {
		const uint8_t b2_header[] = { 0x00, 0x00, 0x01, 0xb2 };
		if (frame.size >= 47 && frame.stream_index == video_stream_index
		    && memcmp(frame.data, b2_header, sizeof(b2_header)) == 0
		    && memcmp(frame.data+44, b2_header, 3) == 0)
		{
			void *tmp = av_malloc(frame.size); /* Let the extra 44 be buffer padding */
			int size = frame.size - 44;
			memcpy(tmp, frame.data+44, size);
			av_free_packet(&frame);
			frame.data = (uint8_t*)tmp;
			frame.size = size;
			frame.destruct = wrap_av_destruct_packet;
		}
	}

	/* Don't run offset logic against frames with no PTS */
	if (!streamdata || frame.pts == (int64_t)AV_NOPTS_VALUE) {
		create_stream_packet(&frame);
		return 0;
	}

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
	if (streamdata->last_pts != (int64_t)AV_NOPTS_VALUE && ctx->nb_streams > 1) {
		if (frame.pts <= streamdata->last_pts || (streamdata->last_pts_diff &&
		    (frame.pts - streamdata->last_pts) >= (streamdata->last_pts_diff*4)))
		{
			bc_log(Debug, "Inconsistent PTS on stream %d (type %d), "
			       "delta %lld. Adjusting based on last interval of %lld.",
			       frame.stream_index,
			       ctx->streams[frame.stream_index]->codec->codec_type,
			       (long long int) frame.pts - streamdata->last_pts,
			       (long long int) streamdata->last_pts_diff);
			streamdata->pts_base -= (streamdata->last_pts - frame.pts)
			                        + streamdata->last_pts_diff;
		} else {
			int64_t newptsdiff = frame.pts - streamdata->last_pts;
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
					bc_log(Debug, "PTS interval on stream %d (type %d) dropped "
					       "to %lld (delta %lld); ignoring interval change unless repeated",
					       frame.stream_index,
					       ctx->streams[frame.stream_index]->codec->codec_type,
					       (long long int)newptsdiff, (long long int)(newptsdiff - streamdata->last_pts_diff));
					streamdata->was_last_diff_skipped = 1;
				} else {
					bc_log(Debug, "PTS interval on stream %d (type %d) dropped "
					       "to %lld (delta %lld) twice; accepting new interval. Could cause "
					       "framerate or desynchronization issues.", frame.stream_index,
					       ctx->streams[frame.stream_index]->codec->codec_type,
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

	streamdata->last_pts = frame.pts;
	frame.pts -= streamdata->pts_base;

	create_stream_packet(&frame);

	return 0;
}

void lavf_device::create_stream_packet(AVPacket *src)
{
	uint8_t *buf = new uint8_t[src->size + FF_INPUT_BUFFER_PADDING_SIZE];
	/* XXX The padding is a hack to avoid overreads by optimized
	 * functions. */
	memcpy(buf, src->data, src->size);

	current_packet = stream_packet(buf, current_properties);
	current_packet.seq      = next_packet_seq++;
	current_packet.size     = src->size;
	current_packet.ts_clock = time(NULL);
	current_packet.pts      = av_rescale_q(src->pts, ctx->streams[src->stream_index]->time_base, AV_TIME_BASE_Q);
	if (src->flags & AV_PKT_FLAG_KEY)
		current_packet.flags |= stream_packet::KeyframeFlag;
	current_packet.ts_monotonic = bc_gettime_monotonic();

	if (src->stream_index == video_stream_index)
		current_packet.type = AVMEDIA_TYPE_VIDEO;
	else if (src->stream_index == audio_stream_index)
		current_packet.type = AVMEDIA_TYPE_AUDIO;
	else
		current_packet.type = AVMEDIA_TYPE_UNKNOWN;
}

void lavf_device::update_properties()
{
	stream_properties *p = new stream_properties;

	if (video_stream_index >= 0) {
		AVCodecContext *ic = ctx->streams[video_stream_index]->codec;
		p->video.codec_id = ic->codec_id;
		p->video.pix_fmt = ic->pix_fmt;
		p->video.width = ic->width;
		p->video.height = ic->height;
		p->video.time_base = ic->time_base;
		p->video.profile = ic->profile;
		if (ic->extradata && ic->extradata_size)
			p->video.extradata.assign(ic->extradata, ic->extradata + ic->extradata_size);
	}

	if (audio_stream_index >= 0) {
		AVCodecContext *ic = ctx->streams[audio_stream_index]->codec;
		p->audio.codec_id = ic->codec_id;
		p->audio.bit_rate = ic->bit_rate;
		p->audio.sample_rate = ic->sample_rate;
		p->audio.sample_fmt = ic->sample_fmt;
		p->audio.channels = ic->channels;
		p->audio.time_base = (AVRational){1, ic->sample_rate};
		p->audio.profile = ic->profile;
		if (ic->extradata && ic->extradata_size)
			p->audio.extradata.assign(ic->extradata, ic->extradata + ic->extradata_size);
	}

	current_properties = std::shared_ptr<stream_properties>(p);
}

void lavf_device::set_current_pts(int64_t pts)
{
	int64_t offset;

	/* Adjust PTS offsets so that the current frame has a PTS of 'pts', and
	 * all other streams are adjusted accordingly. */
	if (frame.stream_index < 0 || frame.stream_index >= MAX_STREAMS)
		return;

	if (frame.pts == (int64_t)AV_NOPTS_VALUE) {
		bc_log(Debug, "Current frame has no PTS, so PTS reset to %" PRId64 " cannot occur", pts);
		return;
	}

	offset = frame.pts - pts;

	bc_log(Debug, "Adjusted pts_base by %" PRId64 " to reset PTS on stream %d to %" PRId64,
	       offset, frame.stream_index, pts);

	for (unsigned int i = 0; i < ctx->nb_streams && i < MAX_STREAMS; ++i) {
		stream_data[i].pts_base += av_rescale_q(offset, ctx->streams[frame.stream_index]->time_base, ctx->streams[i]->time_base);
	}

	frame.pts = pts;
}

const char *lavf_device::stream_info()
{
	char *buf = error_message;
	int size = sizeof(error_message);

	if (video_stream_index < 0) {
		strlcpy(buf, "No streams", size);
		return buf;
	}

	AVStream *stream = ctx->streams[video_stream_index];

	/* Borrow the error_message field */
	avcodec_string(buf, size, stream->codec, 0);

	int off = strlen(buf);
	off += snprintf(buf+off, size-off, ", %d/%d(s) %d/%d(c)", stream->time_base.num,
                        stream->time_base.den, stream->codec->time_base.num,
			stream->codec->time_base.den);

	if (audio_stream_index >= 0 && size - off > 2) {
		stream = ctx->streams[audio_stream_index];
		buf[off++] = ';';
		buf[off++] = ' ';
		avcodec_string(buf+off, size-off, stream->codec, 0);
	}

	buf[size-1] = 0;
	return buf;
}

