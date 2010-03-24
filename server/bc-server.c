/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "bc-server.h"

#define srv_err(fmt, args...) do {	\
	bc_log(fmt, ## args);		\
	exit(1);			\
} while(0)

static void do_decode(struct bc_rec *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;

	for (;;) {
		if (bc_buf_get(bc))
			srv_err("error getting buffer: %m");

		if (bc_mux_out(bc_rec))
			srv_err("error writing frame to outfile: %m");
	}
}

extern char *__progname;

static void usage(void)
{
	fprintf(stderr, "Usage: %s <args> [-o outfile]\n", __progname);
	fprintf(stderr, "  -d\tName of v4l2 device (/dev/video0 default)\n");
	fprintf(stderr, "  -o\tName of output file (required)\n");
	fprintf(stderr, "  -v\tVerbose output\n");
	fprintf(stderr, "  -D\tUse D1 mode (default CIF)\n");
	fprintf(stderr, "  -i\tFrame interval (default 1)\n");
	fprintf(stderr, "  -m\tEnable motion detection (default off)\n");
	exit(1);
}

int main(int argc, char **argv)
{
	struct bc_rec bc_rec = { 0 };
	struct bc_handle *bc;
	const char *dev = "/dev/video0";
	int d1_mode = 0;
	int interval = 1;
	int motion = 0;
	int opt;
	int ret;

	while ((opt = getopt(argc, argv, "d:o:i:vhDm")) != -1) {
		switch (opt) {
		case 'o': bc_rec.outfile	= optarg;	break;
		case 'd': dev			= optarg;	break;
		case 'D': d1_mode		= 1;		break;
		case 'i': interval		= atoi(optarg); break;
		case 'm': motion		= 1;		break;
		case 'h': default: usage();
		}
	}

	if (interval < 1)
		interval = 1;
	else if (interval > 15)
		interval = 15;

	if (!bc_rec.outfile)
		usage();

	/* Open the device */
	if ((bc = bc_handle_get(dev)) == NULL)
		srv_err("%s: error opening device: %m", dev);
	bc_rec.bc = bc;

	if (bc_set_interval(bc, interval))
		srv_err("%s: error setting interval: %m", dev);

	if (d1_mode)
		ret = bc_set_format(bc, V4L2_PIX_FMT_MPEG, 704, 480);
	else
		ret = bc_set_format(bc, V4L2_PIX_FMT_MPEG, 352, 240);

	if (ret)
		srv_err("%s: error setting format: %m", dev);

	if (bc_set_motion(bc, motion))
		srv_err("%s: error setting motion detection", dev);

	if (bc_handle_start(bc))
		srv_err("%s: error starting stream: %m", dev);

	if (bc_open_avcodec(&bc_rec))
		srv_err("%s: error opening avcodec: %m", dev);

	bc_log("%s: Starting record", dev);

	do_decode(&bc_rec);

	bc_close_avcodec(&bc_rec);
	bc_handle_free(bc);

	return 0;
}
