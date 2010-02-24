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

#include <linux/videodev2.h>
#include <libv4l2.h>

#define MP4_DATA_SIZE		(1024 * 1024)
#define XVID_HEADER_SIZE	30

#define vprint(fmt, args...) \
    ({ if (verbose) fprintf(stderr, fmt , ## args); })

static int userptr_io;
static int read_io;
static int libv4l2;
static int buf_req = 8;
static const char *outfile;
static int mp4_size;
static int verbose;
static int out_fd;

static int (*my_open)(const char *, int, ...) = open;
static int (*my_close)(int) = close;
static int (*my_ioctl)(int, unsigned long int, ...) = ioctl;
static ssize_t (*my_read)(int, void *, size_t) = read;
static void *(*my_mmap)(void *, size_t, int, int, int, off_t) = mmap;
static int (*my_munmap)(void *, size_t) = munmap;

struct vop_header {
	/* VD_IDX0 */
	uint32_t size:20, sync_start:1, page_stop:1, vop_type:2, channel:4,
		nop0:1, source_fl:1, interlace:1, progressive:1;

	/* VD_IDX1 */
	uint32_t vsize:8, hsize:8, frame_interop:1, nop1:7, win_id:4, scale:4;

	/* VD_IDX2 */
	uint32_t base_addr:16, nop2:15, hoff:1;

	/* VD_IDX3 - User set macros */
	uint32_t sy:12, sx:12, nop3:1, hzoom:1, read_interop:1,
		write_interlace:1, scale_mode:4;

	/* VD_IDX4 - User set macros continued */
	uint32_t write_page:8, nop4:24;

	/* VD_IDX5 */
	uint32_t next_code_addr;

	uint32_t end_nops[10];
} __attribute__((packed));

static unsigned char xvid_header_stub[XVID_HEADER_SIZE] = {
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x20,
	0x02, 0x48, 0x0d, 0xc0, 0x00, 0x40, 0x00, 0x40,
	0x00, 0x40, 0x00, 0x80, 0x00, 0x97, 0x53, 0x0a,
	0x2c, 0x08, 0x3c, 0x28, 0x8c, 0x1f
};

static int transcode_to_output(void *mp4)
{
	unsigned char *p;
	struct vop_header *vh = mp4;
	size_t size;

	/* Skip the solo vop header */
	mp4 += sizeof(*vh);

	/* This only works because the parts of vop_header we care about
	 * are in the 34 bytes at the start of the buffer, and we only
	 * overwrite the last 30 bytes for xvid_header_stub in i-frame case */
	if (vh->vop_type == 0) {
		/* Type 0 == I-frame */
		mp4 -= sizeof(xvid_header_stub);
		memcpy(mp4, xvid_header_stub, sizeof(xvid_header_stub));

		/* Munge the header */
		p = mp4;
		p[24] = vh->hsize;
		p[25] = (vh->hsize << 8) + 0x8 +
		    ((vh->vsize >> 6) & 0x7);
		p[26] = (vh->vsize << 2) & 0xff;

		if (vh->scale > 8)
			p[27] = (((vh->vsize << 4) & 0x3) << 6) + 0x38;
		else
			p[27] = (((vh->vsize << 4) & 0x3) << 6) + 0x28;

		size = vh->size + sizeof(xvid_header_stub);
	} else {
		/* Anything else is a P frame */
		size = vh->size;
	}

	/* Sanity checks */
	if (size > mp4_size)
		return -1;

	if (write(out_fd, mp4, size) != size)
		return -1;

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

	if (my_ioctl(fd, VIDIOC_REQBUFS, &req) < 0)
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

		if (my_ioctl(fd, VIDIOC_QUERYBUF, &vbuf) < 0)
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
			p_bufs[i].data = my_mmap(NULL, vbuf.length,
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
		if (my_ioctl(fd, VIDIOC_QBUF, &vbuf) < 0)
			error(1, errno, "qbuf failed");
	}

	buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (my_ioctl(fd, VIDIOC_STREAMON, &buf_type) < 0)
		error(1, errno, "streamon failed");
}

static unsigned char read_buf[MP4_DATA_SIZE];
struct v4l2_buffer *q_vbuf = (struct v4l2_buffer *)read_buf;

static void *get_next_frame(int fd, unsigned long long *size)
{
	if (read_io) {
		int ret = my_read(fd, read_buf, sizeof(read_buf));
		if (ret <= 0) {
			error(0, errno, "error in read");
			return NULL;
		}
		*size += ret;
		return read_buf;
	}

	reset_vbuf(q_vbuf);

	if (my_ioctl(fd, VIDIOC_DQBUF, q_vbuf) < 0) {
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

	if (my_ioctl(fd, VIDIOC_QBUF, q_vbuf) < 0)
		error(0, errno, "Error requeuing buffer");
}

static void do_decode(int fd)
{
	void *mp4;
	unsigned long long size = 0;

	prepare_buffers(fd);
	start_streaming(fd);

	/* One loop per frame */
	for (;;) {
		mp4 = get_next_frame(fd, &size);
		if (!mp4)
			break;

		if (transcode_to_output(mp4)) {
			error(0, 0, "Error during transcode");
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
	fprintf(stderr, "  -V\tUse libv4l2 wrapper (default no)\n");
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

	while ((opt = getopt(argc, argv, "d:o:b:urVvh")) != -1) {
		switch (opt) {
		case 'o': outfile	= optarg;	break;
		case 'd': dev		= optarg;	break;
		case 'u': userptr_io	= 1;		break;
		case 'r': read_io	= 1;		break;
		case 'V': libv4l2	= 1;		break;
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

	if (libv4l2) {
		my_open = v4l2_open;
		my_close = v4l2_close;
		my_ioctl = v4l2_ioctl;
		my_read = v4l2_read;
		my_mmap = v4l2_mmap;
		my_munmap = v4l2_munmap;
		vprint("Using libv4l2 with ");
	} else
		vprint("Using direct calls with ");

	if (read_io)
		vprint("read I/O\n");
	else if (userptr_io)
		vprint("user pointer I/O\n");
	else
		vprint("mmap I/O\n");

	/* Open the output file */
	out_fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (out_fd < 0)
		error(1, errno, "%s: error opening outfile", outfile);

	/* Open the device */
	fd = my_open(dev, O_RDWR);
	if (fd < 0)
		error(1, errno, "%s: error opening device", dev);

	/* Query the capbilites and verify it is a solo encoder */
	if (my_ioctl(fd, VIDIOC_QUERYCAP, &dev_cap) < 0)
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

	my_close(fd);

	return 0;
}
