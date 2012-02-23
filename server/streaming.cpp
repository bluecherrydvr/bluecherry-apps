/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <unistd.h>
#include "bc-server.h"

int bc_streaming_setup(struct bc_record *bc_rec)
{
	AVFormatContext *ctx;
	AVCodec *codec;
	int i;

	if (bc_rec->stream_ctx)
		return 0;

	ctx = bc_rec->stream_ctx = avformat_alloc_context();
	if (!ctx)
		return -1;

	ctx->oformat = av_guess_format("rtsp", NULL, NULL);
	if (!ctx->oformat)
		goto error;

	for (i = 0; i < bc_rec->oc->nb_streams; ++i) {
		AVCodecContext *ic = bc_rec->oc->streams[i]->codec;
		AVStream *st;

		/* XXX video only */
		if (ic->codec_type != AVMEDIA_TYPE_VIDEO)
			continue;

		st = avformat_new_stream(ctx, NULL);

		/* Copied from rtp_device_setup_output; video only. Won't work for audio. */ 
		st->codec->codec_id = ic->codec_id;
		st->codec->codec_type = ic->codec_type;
		st->codec->pix_fmt = ic->pix_fmt;
		st->codec->width = ic->width;
		st->codec->height = ic->height;
		st->codec->time_base = ic->time_base;
		st->codec->profile = ic->profile;
		if (ic->extradata && ic->extradata_size) {
			st->codec->extradata_size = ic->extradata_size;
			st->codec->extradata = (uint8_t*)av_malloc(ic->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
			memcpy(st->codec->extradata, ic->extradata, ic->extradata_size);
		}

		if (ctx->oformat->flags & AVFMT_GLOBALHEADER)
			st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

		codec = avcodec_find_encoder(st->codec->codec_id);
		if (codec == NULL || avcodec_open2(st->codec, codec, NULL) < 0)
			goto error;
	}

	snprintf(ctx->filename, sizeof(ctx->filename), "rtsp://127.0.0.1:5545/live_%d", bc_rec->id);

	if ((i = avformat_write_header(ctx, NULL)) < 0) {
		char error[512];
		av_strerror(i, error, sizeof(error));
		bc_dev_err(bc_rec, "Failed to start live stream to %s: %s", ctx->filename, error);
		return i;
	}

	char sdp[2048];
	if ((i = av_sdp_create(&ctx, 1, sdp, sizeof(sdp))) < 0) {
		char error[512];
		av_strerror(i, error, sizeof(error));
		bc_dev_err(bc_rec, "Cannot create SDP: %s", error);
	}
	else
		bc_dev_info(bc_rec, "SDP: %s", sdp);

	bc_dev_info(bc_rec, "Opened stream to %s", ctx->filename);
	return 0;

error:
	bc_streaming_destroy(bc_rec);
	return -1;
}

void bc_streaming_destroy(struct bc_record *bc_rec)
{
	AVFormatContext *ctx = bc_rec->stream_ctx;
	int i;

	if (!ctx)
		return;

	if (ctx->pb)
		av_write_trailer(ctx);

	for (i = 0; i < ctx->nb_streams; ++i) {
		avcodec_close(ctx->streams[i]->codec);
		av_freep(&ctx->streams[i]->codec);
		av_freep(&ctx->streams[i]);
	}

	if (ctx->pb)
		avio_close(ctx->pb);

	av_free(ctx);
	bc_rec->stream_ctx = NULL;
}

int bc_streaming_is_active(struct bc_record *bc_rec)
{
	return bc_rec->stream_ctx != NULL;
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

	return 1;
}

