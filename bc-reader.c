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

#define MP4_DATA_SIZE		(256 * 1024)

#define vprint(fmt, args...) \
    ({ if (verbose) fprintf(stderr, fmt , ## args); })

static int userptr_io;
static int read_io;
static int buf_req = 8;
static const char *outfile;
static int verbose;
static int out_fd;

/* Simple profile level 3, object type video, plus fancy header */
static unsigned char vid_file_header[] = {
	0x00, 0x00, 0x01, 0xB0, 0x03, 0x00, 0x00, 0x01,
	0xB5, 0x89, 0x13, 0x00, 0x00, 0x01, 0xb2,
	'B', 'l', 'u', 'e', 'c', 'h', 'e', 'r', 'r', 'y',
};

#define reset_vbuf(__vb) do {				\
	memset((__vb), 0, sizeof(*(__vb)));		\
	(__vb)->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	\
	(__vb)->memory = (userptr_io ? V4L2_MEMORY_USERPTR : \
			V4L2_MEMORY_MMAP);		\
} while(0)

struct solo_buffer {
	void *data;
	size_t size;
};

static struct solo_buffer *p_bufs;
static int n_bufs;

static void prepare_buffers(int fd)
{
	int page_size = sysconf(_SC_PAGESIZE);
	struct v4l2_requestbuffers req;
	struct v4l2_buffer vbuf;
	int i;

	if (read_io)
		return;

	memset(&req, 0, sizeof(req));
	req.count = buf_req;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = (userptr_io ? V4L2_MEMORY_USERPTR : V4L2_MEMORY_MMAP);

	if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0)
		error(1, errno, "Device does not support mmap?");

	if (req.count < 2)
		error(1, 0, "VIDIOC_REQBUFS reports too few buffers");

	if (req.count != buf_req)
		error(0, 0, "Requested %d buffers, but returned %d",
		      buf_req, req.count);

	n_bufs = req.count;

	if ((p_bufs = calloc(n_bufs, sizeof(*p_bufs))) == NULL)
		error(1, errno, "Failed to allocate mmap buffers");

	for (i = 0; i < n_bufs; i++) {
		reset_vbuf(&vbuf);
		vbuf.index = i;

		if (ioctl(fd, VIDIOC_QUERYBUF, &vbuf) < 0)
			error(1, errno, "VIDIOC_QUERYBUF failed");

		if (userptr_io) {
			int buf_size = (vbuf.length + page_size - 1) &
					~(page_size - 1);
			p_bufs[i].size = buf_size;
			if (posix_memalign(&p_bufs[i].data, page_size,
					   buf_size))
				error(1, errno, "posix_memalign failed");
		} else {
			p_bufs[i].size = vbuf.length;
			p_bufs[i].data = mmap(NULL, vbuf.length,
					   PROT_WRITE | PROT_READ, MAP_SHARED,
					   fd, vbuf.m.offset);
			if (p_bufs[i].data == MAP_FAILED)
				error(1, errno, "mmap failed");
		}
	}
}

static void start_streaming(int fd)
{
	enum v4l2_buf_type buf_type;
	struct v4l2_buffer vbuf;
	int i;

	if (read_io)
		return;

	for (i = 0; i < n_bufs; i++) {
		reset_vbuf(&vbuf);
		vbuf.index = i;
		if (userptr_io) {
			vbuf.m.userptr = (unsigned long)p_bufs[i].data;
			vbuf.length = p_bufs[i].size;
		}
		if (ioctl(fd, VIDIOC_QBUF, &vbuf) < 0)
			error(1, errno, "qbuf failed");
	}

	buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &buf_type) < 0)
		error(1, errno, "streamon failed");
}

static unsigned char read_buf[MP4_DATA_SIZE];
struct v4l2_buffer *q_vbuf = (struct v4l2_buffer *)read_buf;

static void return_buf(int fd)
{
	if (read_io)
		return;

	if (ioctl(fd, VIDIOC_QBUF, q_vbuf) < 0)
		error(0, errno, "Error requeuing buffer");
}

static void *get_next_frame(int fd, size_t *size)
{
	if (read_io) {
		int ret = read(fd, read_buf, sizeof(read_buf));
		if (ret <= 0)
			error(1, errno, "error in read");
		*size = ret;
		return read_buf;
	}

	reset_vbuf(q_vbuf);

	if (ioctl(fd, VIDIOC_DQBUF, q_vbuf) < 0) {
		static unsigned int count = 0;
		error(0, errno, "error in dqbuf: %u", ++count);
		if (errno == EIO) {
			return_buf(fd);
			return get_next_frame(fd, size);
		}
		exit(1);
	}

	if (q_vbuf->index >= n_bufs)
		error(1, 0, "Got invalid vbuf index");

	*size = q_vbuf->bytesused;
	return p_bufs[q_vbuf->index].data;
}

static void do_decode(int fd)
{
	prepare_buffers(fd);
	start_streaming(fd);

	if (write(out_fd, vid_file_header, sizeof(vid_file_header)) !=
	    sizeof(vid_file_header))
		error(1, errno, "writing header to outfile");

	/* One loop per frame */
	for (;;) {
		static int got_vop;
		size_t size;
		unsigned char *mp4 = get_next_frame(fd, &size);

		/* Wait till I_VOP */
		if (!got_vop && !(mp4[2] == 0x01 && mp4[3] == 0x00)) {
			return_buf(fd);
			continue;
		} else
			got_vop = 1;

		if (write(out_fd, mp4, size) != size)
			error(1, errno, "writing frame to outfile");

		return_buf(fd);
	}
}

extern char *__progname;

static void usage(void)
{
	fprintf(stderr, "Usage: %s <args> [-o outfile]\n", __progname);
	fprintf(stderr, "  -d\tName of v4l2 device (/dev/video0 default)\n");
	fprintf(stderr, "  -o\tName of output file (required)\n");
	fprintf(stderr, "  -b\tNumber of buffers to use for mmap/userptr\n");
	fprintf(stderr, "  -v\tVerbose output\n");
	fprintf(stderr, "  -D\tUse D1 mode (default CIF)\n");
	fprintf(stderr, "  -i\tFrame interval (default 1)\n");
	fprintf(stderr, "By default MMAP I/O is used. Alternatives:\n");
	fprintf(stderr, "  -u\tuserptr I/O support\n");
	fprintf(stderr, "  -r\tread() I/O support\n");
	exit(1);
}

int main(int argc, char **argv)
{
	const char *dev = "/dev/video0";
	struct v4l2_capability dev_cap;
	struct v4l2_format vid;
	struct v4l2_streamparm sparm;
	int fd;
	int d1_mode = 0;
	int interval = 1;
	int opt;

	while ((opt = getopt(argc, argv, "d:o:b:i:urvhD")) != -1) {
		switch (opt) {
		case 'o': outfile	= optarg;	break;
		case 'd': dev		= optarg;	break;
		case 'u': userptr_io	= 1;		break;
		case 'r': read_io	= 1;		break;
		case 'b': buf_req	= atoi(optarg); break;
		case 'v': verbose	= 1;		break;
		case 'D': d1_mode	= 1;		break;
		case 'i': interval	= atoi(optarg); break;
		case 'h':
		default:
			usage();
		}
	}

	if (interval < 1)
		interval = 1;
	else if (interval > 15)
		interval = 15;

	if (!outfile)
		usage();

	if (read_io && userptr_io)
		usage();

	if (read_io)
		vprint("Using read I/O\n");
	else if (userptr_io)
		vprint("Using user pointer I/O\n");
	else
		vprint("Using mmap I/O\n");

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

	if (!read_io) {
		if (!(dev_cap.capabilities & V4L2_CAP_STREAMING))
			error(1, 0, "%s: does not support streaming I/O", dev);
	} else {
		if (!(dev_cap.capabilities & V4L2_CAP_READWRITE))
			error(1, 0, "%s: does not support rw I/O", dev);
	}

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
	memset(&sparm, 0, sizeof(sparm));
	sparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	sparm.parm.capture.timeperframe.numerator = interval;
	sparm.parm.capture.timeperframe.denominator = 30;
	if (ioctl(fd, VIDIOC_S_PARM, &sparm) < 0)
		error(1, errno, "%s: error setting capture parm", dev);

	vprint("  Frames/sec: %f\n",
		(float)sparm.parm.capture.timeperframe.denominator /
		(float)sparm.parm.capture.timeperframe.numerator);

	/* Set the format */
	memset(&vid, 0, sizeof(vid));
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
