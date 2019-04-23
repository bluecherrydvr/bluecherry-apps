/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
	frame.data = NULL;
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
				stream->discard = AVDISCARD_ALL;
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
				stream->discard = AVDISCARD_ALL;
				continue;
			} else if (stream->codec->codec_id == AV_CODEC_ID_NONE) {
				bc_log(Error, "No matching audio codec for stream; ignoring audio");
				stream->discard = AVDISCARD_ALL;
				continue;
			}
			audio_stream_index = i;
		} else {
				stream->discard = AVDISCARD_ALL;
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
	av_free_packet(pkt);
}

int lavf_device::read_packet()
{
	int re;
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
	current_packet.pts      = av_rescale_q_rnd(src->pts,
			ctx->streams[src->stream_index]->time_base, AV_TIME_BASE_Q,
			(enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
	current_packet.dts      = av_rescale_q_rnd(src->dts,
			ctx->streams[src->stream_index]->time_base, AV_TIME_BASE_Q,
			(enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
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
		p->audio.bits_per_coded_sample = ic->bits_per_coded_sample;
		if (ic->extradata && ic->extradata_size)
			p->audio.extradata.assign(ic->extradata, ic->extradata + ic->extradata_size);
	}

	current_properties = std::shared_ptr<stream_properties>(p);
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

void lavf_device::getStatusXml(pugi::xml_node& xmlnode)
{
	// AVFormatContext info
	if (ctx && ctx->iformat && ctx->nb_streams) {
		// TODO FIXME Unguarded access, may crash if managing thread releases it meanwhile
		unsigned i;
		pugi::xml_node av_ctx = xmlnode.append_child("AVFormatContext");
		av_ctx.append_attribute("Format") = ctx->iformat->name;
		pugi::xml_node es_list = av_ctx.append_child("ElementaryStreams");
		for (i = 0; i < ctx->nb_streams; i++) {
			AVStream *s = ctx->streams[i];
			char codec_descr[100];
			avcodec_string(codec_descr, sizeof(codec_descr), s->codec, 0);

			pugi::xml_node es = es_list.append_child("ElementaryStream");
			es.append_attribute("index") = i;
			es.append_attribute("codec_type") = av_get_media_type_string(s->codec->codec_type);

			es.append_attribute("codec") = avcodec_get_name(s->codec->codec_id);
			es.append_attribute("description") = codec_descr;

			switch (s->codec->codec_type) {
				case AVMEDIA_TYPE_VIDEO: {
					es.append_attribute("height") = s->codec->height;
					es.append_attribute("width") = s->codec->width;
					if (s->sample_aspect_ratio.num && s->sample_aspect_ratio.den) {
						es.append_attribute("sample_aspect_ratio_num") = s->sample_aspect_ratio.num;
						es.append_attribute("sample_aspect_ratio_den") = s->sample_aspect_ratio.den;
						AVRational display_aspect_ratio;
						av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den,
								s->codec->width  * s->sample_aspect_ratio.num,
								s->codec->height * s->sample_aspect_ratio.den,
								1024 * 1024);
						es.append_attribute("display_aspect_ratio_num") = display_aspect_ratio.num;
						es.append_attribute("display_aspect_ratio_den") = display_aspect_ratio.den;
					}
					if (s->avg_frame_rate.den && s->avg_frame_rate.num)
						es.append_attribute("avg_frame_rate") = av_q2d(s->avg_frame_rate);
					break;
				}
				case AVMEDIA_TYPE_AUDIO: {
					es.append_attribute("sample_rate") = s->codec->sample_rate;
					es.append_attribute("channels") = s->codec->channels;
					char channel_layout_descr[50];
					av_get_channel_layout_string(channel_layout_descr, sizeof(channel_layout_descr),
							s->codec->channels, s->codec->channel_layout);
					es.append_attribute("channel_layout") = channel_layout_descr;
					break;
				}
				default: break;
			}
		}
	}
}
