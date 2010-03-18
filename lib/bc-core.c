/*
 * Copyright (C) 2009-2010 Ben Collins <bcollins@bluecherry.net>
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <libbluecherry.h>

#define reset_vbuf(__vb) do {				\
	memset((__vb), 0, sizeof(*(__vb)));		\
	(__vb)->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	\
	(__vb)->memory = V4L2_MEMORY_MMAP;		\
} while(0)

static int bc_bufs_prepare(struct bc_handle *bc)
{
	struct v4l2_requestbuffers req;
	int i;

	reset_vbuf(&req);
	req.count = bc->p_cnt;

	if (ioctl(bc->dev_fd, VIDIOC_REQBUFS, &req) < 0)
		return -1;

	if (req.count < 2 || req.count > BC_MAX_BUFFERS) {
		errno = EINVAL;
		return -1;
	}

	bc->p_cnt = bc->q_cnt = req.count;

	for (i = 0; i < bc->p_cnt; i++) {
		struct v4l2_buffer *vb = &bc->q_buf[i];

		reset_vbuf(vb);
		vb->index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QUERYBUF, vb) < 0)
			return -1;

		bc->p_buf[i].size = vb->length;
		bc->p_buf[i].data = mmap(NULL, vb->length,
					 PROT_WRITE | PROT_READ, MAP_SHARED,
					 bc->dev_fd, vb->m.offset);
		if (bc->p_buf[i].data == MAP_FAILED)
			return -1;
	}

	return 0;
}

int bc_handle_start(struct bc_handle *bc)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (ioctl(bc->dev_fd, VIDIOC_STREAMON, &type) < 0)
		return -1;

	return 0;
}

void bc_handle_stop(struct bc_handle *bc)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	while (bc->q_cnt < bc->p_cnt)
		ioctl(bc->dev_fd, VIDIOC_DQBUF, &bc->q_buf[bc->q_cnt++]);

	ioctl(bc->dev_fd, VIDIOC_STREAMOFF, &type);

	return;
}

static void bc_buf_return(struct bc_handle *bc)
{
	int i;

	/* Don't return bufs until half are used */
	if (bc->q_cnt < (bc->p_cnt >> 1))
		return;

	for (i = 0; i < bc->q_cnt; i++)
		ioctl(bc->dev_fd, VIDIOC_QBUF, &bc->q_buf[i]);

	bc->q_cnt = 0;
}

int bc_buf_get(struct bc_handle *bc)
{
	struct v4l2_buffer *vb;

	bc_buf_return(bc);

	vb = &bc->q_buf[bc->q_cnt++];
	reset_vbuf(vb);

	if (ioctl(bc->dev_fd, VIDIOC_DQBUF, vb) < 0) {
		if (errno == EIO)
			return bc_buf_get(bc);
		return -1;
	}

	if (vb->index >= bc->p_cnt)
		return -1;

	return 0;
}

struct bc_handle *bc_handle_get(const char *dev)
{
	struct bc_handle *bc;

	if ((bc = malloc(sizeof(*bc))) == NULL) {
		errno = ENOMEM;
		goto error_fail;
	}

	memset(bc, 0, sizeof(*bc));
	strncpy(bc->dev_file, dev, sizeof(bc->dev_file) - 1);
	bc->dev_file[sizeof(bc->dev_file) - 1] = '\0';
	bc->p_cnt = 8;

	/* Open the device */
	if ((bc->dev_fd = open(bc->dev_file, O_RDWR)) < 0)
		goto error_fail;

	/* Query the capabilites and verify them */
	if (ioctl(bc->dev_fd, VIDIOC_QUERYCAP, &bc->vcap) < 0)
		goto error_fail;

	if (!(bc->vcap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
	    !(bc->vcap.capabilities & V4L2_CAP_STREAMING)) {
		errno = EINVAL;
		goto error_fail;
	}

	/* Get the parameters */
	bc->vparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(bc->dev_fd, VIDIOC_G_PARM, &bc->vparm) < 0)
		goto error_fail;

	/* Get the format */
	bc->vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(bc->dev_fd, VIDIOC_G_FMT, &bc->vfmt) < 0)
		goto error_fail;

	if (bc_bufs_prepare(bc))
		goto error_fail;

	return bc;

error_fail:
	bc_handle_free(bc);
	return NULL;
}

void bc_handle_free(struct bc_handle *bc)
{
	int save_err = errno;

	if (!bc)
		return;
	if (bc->dev_fd >= 0)
		close(bc->dev_fd);
	free(bc);

	errno = save_err;
}
