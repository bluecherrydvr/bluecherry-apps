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

#define MP4_DATA_SIZE		(1024 * 1024)

#define vprint(fmt, args...) \
    ({ if (verbose) fprintf(stderr, fmt , ## args); })

static int userptr_io;
static int read_io;
static int buf_req = 8;
static const char *outfile;
static int mp4_size;
static int verbose;
static int out_fd;

struct vop_header {
	/* VD_IDX0 */
	u_int32_t size:20, sync_start:1, page_stop:1, vop_type:2, channel:4,
		nop0:1, source_fl:1, interlace:1, progressive:1;

	/* VD_IDX1 */
	u_int32_t vsize:8, hsize:8, frame_interop:1, nop1:7, win_id:4, scale:4;

	/* VD_IDX2 */
	u_int32_t base_addr:16, nop2:15, hoff:1;

	/* VD_IDX3 - User set macros */
	u_int32_t sy:12, sx:12, nop3:1, hzoom:1, read_interop:1,
		write_interlace:1, scale_mode:4;

	/* VD_IDX4 - User set macros continued */
	u_int32_t write_page:8, nop4:24;

	/* VD_IDX5 */
	u_int32_t next_code_addr;

	u_int32_t end_nops[10];
} __attribute__((packed));

#define PAR_11_VGA	1 // 1:1 Square
#define PAR_43_PAL	2 // 4:3 pal (12:11 625-line)
#define PAR_43_NTSC	3 // 4:3 ntsc (10:11 525-line)

/* Simple profile level 3, object type video, plus fancy header */
static unsigned char vid_file_header[] = {
	0x00, 0x00, 0x01, 0xB0, 0x03, 0x00, 0x00, 0x01,
	0xB5, 0x09, 0x00, 0x00, 0x01, 0xb2,
	'B', 'l', 'u', 'e', 'c', 'h', 'e', 'r', 'r', 'y',
};

static unsigned char vid_vop_header[] = {
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x20,
	0x02, 0x48, 0x0d, 0xc0, 0x00, 0x40, 0x00, 0x40,
	0x00, 0x40, 0x00, 0x80, 0x00, 0x97, 0x53, 0x04,
	0x1f, 0x4c, 0x58, 0x10, 0x78, 0x51, 0x18, 0x3e,
};

/*
 * Things we can change around:
 *
 * byte  10,        4-bits 01111000                   aspect
 * bytes 21,22,23, 16-bits 000x1111 11111111 1111x000 fps/res
 * bytes 23,24,25  15-bits 00000n11 11111111 11111x00 interval
 * bytes 25,26,27  13-bits 00000x11 11111111 111x0000 width
 * bytes 27,28,29  13-bits 000x1111 11111111 1x000000 height
 * byte  29         1-bit  0x100000                   interlace
 */

static void write_header(void)
{
	if (write(out_fd, vid_file_header, sizeof(vid_file_header)) !=
	    sizeof(vid_file_header))
		error(1, errno, "writing header to outfile");
}

static int transcode_to_output(void *mp4)
{
	struct vop_header *vh = mp4;
	static int vop_once;

	/* Skip the solo vop header */
	mp4 += sizeof(*vh);

	/* Sanity checks */
	if (vh->size > mp4_size)
		return 0;
	if ((vh->vsize << 4) > 704 || (vh->hsize << 4) > 576)
		return 0;

	if (vh->vop_type == 0) {
		unsigned char *p = vid_vop_header;

		/* Should not change mid-stream */
		if (!vop_once) {
			unsigned short fps = 30000;
			unsigned short interval = 1000;
			unsigned short w = vh->hsize << 4;
			unsigned short h = vh->vsize << 4;

			/* Aspect ration */
			p[10] = (p[10] & 0x87) | ((PAR_43_NTSC << 3) & 0x78);

			/* Frame rate and interval */
			p[22] = fps >> 4;
			p[23] = ((fps << 4) & 0xf0) | 0x04 | (interval >> 13);
			p[24] = (interval >> 5) & 0xff;
			p[25] = ((interval << 3) & 0xf8) | 0x04;

			/* Width and height */
			p[26] = (w >> 3) & 0xff;
			p[27] = ((h >> 9) & 0x0f) | 0x10;
			p[28] = (h >> 1) & 0xff;

			/* Interlace */
			if (vh->scale > 8)
				p[29] |= 0x20;

			vop_once = 1;
		}

		if (write(out_fd, p, sizeof(vid_vop_header)) !=
		    sizeof(vid_vop_header))
			return errno;
	}

	if (write(out_fd, mp4, vh->size) != vh->size)
		return errno;

	return 0;
}

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
		if (!mp4_size)
			mp4_size = p_bufs[i].size;
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

static void *get_next_frame(int fd, unsigned long long *size)
{
	if (read_io) {
		int ret = read(fd, read_buf, sizeof(read_buf));
		if (ret <= 0) {
			error(0, errno, "error in read");
			return NULL;
		}
		*size += ret;
		return read_buf;
	}

	reset_vbuf(q_vbuf);

	if (ioctl(fd, VIDIOC_DQBUF, q_vbuf) < 0) {
		error(0, errno, "error in dqbuf");
		return NULL;
	}

	if (q_vbuf->index >= n_bufs) {
		error(0, 0, "Got invalid vbuf index");
		return NULL;
	}

	*size += q_vbuf->bytesused;
	return p_bufs[q_vbuf->index].data;
}

static void return_buf(int fd)
{
	if (read_io)
		return;

	if (ioctl(fd, VIDIOC_QBUF, q_vbuf) < 0)
		error(0, errno, "Error requeuing buffer");
}

static void do_decode(int fd)
{
	void *mp4;
	unsigned long long size = 0;
	int ret;

	prepare_buffers(fd);
	start_streaming(fd);
	write_header();

	/* One loop per frame */
	for (;;) {
		mp4 = get_next_frame(fd, &size);
		if (!mp4)
			break;

		if ((ret = transcode_to_output(mp4))) {
			error(0, ret, "Error during transcode");
			break;
		}

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
	fprintf(stderr, "By default MMAP I/O is used. Alternatives:\n");
	fprintf(stderr, "  -u\tuserptr I/O support\n");
	fprintf(stderr, "  -r\tread() I/O support\n");
	exit(1);
}

int main(int argc, char **argv)
{
	const char *dev = "/dev/video0";
	struct v4l2_capability dev_cap;
	int fd;
	int opt;

	while ((opt = getopt(argc, argv, "d:o:b:urvh")) != -1) {
		switch (opt) {
		case 'o': outfile	= optarg;	break;
		case 'd': dev		= optarg;	break;
		case 'u': userptr_io	= 1;		break;
		case 'r': read_io	= 1;		break;
		case 'b': buf_req	= atoi(optarg); break;
		case 'v': verbose	= 1;		break;
		case 'h':
		default:
			usage();
		}
	}

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

	/* Query the capbilites and verify it is a solo encoder */
	if (ioctl(fd, VIDIOC_QUERYCAP, &dev_cap) < 0)
		error(1, 0, "%s: error getting capabilities", dev);

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

	if (!verbose)
		fprintf(stderr, "%s: Starting record\n", dev);

	do_decode(fd);

	close(fd);

	return 0;
}
