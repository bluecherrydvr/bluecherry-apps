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

lavf_device::lavf_device(const char *u, int rtp_protocol)
	: ctx(0), video_stream_index(-1), audio_stream_index(-1)
		, rtp_protocol(rtp_protocol)
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
	av_packet_unref(&frame);

	// Ensure proper cleanup of the format context
	if (ctx) {
		avformat_close_input(&ctx);
		ctx = NULL;
	}
	
	video_stream_index = audio_stream_index = -1;
}

int lavf_device::start()
{
	// Ensure clean state before starting
	stop();

	if (ctx) {
		bc_log(Error, "Format context not properly cleaned up");
		return -1;
	}

	// Initialize format context to NULL
	ctx = NULL;

	// Common options for all stream types
	AVDictionary *avopt_open_input = NULL;
	const AVInputFormat *input_fmt = NULL;
	int re = -1;  // Move re declaration to outer scope

	bc_log(Debug, "Opening session from URL: %s", url);

	if (!strncmp(url, "rtsp://", 7))
	{
		// Check if URL is local network (private IP range)
		bool is_local = false;
		bool is_reolink = false;
		char host[256] = {0};
		int port = 0;
		char scheme[8] = {0};
		char auth[256] = {0};
		char path[1024] = {0};
		
		// Parse URL with more detailed error handling
		av_url_split(scheme, sizeof(scheme),
					auth, sizeof(auth),
					host, sizeof(host),
					&port,
					path, sizeof(path),
					url);
				
		bc_log(Debug, "Parsed RTSP URL - Scheme: %s, Host: %s, Port: %d, Path: %s", 
			   scheme, host, port, path);
			
		if (strlen(host) == 0) {
			bc_log(Error, "Invalid RTSP URL: Could not parse host from %s", url);
			return -1;
		}
		
		if (port <= 0) {
			port = 554; // Default RTSP port
			bc_log(Debug, "No port specified in URL, using default RTSP port 554");
		}

		unsigned int ip[4];
		if (sscanf(host, "%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]) == 4) {
			// Check for private IP ranges
			is_local = (ip[0] == 10) || 
					  (ip[0] == 172 && ip[1] >= 16 && ip[1] <= 31) ||
					  (ip[0] == 192 && ip[1] == 168);
			bc_log(Debug, "IP address %s is %s", host, is_local ? "local" : "remote");
		} else {
			bc_log(Debug, "Host %s is not an IP address, assuming remote");
		}
		
		// Check if this is a Reolink camera by looking for common Reolink paths
		is_reolink = (strstr(path, "/h264Preview") != NULL || 
					 strstr(path, "/h264") != NULL ||
					 strstr(path, "/h265") != NULL);
		if (is_reolink) {
			bc_log(Debug, "Detected Reolink camera based on path pattern");
		}

		// Try different transport protocols
		const char *transports[] = {"tcp", "udp"};
		for (int i = 0; i < 2 && re != 0; i++) {
			// Clean up any existing context before retry
			if (ctx) {
				avformat_close_input(&ctx);
				ctx = NULL;
			}

			// Set up options for this attempt
			av_dict_free(&avopt_open_input);
			avopt_open_input = NULL;
			
			// Set transport protocol
			av_dict_set(&avopt_open_input, "rtsp_transport", transports[i], 0);
			
			// Set timeout options
			av_dict_set(&avopt_open_input, "stimeout", "3000000", 0);  // 3 seconds in microseconds
			av_dict_set(&avopt_open_input, "timeout", "3000000", 0);   // 3 seconds in microseconds
			
			// Additional options for better stability and faster setup
			av_dict_set(&avopt_open_input, "reorder_queue_size", "0", 0);
			av_dict_set(&avopt_open_input, "max_delay", "300000", 0);  // 300ms max delay
			av_dict_set(&avopt_open_input, "probesize", "500000", 0);  // Smaller probe size for faster analysis
			av_dict_set(&avopt_open_input, "analyzeduration", "1000000", 0); // 1 second analyze duration
			
			// Try to open the input
			re = avformat_open_input(&ctx, url, NULL, &avopt_open_input);
			
			// Clean up options dictionary
			av_dict_free(&avopt_open_input);

			if (re != 0) {
				char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
				av_strerror(re, errbuf, sizeof(errbuf));
				bc_log(Info, "%s connection failed (%s)", transports[i], errbuf);
				
				// Clean up failed context
				if (ctx) {
					avformat_close_input(&ctx);
					ctx = NULL;
				}
			}
		}

		if (re != 0) {
			char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
			av_strerror(re, errbuf, sizeof(errbuf));
			bc_log(Error, "Failed to open RTSP stream %s. Error: %d (%s)", url, re, errbuf);
			if (ctx) {
				avformat_close_input(&ctx);
				ctx = NULL;
			}
			return -1;
		}

		// Verify the context is valid
		if (!ctx || !ctx->iformat) {
			bc_log(Error, "Invalid format context after opening stream");
			if (ctx) {
				avformat_close_input(&ctx);
				ctx = NULL;
			}
			return -1;
		}

		// Set up stream info
		AVDictionary *avopt_find_stream_info = NULL;
		av_dict_set(&avopt_find_stream_info, "threads", "1", 0);

		/* avformat_find_stream_info takes an array of AVDictionary ptrs for each stream */
		AVDictionary **opt_si = new AVDictionary*[ctx->nb_streams];

		for (unsigned int i = 0; i < ctx->nb_streams; ++i) {
			opt_si[i] = NULL;
			av_dict_copy(&opt_si[i], avopt_find_stream_info, 0);
		}

		re = avformat_find_stream_info(ctx, opt_si);

		// Clean up stream info options
		for (unsigned int i = 0; i < ctx->nb_streams; ++i) {
			if (opt_si[i]) {
				av_dict_free(&opt_si[i]);
			}
		}
		delete[] opt_si;
		av_dict_free(&avopt_find_stream_info);

		if (re < 0) {
			stop();
			av_strerror(re, error_message, sizeof(error_message));
			bc_log(Error, "Failed to analyze input stream. Error: %d (%s)", re, error_message);
			return -1;
		}
	}
	else if (!strncmp(url, "http://", 7))
	{
		/* For MJPEG streams, generate timestamps from system time */
		av_dict_set(&avopt_open_input, "use_wallclock_as_timestamps", "1", 0);
		/* Declare format explicitly, as auto-recognition of MJPEG doesn't work on some models */
		input_fmt = av_find_input_format("mjpeg");

		if (!input_fmt)
		{
			assert(0);
			return -1;
		}

		re = avformat_open_input(&ctx, url, input_fmt, &avopt_open_input);
		av_dict_free(&avopt_open_input);

		if (re != 0)
		{
			av_strerror(re, error_message, sizeof(error_message));
			bc_log(Error, "Failed to open stream. Error: %d (%s)", re, error_message);
			ctx = NULL;
			return -1;
		}

		// Set up stream info for HTTP streams
		AVDictionary *avopt_find_stream_info = NULL;
		av_dict_set(&avopt_find_stream_info, "threads", "1", 0);

		/* avformat_find_stream_info takes an array of AVDictionary ptrs for each stream */
		AVDictionary **opt_si = new AVDictionary*[ctx->nb_streams];

		for (unsigned int i = 0; i < ctx->nb_streams; ++i) {
			opt_si[i] = NULL;
			av_dict_copy(&opt_si[i], avopt_find_stream_info, 0);
		}

		re = avformat_find_stream_info(ctx, opt_si);

		// Clean up stream info options
		for (unsigned int i = 0; i < ctx->nb_streams; ++i) {
			if (opt_si[i]) {
				av_dict_free(&opt_si[i]);
			}
		}
		delete[] opt_si;
		av_dict_free(&avopt_find_stream_info);

		if (re < 0) {
			stop();
			av_strerror(re, error_message, sizeof(error_message));
			bc_log(Error, "Failed to analyze input stream. Error: %d (%s)", re, error_message);
			return -1;
		}
	}

	// Process streams
	for (unsigned int i = 0; i < ctx->nb_streams && i < MAX_STREAMS; ++i)
	{
		AVStream *stream = ctx->streams[i];

		if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			if (video_stream_index >= 0)
			{
				bc_log(Warning, "Session for %s has multiple video streams. Only the "
					   "first stream will be recorded.", url);
				stream->discard = AVDISCARD_ALL;
				continue;
			}

			video_stream_index = i;

			if (stream->time_base.num != 1 || stream->time_base.den != 90000)
			{
				bc_log(Info, "Video stream timebase is unusual (%d/%d). This could cause "
					   "timing issues.", stream->time_base.num, stream->time_base.den);
			}
		}
		else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			if (audio_stream_index >= 0)
			{
				bc_log(Warning, "Session for %s has multiple audio streams. Only the "
						"first stream will be recorded.", url);

				stream->discard = AVDISCARD_ALL;
				continue;
			}
			else if (stream->codecpar->codec_id == AV_CODEC_ID_NONE)
			{
				bc_log(Error, "No matching audio codec for stream; ignoring audio");
				stream->discard = AVDISCARD_ALL;
				continue;
			}

			audio_stream_index = i;
		}
		else
		{
			stream->discard = AVDISCARD_ALL;
		}
	}

	if (video_stream_index < 0)
	{
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
	av_packet_unref(pkt);
}

int lavf_device::read_packet()
{
	int re;
	if (!ctx) {
		strcpy(error_message, "No active session");
		return -1;
	}

	av_packet_unref(&frame);
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
	uint8_t *buf = new uint8_t[src->size + AV_INPUT_BUFFER_PADDING_SIZE];
	/* XXX The padding is a hack to avoid overreads by optimized
	 * functions. */
	memcpy(buf, src->data, src->size);

	current_packet = stream_packet(buf, current_properties);
	current_packet.seq      = next_packet_seq++;
	current_packet.size     = src->size;
	current_packet.ts_clock = time(NULL);
	
	// Get stream timebase
	AVRational tb = ctx->streams[src->stream_index]->time_base;
	
	// Calculate PTS and DTS with proper timebase conversion
	int64_t pts = av_rescale_q_rnd(src->pts, tb, AV_TIME_BASE_Q,
			(enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
	int64_t dts = av_rescale_q_rnd(src->dts, tb, AV_TIME_BASE_Q,
			(enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));

	// Only adjust DTS for VBR streams (bit_rate == 0)
	if (src->stream_index == video_stream_index) {
		AVCodecParameters *codecpar = ctx->streams[video_stream_index]->codecpar;
		static int64_t last_dts = 0;
		if (codecpar->bit_rate == 0 && dts == last_dts) {
			// For VBR streams, increment DTS by a small amount
			dts = last_dts + 1;
			bc_log(Debug, "VBR stream detected - adjusted DTS from %ld to %ld", last_dts, dts);
		}
		last_dts = dts;
	}
	
	current_packet.pts = pts;
	current_packet.dts = dts;
	
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
		AVCodecParameters *ic = ctx->streams[video_stream_index]->codecpar;
		AVRational tb = ctx->streams[video_stream_index]->time_base;
		p->video.codec_id = ic->codec_id;
		p->video.pix_fmt = (AVPixelFormat)ic->format;
		p->video.width = ic->width;
		p->video.height = ic->height;
		p->video.bit_rate = ic->bit_rate;
		p->video.time_base = tb;
		p->video.profile = ic->profile;
		if (ic->extradata && ic->extradata_size)
			p->video.extradata.assign(ic->extradata, ic->extradata + ic->extradata_size);
	}

	if (audio_stream_index >= 0) {
		AVCodecParameters *ic = ctx->streams[audio_stream_index]->codecpar;
		p->audio.codec_id = ic->codec_id;
		p->audio.bit_rate = ic->bit_rate;
		p->audio.sample_rate = ic->sample_rate;
		p->audio.sample_fmt = (AVSampleFormat)ic->format;
		// Use new channel layout API
		p->audio.channels = ic->ch_layout.nb_channels;
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
	const AVCodecDescriptor *desc = avcodec_descriptor_get(stream->codecpar->codec_id);
	if (desc == NULL) snprintf(buf, size, "Unknown");
	else snprintf(buf, size, "%s", desc->name);

	int off = strlen(buf);
	off += snprintf(buf+off, size-off, ", %d/%d(s) %d/%d(c)", stream->time_base.num,
                stream->time_base.den, stream->time_base.num, stream->time_base.den);

	if (audio_stream_index >= 0 && size - off > 2) {
		stream = ctx->streams[audio_stream_index];
		buf[off++] = ';';
		buf[off++] = ' ';

		const AVCodecDescriptor *adesc = avcodec_descriptor_get(stream->codecpar->codec_id);
		if (adesc == NULL) snprintf(buf+off, size-off, "Unknown");
		else snprintf(buf+off, size-off, "%s", adesc->name);
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

			const AVCodecDescriptor *desc = avcodec_descriptor_get(s->codecpar->codec_id);
			if (desc == NULL) snprintf(codec_descr, sizeof(codec_descr), "Unknown");
			else snprintf(codec_descr, sizeof(codec_descr), "%s", desc->name);

			pugi::xml_node es = es_list.append_child("ElementaryStream");
			es.append_attribute("index") = i;
			es.append_attribute("codec_type") = av_get_media_type_string(s->codecpar->codec_type);

			es.append_attribute("codec") = avcodec_get_name(s->codecpar->codec_id);
			es.append_attribute("description") = codec_descr;

			switch (s->codecpar->codec_type) {
				case AVMEDIA_TYPE_VIDEO: {
					es.append_attribute("height") = s->codecpar->height;
					es.append_attribute("width") = s->codecpar->width;
					if (s->sample_aspect_ratio.num && s->sample_aspect_ratio.den) {
						es.append_attribute("sample_aspect_ratio_num") = s->sample_aspect_ratio.num;
						es.append_attribute("sample_aspect_ratio_den") = s->sample_aspect_ratio.den;
						AVRational display_aspect_ratio;
						av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den,
								s->codecpar->width  * s->sample_aspect_ratio.num,
								s->codecpar->height * s->sample_aspect_ratio.den,
								1024 * 1024);
						es.append_attribute("display_aspect_ratio_num") = display_aspect_ratio.num;
						es.append_attribute("display_aspect_ratio_den") = display_aspect_ratio.den;
					}
					if (s->avg_frame_rate.den && s->avg_frame_rate.num)
						es.append_attribute("avg_frame_rate") = av_q2d(s->avg_frame_rate);
					break;
				}
				case AVMEDIA_TYPE_AUDIO: {
					es.append_attribute("sample_rate") = s->codecpar->sample_rate;
					// Use new channel layout API
					es.append_attribute("channels") = s->codecpar->ch_layout.nb_channels;
					char channel_layout_descr[50];
					av_channel_layout_describe(&s->codecpar->ch_layout, channel_layout_descr, sizeof(channel_layout_descr));
					es.append_attribute("channel_layout") = channel_layout_descr;
					break;
				}
				default: break;
			}
		}
	}
}
