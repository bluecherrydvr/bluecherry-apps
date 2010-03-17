/*
 * Copyright (C) 2009-2010 Ben Collins <bcollins@bluecherry.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

#include <linux/videodev2.h>
#include <libavformat/avformat.h>

#define MP4_DATA_SIZE		(256 * 1024)

#define vprint(fmt, args...) \
    ({ if (verbose) fprintf(stderr, fmt , ## args); })

static int buf_req = 8;
static const char *outfile;
static int verbose;
static int out_fd;
static const char *dev = "/dev/video0";

/* v4l globals */
static struct v4l2_format vid;
static struct v4l2_streamparm sparm;

/* AVFormat stuff */
static AVOutputFormat *fmt_out;
static AVStream *video_st;
static AVFormatContext *oc;

#define reset_vbuf(__vb) do {				\
	memset((__vb), 0, sizeof(*(__vb)));		\
	(__vb)->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	\
	(__vb)->memory = V4L2_MEMORY_MMAP;		\
} while(0)

struct solo_buffer {
	void *data;
	size_t size;
};

static struct solo_buffer *p_bufs;

static int is_key_frame(struct v4l2_buffer *vb)
{
	unsigned char *p = p_bufs[vb->index].data;

	// XXX Should be checking vb->flags when driver sets it
	if (p[2] == 0x01 && p[3] == 0x00)
		return 1;

	return 0;
}

static int mux_out(struct v4l2_buffer *vb)
{
	AVPacket pkt;

	av_init_packet(&pkt);

	if (is_key_frame(vb))
		pkt.flags |= PKT_FLAG_KEY;

	pkt.data = p_bufs[vb->index].data;
	pkt.size = vb->bytesused;

	if (av_write_frame(oc, &pkt))
		return EINVAL;

	return 0;
}

static void prepare_buffers(int fd)
{
	struct v4l2_requestbuffers req;
	struct v4l2_buffer vbuf;
	int i;

	reset_vbuf(&req);
	req.count = buf_req;

	if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0)
		error(1, errno, "Device does not support mmap?");

	if (req.count < 2)
		error(1, 0, "VIDIOC_REQBUFS reports too few buffers");

	if (req.count != buf_req)
		error(0, 0, "Requested %d buffers, but returned %d",
		      buf_req, req.count);

	buf_req = req.count;

	if ((p_bufs = calloc(buf_req, sizeof(*p_bufs))) == NULL)
		error(1, errno, "Failed to allocate mmap buffers");

	for (i = 0; i < buf_req; i++) {
		reset_vbuf(&vbuf);
		vbuf.index = i;

		if (ioctl(fd, VIDIOC_QUERYBUF, &vbuf) < 0)
			error(1, errno, "VIDIOC_QUERYBUF failed");

		p_bufs[i].size = vbuf.length;
		p_bufs[i].data = mmap(NULL, vbuf.length,
				   PROT_WRITE | PROT_READ, MAP_SHARED,
				   fd, vbuf.m.offset);
		if (p_bufs[i].data == MAP_FAILED)
			error(1, errno, "mmap failed");
	}
}

static void start_streaming(int fd)
{
	enum v4l2_buf_type buf_type;
	struct v4l2_buffer vbuf;
	int i;

	for (i = 0; i < buf_req; i++) {
		reset_vbuf(&vbuf);
		vbuf.index = i;
		if (ioctl(fd, VIDIOC_QBUF, &vbuf) < 0)
			error(1, errno, "qbuf failed");
	}

	buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &buf_type) < 0)
		error(1, errno, "streamon failed");
}

/* return_buf and get_next_frame are NOT thread safe in that you cannot
 * call one while another thread is calling the other, or call them from
 * each other, nor can you return buffers out-of-order */
static unsigned char read_buf[MP4_DATA_SIZE];
static struct v4l2_buffer *q_vbuf = (struct v4l2_buffer *)read_buf;
static int q_cnt;

static void return_buf(int fd)
{
	int i;

	/* Don't return bufs until half are used */
	if (q_cnt < (buf_req >> 1))
		return;

	for (i = 0; i < q_cnt; i++) {
		if (ioctl(fd, VIDIOC_QBUF, &q_vbuf[i]) < 0)
			error(0, errno, "Error requeuing buffer");
	}

	q_cnt = 0;
}

static struct v4l2_buffer *get_next_frame(int fd)
{
	struct v4l2_buffer *vb = &q_vbuf[q_cnt];

	reset_vbuf(vb);

	if (ioctl(fd, VIDIOC_DQBUF, vb) < 0) {
		static unsigned int count = 0;
		error(0, errno, "error in dqbuf: %u", ++count);
		if (errno == EIO) {
			return_buf(fd);
			return get_next_frame(fd);
		}
		exit(1);
	}

	if (vb->index >= buf_req)
		error(1, 0, "Got invalid vbuf index");

	q_cnt++;

	return vb;
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


static void open_avcodec(void)
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
	snprintf(oc->title, sizeof(oc->title), "BC: %s", dev);

	/* Setup new video stream */
	video_st = av_new_stream(oc, 0);
	if (!video_st)
		error(1, 0, "Failed to create new video stream");

	video_st->stream_copy = 1;
	video_st->time_base.den =
		sparm.parm.capture.timeperframe.denominator;
	video_st->time_base.num =
		sparm.parm.capture.timeperframe.numerator;
	snprintf(video_st->language, sizeof(video_st->language), "eng");

	video_st->codec->codec_id = CODEC_ID_MPEG4;
	video_st->codec->codec_type = CODEC_TYPE_VIDEO;
	video_st->codec->pix_fmt = PIX_FMT_YUV420P;
	video_st->codec->width = vid.fmt.pix.width;
	video_st->codec->height = vid.fmt.pix.height;
	video_st->codec->time_base.den =
		sparm.parm.capture.timeperframe.denominator;
	video_st->codec->time_base.num =
		sparm.parm.capture.timeperframe.numerator;

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

static void do_decode(int fd)
{
	struct v4l2_buffer *vb;

	prepare_buffers(fd);
	start_streaming(fd);
	open_avcodec();

	/* Skip until we get an I-Frame */
	for (;;) {
		vb = get_next_frame(fd);
		if (is_key_frame(vb))
			break;
		return_buf(fd);
	}

	/* Now loop endlessly */
	for (;;) {
		int ret;

		if ((ret = mux_out(vb)))
			error(1, ret, "writing frame to outfile");

		return_buf(fd);

		vb = get_next_frame(fd);
	}
}

extern char *__progname;

static void usage(void)
{
	fprintf(stderr, "Usage: %s <args> [-o outfile]\n", __progname);
	fprintf(stderr, "  -d\tName of v4l2 device (/dev/video0 default)\n");
	fprintf(stderr, "  -o\tName of output file (required)\n");
	fprintf(stderr, "  -b\tNumber of buffers to use for mmap\n");
	fprintf(stderr, "  -v\tVerbose output\n");
	fprintf(stderr, "  -D\tUse D1 mode (default CIF)\n");
	fprintf(stderr, "  -i\tFrame interval (default 1)\n");
	exit(1);
}

int main(int argc, char **argv)
{
	struct v4l2_capability dev_cap;
	int fd;
	int d1_mode = 0;
	int interval = 1;
	int opt;

	while ((opt = getopt(argc, argv, "d:o:b:i:vhD")) != -1) {
		switch (opt) {
		case 'o': outfile	= optarg;	break;
		case 'd': dev		= optarg;	break;
		case 'b': buf_req	= atoi(optarg); break;
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

	/* Open the output file */
	out_fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (out_fd < 0)
		error(1, errno, "%s: error opening outfile", outfile);

	/* Open the device */
	fd = open(dev, O_RDWR);
	if (fd < 0)
		error(1, errno, "%s: error opening device", dev);

	memset(&dev_cap, 0, sizeof(dev_cap));
	/* Query the capbilites and verify it is a solo encoder */
	if (ioctl(fd, VIDIOC_QUERYCAP, &dev_cap) < 0)
		error(1, errno, "%s: error getting capabilities", dev);

	if (!(dev_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
		error(1, 0, "%s: not a capture device", dev);

	if (!(dev_cap.capabilities & V4L2_CAP_STREAMING))
		error(1, 0, "%s: does not support streaming I/O", dev);

	if (strcmp((char *)dev_cap.driver, "solo6010"))
		error(1, 0, "%s: not a solo6010 device", dev);
	if (strncmp((char *)dev_cap.card, "Softlogic 6010 Enc ", 19))
		error(1, 0, "%s: not a solo6010 encoder", dev);

	vprint("V4L2 device: %s\n", dev_cap.card);
	vprint("  Driver: %s\n", dev_cap.driver);
	vprint("  Version: %u.%u.%u\n", (dev_cap.version >> 16) & 0xff,
	       (dev_cap.version >> 8) & 0xff, dev_cap.version & 0xff);
	vprint("  Bus info: %s\n", dev_cap.bus_info);

	/* Set the interval/framerate */
	sparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	sparm.parm.capture.timeperframe.numerator = interval;
	sparm.parm.capture.timeperframe.denominator = 30;
	if (ioctl(fd, VIDIOC_S_PARM, &sparm) < 0)
		error(1, errno, "%s: error setting capture parm", dev);

	vprint("  Frames/sec: %f\n",
		(float)sparm.parm.capture.timeperframe.denominator /
		(float)sparm.parm.capture.timeperframe.numerator);

	/* Set the format */
	vid.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vid.fmt.pix.pixelformat = V4L2_PIX_FMT_MPEG;
	vid.fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
	if (d1_mode) {
		vid.fmt.pix.width = 704;
		vid.fmt.pix.height = 480;
	} else {
		vid.fmt.pix.width = 352;
		vid.fmt.pix.height = 240;
	}
	vid.fmt.pix.field = V4L2_FIELD_ANY;

	if (ioctl(fd, VIDIOC_S_FMT, &vid) < 0)
		error(1, errno, "%s: error setting vid fmt cap", dev);

	vprint("  Format: %ux%d\n", vid.fmt.pix.width, vid.fmt.pix.height);

	if (!verbose)
		fprintf(stderr, "%s: Starting record\n", dev);

	do_decode(fd);

	close(fd);

	return 0;
}
