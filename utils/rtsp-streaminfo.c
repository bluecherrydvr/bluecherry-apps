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

static void usage(void)
{
	fprintf(stderr, "Usage: rtsp-streaminfo rtsp://[USERINFO@]"
		"SERVER[:PORT]/PATH_TO_SDP\n\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	AVFormatContext *fmt_ctx;
	int opt;
	char *url;

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
		case 'h':
		default:
			usage();
		}
	}

	if (argc <= optind)
		usage();

	url = argv[optind++];

	av_register_all();
	if (av_open_input_file(&fmt_ctx, url, NULL, 0, NULL) != 0) {
		fprintf(stderr, "Could not open URI\n");
		exit(1);
	}

	if (av_find_stream_info(fmt_ctx) < 0) {
		fprintf(stderr, "Could not find stream info\n");
		exit(1);
	}

	dump_format(fmt_ctx, 0, url, 0);

	exit(0);
}
