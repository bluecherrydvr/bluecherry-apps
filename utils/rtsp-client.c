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

static void usage(void)
{
	fprintf(stderr, "Usage: rtsp_client [options] rtsp://[USERINFO@]"
		"SERVER[:PORT]/PATH_TO_SDP\n\n");
	fprintf(stderr, "Options: \n");
	fprintf(stderr, "  -a: Enable recording audio as well (default off)\n");
	fprintf(stderr, "  -o: Output path for MKV file (default temp.mkv)\n");
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

static void check_mpeg(struct rtp_session *rs)
{
	AVCodec *codec;
	AVCodecContext *c = NULL;
	int got_picture, len;
	AVFrame *picture;

	codec = avcodec_find_decoder(CODEC_ID_MPEG4);
	if (!codec) {
		fprintf(stderr, "codec not found\n");
		exit(1);
	}

	c = avcodec_alloc_context();
	picture = avcodec_alloc_frame();

	if (avcodec_open(c, codec) < 0) {
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}

	len = avcodec_decode_video(c, picture, &got_picture, rs->frame_buf,
				   rs->frame_len);

	if (len < 0) {
		fprintf(stderr, "Error while decoding frame\n");
		exit(1);
	}
	if (got_picture) {
		fprintf(stderr, "Got frame at %dx%d @ %d fps\n",
			c->width, c->height, rs->framerate);
	} else {
		fprintf(stderr, "Could not decode frame\n");
	}

	avcodec_close(c);
	av_freep(&c);
	av_freep(&picture);
}

static void do_out(struct rtp_session *rs, int fd)
{
	static int got_one;
	int ret;

	if (fd < 0)
		return;

	ret = rtp_session_read(rs);

	if (ret && ret != EAGAIN) {
		fprintf(stderr, "Error reading from RTP: %s\n", strerror(ret));
		exit(EXIT_FAILURE);
	}

	if (ret == EAGAIN || !rs->frame_valid)
		return;

	if (write(fd, rs->frame_buf, rs->frame_len) != rs->frame_len) {
		fprintf(stderr, "Could not write data: %m\n");
		exit(EXIT_FAILURE);
	}

	if (!got_one) {
		got_one = 1;
		check_mpeg(rs);
	}
}

int main(int argc, char* argv[])
{
	int opt;
	const char *outfile = "temp.m4v";
	const char *out_a = "temp.aac";
	int audio = 0;
	struct rtp_session rs_v, rs_a;
	char *p, *t, c;
	char userinfo[128], server[128], port[8], path[128];
	int fd_v = -1, fd_a = -1;

	strcpy(port, DEFAULT_PORT);
	strcpy(userinfo, DEFAULT_USERINFO);
	avcodec_init();
        av_register_all();

	while ((opt = getopt(argc, argv, "hao:")) != -1) {
		switch (opt) {
		case 'a':
			audio = 1;
			break;
		case 'o':
			outfile = optarg;
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

	fd_v = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_v < 0) {
		fprintf(stderr, "Could not open output file: %s: %m\n",
			outfile);
		exit(EXIT_FAILURE);
	}
	rtp_session_init(&rs_v, userinfo, path, server, atoi(port),
			 RTP_MEDIA_VIDEO);
	if (rtp_session_start(&rs_v)) {
		fprintf(stderr, "Could not connect to video server\n");
		exit(EXIT_FAILURE);
	}

	if (audio) {
		fd_a = open(out_a, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd_a < 0) {
			fprintf(stderr, "Could not open output file: %s: %m\n",
				out_a);
			exit(EXIT_FAILURE);
		}
		rtp_session_init(&rs_a, userinfo, path, server, atoi(port),
				 RTP_MEDIA_AUDIO);
		if (rtp_session_start(&rs_a)) {
			fprintf(stderr, "Could not connect to audio server\n");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stderr, "Conecting to rtsp://%s%s%s:%s%s\n",
		userinfo[0] ? userinfo : "", userinfo[0] ? "@" : "", server,
		port, path);

	fprintf(stderr, "Recording data...\n");

	while (1) {
		do_out(&rs_v, fd_v);
		do_out(&rs_a, fd_a);
	}

	exit(0);
}
