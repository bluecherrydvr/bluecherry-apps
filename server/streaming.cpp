/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <unistd.h>
#include "bc-server.h"
#include "rtsp.h"

extern "C" {
#include <libavutil/mathematics.h>
#include <libavutil/dict.h>
}

#define RTP_MAX_PACKET_SIZE 1472

static int io_write(void *opaque, uint8_t *buf, int buf_size)
{
	struct bc_record *bc_rec = (struct bc_record *)opaque;
	bc_rec->rtsp_stream->sendPackets(buf, buf_size, bc_rec->cur_pkt_flags,
		bc_rec->cur_stream_index);
	return buf_size;
}

static int bc_streaming_setup_elementary(struct bc_record *bc_rec, std::shared_ptr<const stream_properties> props,
	int index, enum AVMediaType type);

int bc_streaming_setup(struct bc_record *bc_rec, std::shared_ptr<const stream_properties> props)
{
	int ret = 0;
	int streams_num = 2;

	if (bc_rec->stream_ctx[0]) {
		bc_rec->log.log(Warning, "bc_streaming_setup() launched on already-setup bc_record");
		return 0;
	}

	ret |= bc_streaming_setup_elementary(bc_rec, props, 0, AVMEDIA_TYPE_VIDEO);
	ret |= bc_streaming_setup_elementary(bc_rec, props, 1, AVMEDIA_TYPE_SUBTITLE);

	if (bc_rec->bc->input->has_audio()) {
		ret |= bc_streaming_setup_elementary(bc_rec, props, 2, AVMEDIA_TYPE_AUDIO);
        streams_num++;
    }

	if (bc_rec->bc->input->has_subtitles()) {
		ret |= bc_streaming_setup_elementary(bc_rec, props, streams_num, AVMEDIA_TYPE_SUBTITLE);
        streams_num++;
    }

	if (ret)
		return ret;

	bc_rec->rtsp_stream = rtsp_stream::create(bc_rec, bc_rec->stream_ctx, streams_num);

	return 0;
}

static int bc_streaming_setup_elementary(struct bc_record *bc_rec, std::shared_ptr<const stream_properties> props,
	int index, enum AVMediaType type)
{
	AVFormatContext *ctx;
	AVStream *st;
	AVCodec *codec;
	AVDictionary *muxer_opts = NULL;
	int ret = -1;
	uint8_t *bufptr;
	AVRational bkp_ts;

	ctx = avformat_alloc_context();
	if (!ctx) {
		ret = AVERROR(ENOMEM);
		goto error;
	}

	ctx->oformat = av_guess_format("rtp", NULL, NULL);
	if (!ctx->oformat) {
		bc_rec->log.log(Error, "RTP output format not available");
		goto error;
	}

	st = avformat_new_stream(ctx, NULL);
	if (!st) {
		bc_rec->log.log(Error, "Couldn't add stream to RTP muxer");
		goto error;
	}

	bkp_ts = st->codec->time_base;
	if (type == AVMEDIA_TYPE_VIDEO)
		props->video.apply(st->codec);
	else if (type == AVMEDIA_TYPE_AUDIO)
		props->audio.apply(st->codec);
	else if (type == AVMEDIA_TYPE_SUBTITLE) {
		//bc_rec->bc->input->properties()->subs.apply(st->codec);
		//bc_rec->bc->input->properties()->audio.apply(st->codec);
		//st->codec->width = 240;
		//st->codec->height = 960;
		/* setup fake audio stream that carries subtitles in payload */
		st->codec->codec_id = AV_CODEC_ID_AAC;
		st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
		st->codec->bit_rate = 0;
		st->codec->sample_rate = (index == 1 ? 48002 : 48001);
		st->codec->sample_fmt = AV_SAMPLE_FMT_FLTP;
		st->codec->channels = 1;
		st->codec->time_base = (AVRational){1, 48000};
		st->codec->profile = 1;
		st->codec->bits_per_coded_sample = 0;
		st->codec->extradata_size = 2;
		st->codec->extradata = (uint8_t*)av_malloc(2 + AV_INPUT_BUFFER_PADDING_SIZE);;//"\021\210"
		memcpy(st->codec->extradata, "\021\210", 2);

        }

	st->codec->time_base = bkp_ts;
	bc_rec->log.log(Debug, "streaming ctx[%d] time_base: %d/%d", index,
		st->codec->time_base.num, st->codec->time_base.den);

	if (ctx->oformat->flags & AVFMT_GLOBALHEADER)
		st->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	ctx->packet_size = RTP_MAX_PACKET_SIZE;

	// This I/O context will call our functions to output the byte stream
	bufptr = (unsigned char *)av_malloc(RTP_MAX_PACKET_SIZE);
	if (!bufptr) {
		ret = AVERROR(ENOMEM);
		goto error;
	}

	ctx->pb = avio_alloc_context(/* buffer and its size */bufptr, RTP_MAX_PACKET_SIZE,
			/* write flag */1,
			/* context pointer passed to functions */bc_rec,
			/* read function */NULL,
			/* write function */io_write,
			/* seek function */NULL);
	bufptr = NULL;  // ctx->pb now owns it
	if (!ctx->pb) {
		fprintf(stderr, "Failed to allocate I/O context\n");
		goto error;
	}
	ctx->pb->seekable = 0;  // Adjust accordingly

	av_dict_set(&muxer_opts, "rtpflags", "skip_rtcp", 0);
	if ((ret = avformat_write_header(ctx, &muxer_opts)) < 0) {
		bc_rec->log.log(Error, "Initializing muxer for RTP streaming failed");
		goto mux_open_error;
	}

	bc_rec->stream_ctx[index] = ctx;

	return 0;

mux_open_error:
	av_free(bufptr);
	av_free(ctx->pb->buffer);
	av_freep(&ctx->pb);
error:
	avformat_free_context(ctx);

	char errbuf[100];
	av_strerror(ret, errbuf, sizeof(errbuf));
	bc_rec->log.log(Error, "bc_streaming_setup() failed: %s", errbuf);
	return ret;
}

void bc_streaming_destroy(struct bc_record *bc_rec)
{
	for (int i = 0; i < 4; i++) {
		AVFormatContext *ctx = bc_rec->stream_ctx[i];

		if (!ctx)
			continue;

		if (ctx->pb) {
			av_write_trailer(ctx);
			av_free(ctx->pb->buffer);
			av_freep(&ctx->pb);
		}

		avformat_free_context(ctx);
		bc_rec->stream_ctx[i] = NULL;
	}

	rtsp_stream::remove(bc_rec);
	bc_rec->rtsp_stream = NULL;
}

int bc_streaming_is_setup(struct bc_record *bc_rec)
{
	return bc_rec->rtsp_stream != NULL;
}

int bc_streaming_is_active(struct bc_record *bc_rec)
{
	return (bc_rec->rtsp_stream && bc_rec->rtsp_stream->activeSessionCount());
}

int bc_streaming_packet_write(struct bc_record *bc_rec, const stream_packet &pkt)
{
	AVPacket opkt;
	int re;
	int ctx_index;

	switch(pkt.type) {
	case AVMEDIA_TYPE_VIDEO:
		ctx_index = 0;
		break;

	case AVMEDIA_TYPE_AUDIO:
		ctx_index = 2;
		break;

	case AVMEDIA_TYPE_SUBTITLE:
		if (bc_rec->stream_ctx[3])
			ctx_index = 3;
		else
			ctx_index = 2;
		break;

		/* Makes subtitle packets pass in keyframe-only mode */
		pkt.flags |= stream_packet::KeyframeFlag;

	default:
		return 0;
	}

	if (!bc_streaming_is_active(bc_rec))
		return 0;

	av_init_packet(&opkt);
	opkt.flags        = pkt.flags;
	opkt.pts          = av_rescale_q_rnd(pkt.pts, AV_TIME_BASE_Q, bc_rec->stream_ctx[ctx_index]->streams[0]->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
	opkt.dts          = av_rescale_q_rnd(pkt.dts, AV_TIME_BASE_Q, bc_rec->stream_ctx[ctx_index]->streams[0]->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));

	opkt.data         = const_cast<uint8_t*>(pkt.data());
	opkt.size         = pkt.size;
	opkt.stream_index = 0;

	bc_rec->cur_pkt_flags = pkt.flags;
	bc_rec->cur_stream_index = ctx_index;
	bc_rec->pkt_first_chunk = true;

	bc_rec->log.log(Debug, "streaming packet for stream %d with flags set to %x dts = %" PRId64 ", pts = %" PRId64, ctx_index,  pkt.flags, opkt.dts, opkt.pts);

	if (pkt.type == AVMEDIA_TYPE_SUBTITLE)
		bc_rec->last_sub_pts = opkt.pts;

	//write_packet:
	re = av_write_frame(bc_rec->stream_ctx[ctx_index], &opkt);
	if (re < 0) {
		if (re == AVERROR(EINVAL)) {
			bc_rec->log.log(Warning, "Likely timestamping error. Ignoring.");
			return 1;
		}
		char err[512] = { 0 };
		av_strerror(re, err, sizeof(err));
		bc_rec->log.log(Error, "Can't write to live stream: %s", err);

		/* Cleanup */
		stop_handle_properly(bc_rec);
		return -1;
	}

	if (pkt.type == AVMEDIA_TYPE_VIDEO && (pkt.flags & stream_packet::MotionFlag || bc_rec->motion_flag)) {//send motion event to client
		ctx_index = 1;
		bc_rec->cur_stream_index = ctx_index;

		opkt.size = 1;
		opkt.pts = bc_rec->last_sub_pts + 1;
		opkt.dts = bc_rec->last_sub_pts + 1;
		bc_rec->log.log(Info, "sending motion popup trigger packet dts = %" PRId64 ", pts = %" PRId64, opkt.dts, opkt.pts);

		re = av_write_frame(bc_rec->stream_ctx[ctx_index], &opkt);
		if (re < 0) {
			char err[512] = { 0 };
	                av_strerror(re, err, sizeof(err));
			bc_rec->log.log(Error, "Can't write popup trigger packet to live stream: %s", err);
		}
		bc_rec->motion_flag = 0;
	}

	return 1;
}

