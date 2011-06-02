/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include "rtp-session.h"

#define DEFAULT_PORT		"554"
#define DEFAULT_USERINFO	"root:pass"


extern int rtp_verbose;

static int do_raw;

static AVOutputFormat *fmt_out;
static AVStream *vst, *ast;
static AVFormatContext *oc;

static void usage(void)
{
	fprintf(stderr, "Usage: rtsp-client [-v] [-r] rtsp://[USERINFO@]"
		"SERVER[:PORT]/PATH_TO_SDP\n\n");
	exit(EXIT_FAILURE);
}

/* If s2 is found, copy the found string to dest, and return a pointer just past
 * the character. Otherwise, return src. */
static char *strpbrk_and_copy(char *dest, char *src, const char *s2, char *res)
{
	char *p = strpbrk(src, s2);

	if (!p)
		return NULL;

	if (res)
		*res = *p;

	memcpy(dest, src, p - src);
	dest[p - src] = '\0';

	return p + 1;
}

static void check_mpeg(struct rtp_session *rs, int *width, int *height)
{
	AVCodec *codec;
	AVCodecContext *c = NULL;
	int got_picture, len;
	AVFrame *picture = NULL;

	codec = avcodec_find_decoder(rs->vid_codec);
	if (!codec) {
		fprintf(stderr, "codec not found: 0x%08x\n", rs->vid_codec);
		return;
	}

	c = avcodec_alloc_context();
	if (c == NULL) {
		fprintf(stderr, "Failed to alloc context\n");
		return;
	}

	picture = avcodec_alloc_frame();
	if (picture == NULL) {
		fprintf(stderr, "Failed to alloc frame\n");
		goto fail_mpeg_check;
	}

	if (avcodec_open(c, codec) < 0) {
		fprintf(stderr, "could not open codec\n");
		goto fail_mpeg_check;
	}

	len = avcodec_decode_video(c, picture, &got_picture, rs->vid_buf,
				   rs->vid_len);

	if (len < 0) {
		fprintf(stderr, "Error while decoding frame\n");
		goto fail_mpeg_check;
	}

	if (got_picture) {
		if (height)
			*height = c->height;
		if (width)
			*width = c->width;

		fprintf(stderr, "Got frame at %dx%d @ %d fps (codec %s)\n",
			c->width, c->height, rs->framerate,
			rs->vid_codec == CODEC_ID_MPEG4 ? "mpeg4" : "h.264");
	} else {
		fprintf(stderr, "Could not decode frame\n");
	}

fail_mpeg_check:
	if (c) {
		avcodec_close(c);
		av_freep(&c);
	}
	if (picture)
		av_freep(&picture);
}

static void open_av(struct rtp_session *rs)
{
	static const char *out = "temp.mkv";
	int width = 640, height = 480;
	AVCodec *codec;

	if (oc)
		return;

	check_mpeg(rs, &width, &height);

	fmt_out = guess_format(NULL, "temp.mkv", NULL);
	if (!fmt_out) {
		fprintf(stderr, "Could not guess output format\n");
		exit(1);
	}

	oc = avformat_alloc_context();
	if (!oc) {
		fprintf(stderr, "Could not alloc context\n");
		exit(1);
	}

	oc->oformat = fmt_out;
	snprintf(oc->filename, sizeof(oc->filename), "%s", out);

	vst = av_new_stream(oc, 0);
	if (!vst) {
		fprintf(stderr, "Could not add video stream\n");
		exit(1);
	}

	if (rs->vid_codec == CODEC_ID_H264) {
		vst->codec->crf = 20;
		vst->codec->me_range = 16;
		vst->codec->me_subpel_quality = 7;
		vst->codec->qmin = 10;
		vst->codec->qmax = 51;
		vst->codec->max_qdiff = 4;
		vst->codec->qcompress = 0.6;
		vst->codec->i_quant_factor = 0.71;
		vst->codec->b_frame_strategy = 1;
	}
	vst->codec->codec_id = rs->vid_codec;
	vst->codec->codec_type = CODEC_TYPE_VIDEO;
	vst->codec->pix_fmt = PIX_FMT_YUV420P;
	vst->codec->width = width;
	vst->codec->height = height;
	vst->codec->time_base = vst->time_base = (AVRational){ 1, rs->framerate };

	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		vst->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	if (rs->tid_a >= 0 || rs->aud_port >= 0) {
		ast = av_new_stream(oc, 1);
		if (!ast) {
			fprintf(stderr, "Could not add audio stream\n");
			exit(1);
		}

		ast->codec->codec_id = rs->aud_codec;
		ast->codec->codec_type = CODEC_TYPE_AUDIO;
		ast->codec->bit_rate = rs->bitrate;
		ast->codec->sample_rate = rs->samplerate;
		ast->codec->channels = rs->channels;
		ast->codec->time_base = (AVRational){1, rs->samplerate};

		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
			ast->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	if (av_set_parameters(oc, NULL) < 0) {
		fprintf(stderr, "Could not set params\n");
		exit(1);
	}

	codec = avcodec_find_encoder(vst->codec->codec_id);
	if (codec == NULL || avcodec_open(vst->codec, codec) < 0) {
		fprintf(stderr, "Could not open video encoder\n");
		exit(1);
	}

        if (ast) {
                codec = avcodec_find_encoder(ast->codec->codec_id);
                if (codec == NULL || avcodec_open(ast->codec, codec) < 0) {
			fprintf(stderr, "Could not open audio encoder\n");
			exit(1);
		}
	}

	if (url_fopen(&oc->pb, out, URL_WRONLY) < 0) {
		fprintf(stderr, "Could not open outfile\n");
		exit(1);
	}

	av_write_header(oc);
}

static void do_vid_out(struct rtp_session *rs)
{
	if (!rs->vid_valid)
		return;

	if (do_raw) {
		static int fd = -1;
		static int got_one;

		if (fd < 0)
			fd = open("temp.m4v", O_WRONLY | O_CREAT | O_TRUNC, 0644);

		if (fd < 0) {
			fprintf(stderr, "Could not open output file: %m\n");
			exit(EXIT_FAILURE);
		}

		if (write(fd, rs->vid_buf, rs->vid_len) != rs->vid_len) {
			fprintf(stderr, "Could not write video data: %m\n");
			exit(EXIT_FAILURE);
		}
		if (!got_one) {
			got_one = 1;
			check_mpeg(rs, NULL, NULL);
		}
	} else {
		static AVRational tb = (AVRational){ 1, 90000 };
		AVPacket pkt;

		open_av(rs);

		av_init_packet(&pkt);

		pkt.data = rs->vid_buf;
		pkt.size = rs->vid_len;
		pkt.pts = av_rescale_q(rs->vid_ts, tb, vst->time_base);
		pkt.stream_index = vst->index;

		if (av_write_frame(oc, &pkt)) {
			fprintf(stderr, "Error writing video frame\n");
			exit(1);
		}
	}

	rs->vid_valid = rs->vid_len = 0;
}

static void do_aud_out(struct rtp_session *rs)
{
	if (!rs->aud_valid)
		return;

	if (do_raw) {
		static int fd = -1;

		if (fd < 0)
			fd = open("test.aac", O_WRONLY | O_CREAT | O_TRUNC, 0644);

		if (fd < 0) {
			fprintf(stderr, "Could not open output file: %m\n");
			exit(EXIT_FAILURE);
		}

		if (write(fd, rs->aud_buf, rs->aud_len) != rs->aud_len) {
		        fprintf(stderr, "Could not write audio data: %m\n");
			exit(EXIT_FAILURE);
		}
	} else if (oc) {
		AVPacket pkt;

		av_init_packet(&pkt);
		pkt.size = rs->aud_len;
		pkt.data = rs->aud_buf;
		pkt.flags |= PKT_FLAG_KEY;
		pkt.stream_index = ast->index;

		if (av_write_frame(oc, &pkt)) {
			fprintf(stderr, "Could not write audio packet\n");
			exit(1);
		}
	}

        rs->aud_valid = rs->aud_len = 0;
}

int main(int argc, char* argv[])
{
	int opt;
	struct rtp_session rs;
	char *p, *t, c;
	char userinfo[128], server[128], port[8], path[128];
	int got_vop = 0;
	const char *err_msg;

	strcpy(port, DEFAULT_PORT);
	strcpy(userinfo, DEFAULT_USERINFO);
	avcodec_init();
        av_register_all();

	while ((opt = getopt(argc, argv, "hvr")) != -1) {
		switch (opt) {
		case 'v':
			rtp_verbose = 1;
			break;
		case 'r':
			do_raw = 1;
			break;
		case 'h':
		default:
			usage();
		}
	}

	if (argc <= optind)
		usage();

	p = argv[optind++];

	if (strncmp(p, "rtsp://", 7)) {
		fprintf(stderr, "Invalid scheme\n");
		exit(EXIT_FAILURE);
	}

	p += 7;

	t = strpbrk_and_copy(userinfo, p, "@", NULL);
	if (t)
		p = t;

	c = '\0';
	t = strpbrk_and_copy(server, p, ":/", &c);
	if (t && c == ':')
		t = strpbrk_and_copy(port, t, "/", NULL);
	if (!t)
		usage();

	strcpy(path, "/");
	strcat(path, t);

	rtp_session_init(&rs, userinfo, path, server, atoi(port));

	fprintf(stderr, "Conecting to rtsp://%s%s%s:%s%s\n",
		userinfo[0] ? userinfo : "", userinfo[0] ? "@" : "", server,
		port, path);

	if (rtp_session_start(&rs, &err_msg)) {
		fprintf(stderr, "Could not connect to server: %s\n", err_msg);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "Recording data...\n");

	while (1) {
		if (rtp_session_read(&rs)) {
			fprintf(stderr, "Error reading froms stream\n");
			exit(EXIT_FAILURE);
		}

		if (!got_vop) {
			if (!rs.vid_valid)
				continue;
			got_vop = 1;
		}
		do_vid_out(&rs);
		do_aud_out(&rs);
	}

	exit(0);
}
