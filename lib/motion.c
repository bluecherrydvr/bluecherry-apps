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
#include <stdarg.h>
#include <syslog.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <libbluecherry.h>

struct v4l2_buffer *bc_buf_v4l2(struct bc_handle *bc);

int bc_set_motion(struct bc_handle *bc, int on)
{
	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;

		vc.id = V4L2_CID_MOTION_ENABLE;
		vc.value = on ? 1 : 0;

		return ioctl(bc->v4l2.dev_fd, VIDIOC_S_CTRL, &vc);
	}

	return 0;
}

int bc_set_motion_thresh_global(struct bc_handle *bc, unsigned short val)
{
	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;

		vc.id = V4L2_CID_MOTION_THRESHOLD;
		vc.value = val;
		/* Upper 16 bits left to 0 for global */

		return ioctl(bc->v4l2.dev_fd, VIDIOC_S_CTRL, &vc);
	}

	return 0;
}

int bc_set_motion_thresh(struct bc_handle *bc, unsigned short val,
			 unsigned short block)
{
	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;

		vc.id = V4L2_CID_MOTION_THRESHOLD;
		vc.value = val;
		/* 0 means global; we must add one to the actual block */
		vc.value |= (unsigned int)(block+1) << 16;

		return ioctl(bc->v4l2.dev_fd, VIDIOC_S_CTRL, &vc);
	}

	return 0;
}

int bc_motion_is_on(struct bc_handle *bc)
{
	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_buffer *vb;

		vb = bc_buf_v4l2(bc);
		if (vb == NULL)
			return 0;

		return vb->flags & V4L2_BUF_FLAG_MOTION_ON ? 1 : 0;
	}
	return 0;
}

int bc_motion_is_detected(struct bc_handle *bc)
{
	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_buffer *vb;

		if (!bc_motion_is_on(bc))
			return 0;

		vb = bc_buf_v4l2(bc);
		if (vb == NULL)
			return 0;

		return vb->flags & V4L2_BUF_FLAG_MOTION_DETECTED ? 1 : 0;
	}
	return 0;
}
