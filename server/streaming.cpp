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
	bc_rec->rtsp_stream->sendPackets(buf, buf_size, bc_rec->cur_pkt_flags);
	return buf_size;
}

int bc_streaming_setup(struct bc_record *bc_rec)
{
	AVFormatContext *ctx;
	AVStream *video_st;
	AVCodec *codec;
	AVDictionary *muxer_opts = NULL;
	int ret = -1;
	uint8_t *bufptr;

	if (bc_rec->stream_ctx) {
		bc_rec->log.log(Warning, "bc_streaming_setup() launched on already-setup bc_record");
		return 0;
	}

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

	video_st = avformat_new_stream(ctx, NULL);
	if (!video_st) {
		bc_rec->log.log(Error, "Couldn't add stream to RTP muxer");
		goto error;
	}

	bc_rec->bc->input->properties()->video.apply(video_st->codec);
	bc_rec->log.log(Debug, "time_base: %d/%d", video_st->codec->time_base.num, video_st->codec->time_base.den);

	if (ctx->oformat->flags & AVFMT_GLOBALHEADER)
		video_st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	/* XXX with multiple streams, avformat_write_header will fail. We need multiple contexts
	 * to do that, because the rtp muxer only handles one stream. */

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

	bc_rec->stream_ctx = ctx;
	bc_rec->rtsp_stream = rtsp_stream::create(bc_rec, ctx);

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
	AVFormatContext *ctx = bc_rec->stream_ctx;

	if (!ctx)
		return;

	if (ctx->pb) {
		av_write_trailer(ctx);
		av_free(ctx->pb->buffer);
		av_freep(&ctx->pb);
	}

	avformat_free_context(ctx);
	bc_rec->stream_ctx = NULL;

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

	if (!bc_streaming_is_active(bc_rec) || pkt.type != AVMEDIA_TYPE_VIDEO)
		return 0;

	av_init_packet(&opkt);
	opkt.flags        = pkt.flags;
	opkt.pts          = pkt.pts;
	if (opkt.pts != (int64_t)AV_NOPTS_VALUE) {
		opkt.pts = av_rescale_q(opkt.pts, AV_TIME_BASE_Q,
				bc_rec->stream_ctx->streams[0]->time_base);
	}
	opkt.dts          = opkt.pts;  /* Not safe, but stream_packet doesn't hold dts */

	opkt.data         = const_cast<uint8_t*>(pkt.data());
	opkt.size         = pkt.size;
	opkt.stream_index = 0; /* XXX */

	bc_rec->cur_pkt_flags = pkt.flags;
	bc_rec->pkt_first_chunk = true;

	re = av_write_frame(bc_rec->stream_ctx, &opkt);
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
	return 1;
}

