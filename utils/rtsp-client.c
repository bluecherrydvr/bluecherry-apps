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
	fprintf(stderr, "Usage: rtsp-client rtsp://[USERINFO@]"
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

static void do_vid_out(struct rtp_session *rs, int fd)
{
	if (write(fd, rs->pkt_vid.data, rs->pkt_vid.size) != rs->pkt_vid.size) {
		fprintf(stderr, "Could not write video data: %m\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char* argv[])
{
	int opt;
	const char *out_v = "temp.m4v";
	struct rtp_session rs;
	char *p, *t, c;
	char userinfo[128], server[128], port[8], path[128];
	int fd_v = -1;
	const char *err_msg;

	strcpy(port, DEFAULT_PORT);
	strcpy(userinfo, DEFAULT_USERINFO);
	avcodec_init();
        av_register_all();

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
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

	fd_v = open(out_v, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_v < 0) {
		fprintf(stderr, "Could not open output file: %s: %m\n",
			out_v);
		exit(EXIT_FAILURE);
	}

	rtp_session_init(&rs, userinfo, path, server, atoi(port));

	fprintf(stderr, "Conecting to rtsp://%s%s%s:%s%s\n",
		userinfo[0] ? userinfo : "", userinfo[0] ? "@" : "", server,
		port, path);

	if (rtp_session_start(&rs, &err_msg)) {
		fprintf(stderr, "Could not connect to server: %s\n", err_msg);
		exit(EXIT_FAILURE);
	}

	dump_format(rs.fmt_ctx, 0, rs.url, 0);

	fprintf(stderr, "Recording data...\n");

	while (1) {
		if (rtp_session_read(&rs)) {
			fprintf(stderr, "Error reading from stream\n");
			exit(EXIT_FAILURE);
		}

		do_vid_out(&rs, fd_v);
	}

	exit(0);
}
