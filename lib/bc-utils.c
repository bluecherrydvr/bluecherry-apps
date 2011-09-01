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

int bc_set_motion(struct bc_handle *bc, int on)
{
	struct v4l2_control vc;

	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

	vc.id = V4L2_CID_MOTION_ENABLE;
	vc.value = on ? 1 : 0;

	return ioctl(bc->dev_fd, VIDIOC_S_CTRL, &vc);
}

int bc_set_motion_thresh_global(struct bc_handle *bc, unsigned short val)
{
	struct v4l2_control vc;
	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

	vc.id = V4L2_CID_MOTION_THRESHOLD;
	vc.value = val;
	/* Upper 16 bits left to 0 for global */

	return ioctl(bc->dev_fd, VIDIOC_S_CTRL, &vc);
}

int bc_set_motion_thresh(struct bc_handle *bc, unsigned short val,
			 unsigned short block)
{
	struct v4l2_control vc;

	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

	vc.id = V4L2_CID_MOTION_THRESHOLD;
	vc.value = val;
	/* 0 means global; we must add one to the actual block */
	vc.value |= (unsigned int)(block+1) << 16;

	return ioctl(bc->dev_fd, VIDIOC_S_CTRL, &vc);
}

int bc_set_mjpeg(struct bc_handle *bc)
{
	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return -1;

	bc->vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	if (ioctl(bc->dev_fd, VIDIOC_S_FMT, &bc->vfmt) < 0)
		return -1;

	return 0;
}

/* Check range and convert our 0-100 to valid ranges in the hardware */
int bc_set_control(struct bc_handle *bc, unsigned int ctrl, int val)
{
	struct v4l2_queryctrl qctrl;
	struct v4l2_control vc;
	float step;

	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

	memset(&qctrl, 0, sizeof(qctrl));
	qctrl.id = ctrl;
	if (ioctl(bc->dev_fd, VIDIOC_QUERYCTRL, &qctrl) < 0)
		return -1;

	step = (float)(qctrl.maximum - qctrl.minimum) / (float)101;
	val = roundf(((float)val * step) + qctrl.minimum);

	vc.id = ctrl;
	vc.value = val;

	return ioctl(bc->dev_fd, VIDIOC_S_CTRL, &vc);
}

void *bc_buf_data(struct bc_handle *bc)
{
	if (bc->cam_caps & BC_CAM_CAP_RTSP) {
		if (bc->rtp_sess.vid_valid)
			return bc->rtp_sess.vid_buf;
		return NULL;
	}

	if (bc->buf_idx < 0)
		return NULL;

	return bc->p_buf[bc->buf_idx].data;
}

unsigned int bc_buf_size(struct bc_handle *bc)
{
	if (bc->cam_caps & BC_CAM_CAP_RTSP) {
		if (bc->rtp_sess.vid_valid)
			return bc->rtp_sess.vid_len;
		return 0;
	}

	if (bc->buf_idx < 0)
		return 0;

	return bc->p_buf[bc->buf_idx].vb.bytesused;
}

int bc_set_format(struct bc_handle *bc, u_int32_t fmt, u_int16_t width,
		  u_int16_t height)
{
	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

	if (bc->vfmt.fmt.pix.pixelformat == fmt &&
	    bc->vfmt.fmt.pix.width == width &&
	    bc->vfmt.fmt.pix.height == height)
		return 0;

	if (bc->started) {
		errno = EAGAIN;
		return -1;
	}

	bc->vfmt.fmt.pix.pixelformat = fmt;
	bc->vfmt.fmt.pix.width = width;
	bc->vfmt.fmt.pix.height = height;

	if (ioctl(bc->dev_fd, VIDIOC_S_FMT, &bc->vfmt) < 0)
		return -1;
	ioctl(bc->dev_fd, VIDIOC_G_FMT, &bc->vfmt);

	return 0;
}

int bc_set_osd(struct bc_handle *bc, char *fmt, ...)
{
	char buf[256];
	va_list ap;
	struct v4l2_ext_control ctrl;
	struct v4l2_ext_controls ctrls;

	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	buf[sizeof(buf) - 1] = '\0';

	memset(&ctrl, 0, sizeof(ctrl));
	memset(&ctrls, 0, sizeof(ctrls));

	ctrls.count = 1;
	ctrls.ctrl_class = V4L2_CTRL_CLASS_FM_TX;
	ctrls.controls = &ctrl;
	ctrl.id = V4L2_CID_RDS_TX_RADIO_TEXT;
	ctrl.size = strlen(buf);
	ctrl.string = buf;

	if (ioctl(bc->dev_fd, VIDIOC_S_EXT_CTRLS, &ctrls) < 0)
		return -1;

	return 0;
}

extern char *__progname;
#define BC_LOG_SERVICE	LOG_DAEMON

void bc_log(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	bc_vlog(msg, ap);
	va_end(ap);
}

void bc_vlog(const char *msg, va_list ap)
{
	static int log_open = 0;

	if (!log_open) {
		openlog(__progname, LOG_PID | LOG_PERROR, BC_LOG_SERVICE);
		log_open = 1;
	}

	vsyslog(LOG_INFO | BC_LOG_SERVICE, msg, ap);
}

time_t bc_gettime_monotonic()
{
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0) {
		bc_log("E: clock_gettime failed: %d", (int)errno);
		return 0;
	}

	return ts.tv_sec;
}
