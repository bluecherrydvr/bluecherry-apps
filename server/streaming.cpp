/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <unistd.h>
#include "bc-server.h"
#include "rtsp.h"

#define RTP_MAX_PACKET_SIZE 1472

int bc_streaming_setup(struct bc_record *bc_rec)
{
	AVFormatContext *ctx;

	if (bc_rec->stream_ctx)
		return 0;

	ctx = bc_rec->stream_ctx = avformat_alloc_context();
	if (!ctx)
		return -1;

	ctx->oformat = av_guess_format("rtp", NULL, NULL);
	if (!ctx->oformat)
		goto error;

	if (setup_output_context(bc_rec, ctx) < 0)
		goto error;

	for (unsigned int i = 0; i < ctx->nb_streams; ++i) {
		AVStream *st = ctx->streams[i];
		AVCodec *codec = avcodec_find_encoder(st->codec->codec_id);
		if (!codec || codec->type != AVMEDIA_TYPE_VIDEO) {
			/* Soft failure; remove this stream. We currently only support one video stream. */
			av_freep(&st->codec);
			av_freep(&st);

			for (unsigned int j = i+1; j < ctx->nb_streams; ++j) {
				ctx->streams[j]->index = j-1;
				ctx->streams[j-1] = ctx->streams[j];
			}

			ctx->nb_streams--;
			ctx->streams[ctx->nb_streams] = 0;
			i--;
		} else if (avcodec_open2(st->codec, codec, NULL) < 0)
			goto error;
	}

	/* XXX with multiple streams, avformat_write_header will fail. We need multiple contexts
	 * to do that, because the rtp muxer only handles one stream. */
	if (ctx->nb_streams < 1)
		goto error;

	url_open_dyn_packet_buf(&ctx->pb, RTP_MAX_PACKET_SIZE);

	{
		int ret = avformat_write_header(ctx, NULL);
		if (ret < 0) {
			char error[512];
			av_strerror(ret, error, sizeof(error));
			bc_dev_err(bc_rec, "Failed to start live stream: %s", error);
			bc_streaming_destroy(bc_rec);
			return ret;
		}
	}

	bc_rec->rtsp_stream = rtsp_stream::create(bc_rec, ctx);

	return 0;

error:
	bc_streaming_destroy(bc_rec);
	return -1;
}

void bc_streaming_destroy(struct bc_record *bc_rec)
{
	AVFormatContext *ctx = bc_rec->stream_ctx;

	if (!ctx)
		return;

	if (ctx->pb) {
		av_write_trailer(ctx);
		uint8_t *buf = 0;
		avio_close_dyn_buf(ctx->pb, &buf);
		av_free(buf);
	}

	for (unsigned int i = 0; i < ctx->nb_streams; ++i) {
		avcodec_close(ctx->streams[i]->codec);
		av_freep(&ctx->streams[i]->codec);
		av_freep(&ctx->streams[i]);
	}

	av_free(ctx);
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

/* XXX can this be merged with bc_output_packet_write? */
int bc_streaming_packet_write(struct bc_record *bc_rec, struct bc_output_packet *pkt)
{
	AVPacket opkt;
	int re;

	if (!bc_streaming_is_active(bc_rec))
		return 0;

	av_init_packet(&opkt);
	opkt.flags        = pkt->flags;
	opkt.pts          = pkt->pts;
	opkt.data         = (uint8_t*)pkt->data;
	opkt.size         = pkt->size;
	opkt.stream_index = 0; /* XXX */

	re = av_write_frame(bc_rec->stream_ctx, &opkt);
	if (re != 0) {
		char err[512] = { 0 };
		av_strerror(re, err, sizeof(err));
		bc_dev_err(bc_rec, "Error writing to live stream: %s", err);
		return -1;
	}

	uint8_t *buf = 0;
	int bufsz = avio_close_dyn_buf(bc_rec->stream_ctx->pb, &buf);

	bc_rec->rtsp_stream->sendPackets(buf, bufsz, opkt.flags);
	av_free(buf);
	url_open_dyn_packet_buf(&bc_rec->stream_ctx->pb, RTP_MAX_PACKET_SIZE);

	return 1;
}

