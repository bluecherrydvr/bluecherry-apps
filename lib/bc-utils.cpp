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

int bc_set_mjpeg(struct bc_handle *bc)
{
	if (bc->type != BC_DEVICE_V4L2)
		return -1;

	bc->v4l2.vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	if (ioctl(bc->v4l2.dev_fd, VIDIOC_S_FMT, &bc->v4l2.vfmt) < 0)
		return -1;

	return 0;
}

/* Check range and convert our 0-100 to valid ranges in the hardware */
int bc_set_control(struct bc_handle *bc, unsigned int ctrl, int val)
{
	struct v4l2_queryctrl qctrl;
	struct v4l2_control vc;
	float step;

	if (bc->type != BC_DEVICE_V4L2)
		return 0;

	memset(&qctrl, 0, sizeof(qctrl));
	qctrl.id = ctrl;
	if (ioctl(bc->v4l2.dev_fd, VIDIOC_QUERYCTRL, &qctrl) < 0)
		return -1;

	step = (float)(qctrl.maximum - qctrl.minimum) / (float)101;
	val = roundf(((float)val * step) + qctrl.minimum);

	vc.id = ctrl;
	vc.value = val;

	return ioctl(bc->v4l2.dev_fd, VIDIOC_S_CTRL, &vc);
}

void *bc_buf_data(struct bc_handle *bc)
{
	if (bc->type == BC_DEVICE_RTP) {
		if (bc->rtp.frame.stream_index == bc->rtp.video_stream_index)
			return bc->rtp.frame.data;
		return NULL;
	}

	if (bc->v4l2.buf_idx < 0)
		return NULL;

	return bc->v4l2.p_buf[bc->v4l2.buf_idx].data;
}

unsigned int bc_buf_size(struct bc_handle *bc)
{
	if (bc->type == BC_DEVICE_RTP) {
		if (bc->rtp.frame.stream_index == bc->rtp.video_stream_index)
			return bc->rtp.frame.size;
		return 0;
	}

	if (bc->v4l2.buf_idx < 0)
		return 0;

	return bc->v4l2.p_buf[bc->v4l2.buf_idx].vb.bytesused;
}

int bc_set_format(struct bc_handle *bc, u_int32_t fmt, u_int16_t width,
		  u_int16_t height)
{
	if (bc->type != BC_DEVICE_V4L2)
		return 0;

	if (bc->v4l2.vfmt.fmt.pix.pixelformat == fmt &&
	    bc->v4l2.vfmt.fmt.pix.width == width &&
	    bc->v4l2.vfmt.fmt.pix.height == height)
		return 0;

	if (bc->started) {
		errno = EAGAIN;
		return -1;
	}

	bc->v4l2.vfmt.fmt.pix.pixelformat = fmt;
	bc->v4l2.vfmt.fmt.pix.width = width;
	bc->v4l2.vfmt.fmt.pix.height = height;

	if (ioctl(bc->v4l2.dev_fd, VIDIOC_S_FMT, &bc->v4l2.vfmt) < 0)
		return -1;
	ioctl(bc->v4l2.dev_fd, VIDIOC_G_FMT, &bc->v4l2.vfmt);

	return 0;
}

int bc_set_osd(struct bc_handle *bc, char *fmt, ...)
{
	char buf[256];
	va_list ap;
	struct v4l2_ext_control ctrl;
	struct v4l2_ext_controls ctrls;

	if (bc->type != BC_DEVICE_V4L2)
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

	if (ioctl(bc->v4l2.dev_fd, VIDIOC_S_EXT_CTRLS, &ctrls) < 0)
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
