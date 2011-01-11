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

static inline int bc_local_bufs(struct bc_handle *bc)
{
	int i, c;

	for (i = c = 0; i < bc->buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QUERYBUF, &vb) < 0)
			continue;

		if (vb.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE))
			continue;

		c++;
	}

	return c;
}

int bc_buf_key_frame(struct bc_handle *bc)
{
	struct v4l2_buffer *vb = bc_buf_v4l2(bc);

	/* For everything other than mpeg, every frame is a keyframe */
	if (bc->vfmt.fmt.pix.pixelformat != V4L2_PIX_FMT_MPEG)
		return 1;

	if (vb && vb->flags & V4L2_BUF_FLAG_KEYFRAME)
		return 1;

	return 0;
}

static int bc_bufs_prepare(struct bc_handle *bc)
{
	struct v4l2_requestbuffers req;
	int i;

	reset_vbuf(&req);
	req.count = bc->buffers;

	if (ioctl(bc->dev_fd, VIDIOC_REQBUFS, &req) < 0)
		return -1;

	if (req.count != bc->buffers) {
		errno = EINVAL;
		return -1;
	}

	for (i = 0; i < bc->buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QUERYBUF, &vb) < 0)
			return -1;

		bc->p_buf[i].size = vb.length;
		bc->p_buf[i].data = mmap(NULL, vb.length,
					 PROT_WRITE | PROT_READ, MAP_SHARED,
					 bc->dev_fd, vb.m.offset);
		if (bc->p_buf[i].data == MAP_FAILED)
			return -1;
	}

	return 0;
}

int bc_handle_start(struct bc_handle *bc)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i;

	if (bc->started)
		return 0;

	/* For mpeg, we get the max, and for mjpeg the min */
	if (bc->vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MPEG)
		bc->buffers = BC_BUFFERS;
	else
		bc->buffers = BC_BUFFERS_JPEG;

	if (bc_bufs_prepare(bc))
		return -1;

	if (ioctl(bc->dev_fd, VIDIOC_STREAMON, &type) < 0)
		return -1;

	/* Queue all buffers */
	for (i = 0; i < bc->buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QUERYBUF, &vb) < 0)
			return -1;

		if (vb.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE))
			continue;

		if (ioctl(bc->dev_fd, VIDIOC_QBUF, &vb) < 0)
			return -1;
	}

	bc->buf_idx = -1;
	bc->started = 1;

	return 0;
}

static void bc_buf_return(struct bc_handle *bc)
{
	int local = (bc->buffers / 2) - 1;
	int thresh = (bc->buffers - local) / 2;
	int cur = bc_local_bufs(bc);
	int i;

	/* Maintain a balance of queued and dequeued buffers */
	if (cur < (local + thresh))
		return;

	for (i = 0; i < bc->buffers && cur > local; i++) {
		struct v4l2_buffer vb;

		if (bc->p_buf[i].status != BC_VB_STATUS_LOCAL)
			continue;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QBUF, &vb) == 0) {
			cur--;
			bc->p_buf[i].status = BC_VB_STATUS_QUEUED;
		}
	}

	return;
}

void (*bc_handle_motion_start)(struct bc_handle *bc) = NULL;
void (*bc_handle_motion_end)(struct bc_handle *bc) = NULL;

static void __bc_start_motion_event(struct bc_handle *bc)
{
	if (bc_handle_motion_start)
		bc_handle_motion_start(bc);
}

static void __bc_stop_motion_event(struct bc_handle *bc)
{
	if (bc_handle_motion_end)
		bc_handle_motion_end(bc);
}

int bc_buf_get(struct bc_handle *bc)
{
	struct v4l2_buffer vb;
	int ret;

	bc_buf_return(bc);

	reset_vbuf(&vb);

	if (ioctl(bc->dev_fd, VIDIOC_DQBUF, &vb) < 0)
		return EAGAIN;

	/* Mark old buffer LOCAL */
	bc->p_buf[bc->buf_idx].status = BC_VB_STATUS_LOCAL;

	/* Update and mark this buffer USING */
	bc->buf_idx = vb.index;
	bc->p_buf[vb.index].vb = vb;
	bc->p_buf[vb.index].status = BC_VB_STATUS_USING;

	/* If no motion detection, then carry on normally */
	if (!(bc_buf_v4l2(bc)->flags & V4L2_BUF_FLAG_MOTION_ON)) {
		/* Reset this counter in case motion gets turned back on */
		bc->mot_cnt = 0;
		/* Call this just in case we have an event in progress. */
		__bc_stop_motion_event(bc);

		if (!bc->got_vop) {
			if (!bc_buf_key_frame(bc)) 
				return EAGAIN;
			bc->got_vop = 1;
		}
		return 0;
        }

	/* Normally for motion, we return EAGAIN, which means skip this,
	 * frame. This can be 0 for "motion in progress, record frame" or
	 * ERESTART for "motion just ended, stop this recording". */
        ret = EAGAIN;

	if (bc_buf_v4l2(bc)->flags & V4L2_BUF_FLAG_MOTION_DETECTED) {
		if (bc->mot_cnt == 0) {
			bc->got_vop = 0;
			// First time, send event
			__bc_start_motion_event(bc);
		}
		/* Reset this counter every time we get a new event */
		bc->mot_cnt = 60;
	}

	if (bc->mot_cnt && !bc->got_vop) {
		if (!bc_buf_key_frame(bc))
			return EAGAIN;
		bc->got_vop = 1;
	}

	if (bc->mot_cnt == 1) {
		/* End of event */
		bc->mot_cnt = 0;
		ret = ERESTART;
		__bc_stop_motion_event(bc);
	} else if (bc->mot_cnt) {
		bc->mot_cnt--;
		ret = 0;
	}

	return ret;
}

int bc_set_interval(struct bc_handle *bc, u_int8_t interval)
{
	struct v4l2_control vc;
	double den = bc->vparm.parm.capture.timeperframe.denominator;
	double num = interval;

	if (!interval)
		return 0;

	if (bc->vparm.parm.capture.timeperframe.numerator == interval)
		return 0;

	if (bc->started) {
		errno = EAGAIN;
		return -1;
	}

	bc->vparm.parm.capture.timeperframe.numerator = interval;
	if (ioctl(bc->dev_fd, VIDIOC_S_PARM, &bc->vparm) < 0)
		return -1;
	ioctl(bc->dev_fd, VIDIOC_G_PARM, &bc->vparm);

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

	return bc;

error_fail:
	bc_handle_free(bc);
	return NULL;
}

void bc_handle_stop(struct bc_handle *bc)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	/* Don't want this call to change errno at all */
	int save_err = errno;
	int i;

	if (!bc || bc->dev_fd < 0 || !bc->started)
		return;

	for (i = 0; i < bc->buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QUERYBUF, &vb) < 0)
			continue;

		if (!(vb.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE)))
			continue;

		ioctl(bc->dev_fd, VIDIOC_DQBUF, &vb);
	}

	/* Stop the stream */
	ioctl(bc->dev_fd, VIDIOC_STREAMOFF, &type);

	/* Unmap all buffers */
	for (i = 0; i < bc->buffers; i++)
		munmap(bc->p_buf[i].data, bc->p_buf[i].size);

	bc->started = 0;
	bc->buf_idx = -1;

	errno = save_err;
}

void bc_handle_free(struct bc_handle *bc)
{
	/* Don't want this call to change errno at all */
	int save_err = errno;

	if (!bc)
		return;

	bc_handle_stop(bc);

	close(bc->dev_fd);
	free(bc);

	errno = save_err;
}
