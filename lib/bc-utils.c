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

#include <libbluecherry.h>

int bc_set_motion(struct bc_handle *bc, int on)
{
	struct v4l2_control vc;

	vc.id = V4L2_CID_MOTION_ENABLE;
	vc.value = on ? 1 : 0;
	if (ioctl(bc->dev_fd, VIDIOC_S_CTRL, &vc) < 0)
		return -1;
	return 0;
}

static inline int bc_current_buf(struct bc_handle *bc)
{
	if (bc->rd_idx == bc->wr_idx)
		return -1;
	return (bc->rd_idx + (BC_BUFFERS - 1)) % BC_BUFFERS;
}

void *bc_buf_data(struct bc_handle *bc)
{
	int idx = bc_current_buf(bc);

	if (idx < 0)
		return NULL;

	return bc->p_buf[bc->q_buf[idx].index].data;
}

unsigned int bc_buf_size(struct bc_handle *bc)
{
	int idx = bc_current_buf(bc);

	if (idx < 0)
		return 0;

	return bc->q_buf[idx].bytesused;
}

struct v4l2_buffer *bc_buf_v4l2(struct bc_handle *bc)
{
	int idx = bc_current_buf(bc);

	if (idx < 0)
		return NULL;

	return &bc->q_buf[idx];
}

int bc_set_format(struct bc_handle *bc, u_int32_t fmt, u_int16_t width,
		  u_int16_t height)
{
	if (fmt)
		bc->vfmt.fmt.pix.pixelformat = fmt;
	if (width)
		bc->vfmt.fmt.pix.width = width;
	if (height)
		bc->vfmt.fmt.pix.height = height;

	if (ioctl(bc->dev_fd, VIDIOC_S_FMT, &bc->vfmt) < 0)
		return -1;

	return 0;
}

int bc_set_osd(struct bc_handle *bc, char *str)
{
	struct v4l2_ext_control ctrl;
	struct v4l2_ext_controls ctrls;

	memset(&ctrl, 0, sizeof(ctrl));
	memset(&ctrls, 0, sizeof(ctrls));

	ctrls.count = 1;
	ctrls.ctrl_class = V4L2_CTRL_CLASS_FM_TX;
	ctrls.controls = &ctrl;
	ctrl.id = V4L2_CID_RDS_TX_RADIO_TEXT;
	ctrl.size = strlen(str);
	ctrl.string = str;

	if (ioctl(bc->dev_fd, VIDIOC_S_EXT_CTRLS, &ctrls) < 0)
		return -1;

	return 0;
}

extern char *__progname;
#define BC_LOG_SERVICE	LOG_LOCAL4

void bc_log(char *msg, ...)
{
	va_list ap;
	static int log_open = 0;

	if (!log_open) {
		openlog(__progname, LOG_PID | LOG_PERROR, BC_LOG_SERVICE);
		log_open = 1;
	}

	va_start(ap, msg);
	vsyslog(LOG_INFO | BC_LOG_SERVICE, msg, ap);
	va_end(ap);
}
