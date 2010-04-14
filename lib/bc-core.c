/*
 * Copyright (C) 2010 Bluecherry, LLC
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
#include <math.h>

#include <libbluecherry.h>

#define reset_vbuf(__vb) do {				\
	memset((__vb), 0, sizeof(*(__vb)));		\
	(__vb)->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	\
	(__vb)->memory = V4L2_MEMORY_MMAP;		\
} while(0)

#define increment_idx(_x) ({ *_x = (*_x + 1) % BC_BUFFERS; *_x; })

static inline int bc_local_bufs(struct bc_handle *bc)
{
	if (bc->rd_idx > bc->wr_idx)
		return bc->rd_idx - bc->wr_idx;
	else
		return ((bc->rd_idx + BC_BUFFERS) - bc->wr_idx) % BC_BUFFERS;
}

int bc_buf_key_frame(struct bc_handle *bc)
{
	unsigned char *p = bc_buf_data(bc);
	struct v4l2_buffer *vb = bc_buf_v4l2(bc);

	/* For everything other than mpeg, every frame is a keyframe */
	if (bc->vfmt.fmt.pix.pixelformat != V4L2_PIX_FMT_MPEG)
		return 1;

	if (!p || !vb)
		return 0;

	if (vb->flags & V4L2_BUF_FLAG_KEYFRAME)
		return 1;

	/* Fallback */
	if (p[2] == 0x01 && p[3] == 0x00)
		return 1;

	return 0;
}

static int bc_bufs_prepare(struct bc_handle *bc)
{
	struct v4l2_requestbuffers req;
	int i;

	reset_vbuf(&req);
	req.count = BC_BUFFERS;

	if (ioctl(bc->dev_fd, VIDIOC_REQBUFS, &req) < 0)
		return -1;

	if (req.count != BC_BUFFERS) {
		errno = EINVAL;
		return -1;
	}

	bc->rd_idx = bc->wr_idx = 0;

	for (i = 0; i < BC_BUFFERS; i++) {
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
	int ret;
	int i;

	if (ioctl(bc->dev_fd, VIDIOC_STREAMON, &type) < 0)
		return -1;

	/* Queue all buffers */
	for (i = 0; i < BC_BUFFERS; i++) {
		ret = ioctl(bc->dev_fd, VIDIOC_QBUF, &bc->q_buf[i]);
		if (ret < 0)
			return -1;
	}

	return 0;
}

static int bc_buf_return(struct bc_handle *bc)
{
#if 0
	struct v4l2_buffer *vb = bc_buf_v4l2(bc);

	/* For motion control, only allow two buffers to be queued */
	if (vb && vb->flags & V4L2_BUF_FLAG_MOTION_ON) {
		if ((bc_local_bufs(bc) + 1) < BC_BUFFERS)
			return 0;
		ioctl(bc->dev_fd, VIDIOC_QBUF, &bc->q_buf[bc->wr_idx]);
		increment_idx(&bc->wr_idx);
	}
#endif
	/* Maintain a balance of queued and dequeued buffers */
	if (bc_local_bufs(bc) < (BC_BUFFERS_LOCAL + BC_BUFFERS_THRESH))
		return 0;

	while (bc_local_bufs(bc) > BC_BUFFERS_LOCAL) {
		ioctl(bc->dev_fd, VIDIOC_QBUF, &bc->q_buf[bc->wr_idx]);
		increment_idx(&bc->wr_idx);
	}

	return 0;
}

int bc_buf_get(struct bc_handle *bc)
{
	struct v4l2_buffer *vb;

	if (bc_buf_return(bc))
		return errno;

	vb = &bc->q_buf[bc->rd_idx];
	reset_vbuf(vb);

	if (ioctl(bc->dev_fd, VIDIOC_DQBUF, vb) < 0) {
		if (errno == EIO) {
			increment_idx(&bc->rd_idx);
			errno = EAGAIN;
		}
		return errno;
	}

	increment_idx(&bc->rd_idx);

	/* If no motion detection, then carry on normally */
	if (!(bc_buf_v4l2(bc)->flags & V4L2_BUF_FLAG_MOTION_ON)) {
		/* Reset this counter in case motion gets turned back on */
		bc->mot_cnt = 0;

		/* Skip frames until the first key frame */
		if (!bc->got_vop) {
			if (!bc_buf_key_frame(bc))
				return EAGAIN;
			bc->got_vop = 1;
		}

		return 0;
	}

	/* XXX We can reset the vop without returning an ERESTART which
	 * would cause video artifacting. */
	/* Motion flag resets counter */
	if (bc_buf_v4l2(bc)->flags & V4L2_BUF_FLAG_MOTION_DETECTED) {
		if (bc->mot_cnt == 0)
			bc->got_vop = 0;
		bc->mot_cnt = 60;
	}

	/* If motion count is 0, signal EOF */
	if (bc->mot_cnt == 0)
		return ERESTART;

	/* Skip frames until the first key frame */
	if (!bc->got_vop) {
		if (!bc_buf_key_frame(bc))
			return EAGAIN; 
		bc->got_vop = 1;
	}

	bc->mot_cnt--;

	return 0;
}

int bc_set_interval(struct bc_handle *bc, u_int8_t interval)
{
	struct v4l2_control vc;
	double den = bc->vparm.parm.capture.timeperframe.denominator;
	double num = interval;

	bc->vparm.parm.capture.timeperframe.numerator = interval;
	if (ioctl(bc->dev_fd, VIDIOC_S_PARM, &bc->vparm) < 0)
		return -1;

	/* Reset GOP */
	bc->gop = lround(den / num);
	if (!bc->gop)
		bc->gop = 1;
	vc.id = V4L2_CID_MPEG_VIDEO_GOP_SIZE;
	vc.value = bc->gop;
	if (ioctl(bc->dev_fd, VIDIOC_S_CTRL, &vc) < 0)
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
