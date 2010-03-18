/*
 * Copyright (C) 2009-2010 Ben Collins <bcollins@bluecherry.net>
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <error.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

#include <libbluecherry.h>
#include <libavformat/avformat.h>

#define vprint(fmt, args...) \
    ({ if (verbose) fprintf(stderr, fmt , ## args); })

static const char *outfile;
static int verbose;

/* AVFormat stuff */
static AVOutputFormat *fmt_out;
static AVStream *video_st;
static AVFormatContext *oc;

static int is_key_frame(struct bc_handle *bc)
{
	unsigned char *p = bc_buf_data(bc);

	// XXX Should be checking vb->flags when driver sets it
	if (p[2] == 0x01 && p[3] == 0x00)
		return 1;

	return 0;
}

static int mux_out(struct bc_handle *bc)
{
	AVPacket pkt;

	av_init_packet(&pkt);

	if (is_key_frame(bc))
		pkt.flags |= PKT_FLAG_KEY;

	pkt.data = bc_buf_data(bc);
	pkt.size = bc_buf_size(bc);

	if (av_write_frame(oc, &pkt)) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

static void close_avcodec(int sig)
{
	int i;

	avcodec_close(video_st->codec);
	av_write_trailer(oc);

	for (i = 0; i < oc->nb_streams; i++) {
		av_freep(&oc->streams[i]->codec);
		av_freep(&oc->streams[i]);
	}

	if (!(fmt_out->flags & AVFMT_NOFILE))
		url_fclose(oc->pb);

	av_free(oc);

	if (sig)
		error(1, 0, "Terminated by signal");
}


static void open_avcodec(struct bc_handle *bc)
{
	AVCodec *codec;
	AVInputFormat *fmt_in;
	struct sigaction sa;

	/* This will make sure we close the stream */
	sa.sa_handler = close_avcodec;
	sa.sa_restorer = NULL;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);

	/* Initialize avcodec */
	avcodec_init();
	avcodec_register_all();
	av_register_all();

	/* Get the input and output formats */
	fmt_out = guess_format(NULL, outfile, NULL);
	if (!fmt_out)
		error(1, 0, "Could not detect output file format");
	fmt_in = av_find_input_format("mpeg");
	if (!fmt_in)
		error(1, 0, "Could not detect input file format");

	if ((oc = avformat_alloc_context()) == NULL)
		error(1, 0, "Could not allocate format context");

	oc->oformat = fmt_out;
	oc->iformat = fmt_in;
	snprintf(oc->filename, sizeof(oc->filename), "%s", outfile);
	snprintf(oc->title, sizeof(oc->title), "BC: %s", bc->dev_file);

	/* Setup new video stream */
	video_st = av_new_stream(oc, 0);
	if (!video_st)
		error(1, 0, "Failed to create new video stream");

	video_st->stream_copy = 1;
	video_st->time_base.den =
		bc->vparm.parm.capture.timeperframe.denominator;
	video_st->time_base.num =
		bc->vparm.parm.capture.timeperframe.numerator;
	snprintf(video_st->language, sizeof(video_st->language), "eng");

	video_st->codec->codec_id = CODEC_ID_MPEG4;
	video_st->codec->codec_type = CODEC_TYPE_VIDEO;
	video_st->codec->pix_fmt = PIX_FMT_YUV420P;
	video_st->codec->width = bc->vfmt.fmt.pix.width;
	video_st->codec->height = bc->vfmt.fmt.pix.height;
	video_st->codec->time_base.den =
		bc->vparm.parm.capture.timeperframe.denominator;
	video_st->codec->time_base.num =
		bc->vparm.parm.capture.timeperframe.numerator;

	if(oc->oformat->flags & AVFMT_GLOBALHEADER)
		video_st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	if (av_set_parameters(oc, NULL) < 0)
		error(1, 0, "Invalid output format parameters");

	/* Open Video output */
	if ((codec = avcodec_find_encoder(video_st->codec->codec_id)) == NULL)
		error(1, 0, "Cannot find output video codec");

	if (avcodec_open(video_st->codec, codec) < 0)
		error(1, 0, "Cannot open output video codec");

	/* Open output file, if needed */
	if (!(fmt_out->flags & AVFMT_NOFILE))
		if (url_fopen(&oc->pb, outfile, URL_WRONLY) < 0)
			error(1, 0, "Could not open output file");

	av_write_header(oc);

	return;
}

static void do_decode(struct bc_handle *bc)
{
	/* Skip until we get an I-Frame */
	for (;;) {
		if (bc_buf_get(bc))
			error(1, errno, "getting buffer");
		if (is_key_frame(bc))
			break;
	}

	/* Now loop endlessly */
	for (;;) {
		if (mux_out(bc))
			error(1, errno, "writing frame to outfile");

		if (bc_buf_get(bc))
			error(1, errno, "getting buffer");
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
	exit(1);
}

int main(int argc, char **argv)
{
	const char *dev = "/dev/video0";
	struct bc_handle *bc;
	int d1_mode = 0;
	int interval = 1;
	int opt;

	while ((opt = getopt(argc, argv, "d:o:i:vhD")) != -1) {
		switch (opt) {
		case 'o': outfile	= optarg;	break;
		case 'd': dev		= optarg;	break;
		case 'v': verbose	= 1;		break;
		case 'D': d1_mode	= 1;		break;
		case 'i': interval	= atoi(optarg); break;
		case 'h': default: usage();
		}
	}

	if (interval < 1)
		interval = 1;
	else if (interval > 15)
		interval = 15;

	if (!outfile)
		usage();

	/* Open the device */
	if ((bc = bc_handle_get(dev)) == NULL)
		error(1, errno, "%s: error opening device", dev);

	vprint("V4L2 device: %s\n", bc->vcap.card);
	vprint("  Driver: %s\n", bc->vcap.driver);
	vprint("  Version: %u.%u.%u\n", (bc->vcap.version >> 16) & 0xff,
	       (bc->vcap.version >> 8) & 0xff, bc->vcap.version & 0xff);
	vprint("  Bus info: %s\n", bc->vcap.bus_info);

	/* Set the interval/framerate */
	bc->vparm.parm.capture.timeperframe.numerator = interval;
	bc->vparm.parm.capture.timeperframe.denominator = 30;
	if (ioctl(bc->dev_fd, VIDIOC_S_PARM, &bc->vparm) < 0)
		error(1, errno, "%s: error setting capture parm", dev);

	vprint("  Frames/sec: %f\n",
		(float)bc->vparm.parm.capture.timeperframe.denominator /
		(float)bc->vparm.parm.capture.timeperframe.numerator);

	/* Set the format */
	bc->vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MPEG;
	if (d1_mode) {
		bc->vfmt.fmt.pix.width = 704;
		bc->vfmt.fmt.pix.height = 480;
	} else {
		bc->vfmt.fmt.pix.width = 352;
		bc->vfmt.fmt.pix.height = 240;
	}

	if (ioctl(bc->dev_fd, VIDIOC_S_FMT, &bc->vfmt) < 0)
		error(1, errno, "%s: error setting vid fmt cap", dev);

	vprint("  Format: %ux%d\n", bc->vfmt.fmt.pix.width,
	       bc->vfmt.fmt.pix.height);

	if (bc_handle_start(bc))
		error(1, errno, "%s: starting stream", dev);

	open_avcodec(bc);

	if (!verbose)
		fprintf(stderr, "%s: Starting record\n", dev);

	do_decode(bc);

	bc_handle_free(bc);

	return 0;
}
