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

#include "libbluecherry.h"
#include <bsd/string.h>

extern "C" {
#include <libavutil/md5.h>
}

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

/**
 * Get the best of the supported pixel formats the card provides.
 *
 * XXX: We take the first one because it happens to be MPEG4 or H264 on our
 * driver.
 */
int get_best_pixfmt(int fd)
{
	struct v4l2_fmtdesc fmt = {
		.index = 0,
		.type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
	};
	int ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmt);
	return ret < 0 ? -1 : fmt.pixelformat;
}

int bc_set_resolution(struct bc_handle *bc, u_int16_t width, u_int16_t height)
{
	if (bc->type != BC_DEVICE_V4L2)
		return 0;

	if (bc->v4l2.vfmt.fmt.pix.width == width &&
	    bc->v4l2.vfmt.fmt.pix.height == height)
		return 0;

	if (bc->started) {
		errno = EAGAIN;
		return -1;
	}

	int fmt = get_best_pixfmt(bc->v4l2.dev_fd);
	if (fmt < 0)
		return -1;

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

int bc_user_auth(const char *username, const char *password, int access_type, int device_id)
{
	int re = 0;
	char md5_pwd[16], s_password[256];
	const char *tmp, *access_field = 0;
	char *s_username = bc_db_escape_string(username, strlen(username));
	BC_DB_RES dbres = bc_db_get_table("SELECT * FROM Users WHERE username='%s'", s_username);
	free(s_username);
	if (!dbres)
		return -1;

	if (bc_db_fetch_row(dbres))
		goto end;

	strlcpy(s_password, password, sizeof(s_password));
	strlcat(s_password, bc_db_get_val(dbres, "salt", NULL), sizeof(s_password));
	av_md5_sum((uint8_t*)md5_pwd, (const uint8_t*)s_password, strlen(s_password));
	hex_encode(s_password, sizeof(s_password), md5_pwd, sizeof(md5_pwd));

	tmp = bc_db_get_val(dbres, "password", NULL);
	if (strcmp(tmp, s_password) != 0)
		goto end;

	switch (access_type) {
		case ACCESS_SETUP:  access_field = "access_setup";
		case ACCESS_REMOTE: access_field = "access_remote";
		case ACCESS_WEB:    access_field = "access_web";
		case ACCESS_BACKUP: access_field = "access_backup";
		case ACCESS_RELAY:  access_field = "access_relay";
		default:            access_field = 0;
	}

	if (access_field && bc_db_get_val_int(dbres, access_field) != 1)
		goto end;

	if (device_id >= 0) {
		const char *reject_devices = bc_db_get_val(dbres, "access_device_list", NULL);
		char *p = (char*)reject_devices, *token;
		while ((token = strsep(&p, ","))) {
			if (atoi(token) == device_id)
				goto end;
		}
	}

	re = 1;
end:
	bc_db_free_table(dbres);
	return re;
}

int hex_encode(char *out, int out_sz, const char *in, int in_sz)
{
	int i;
	static const char table[] = "0123456789abcdef";

	if (out_sz < in_sz * 2 + 1) {
		*out = 0;
		return 0;
	}

	for (i = 0; i < in_sz; ++i) {
		*(out++) = table[(in[i] >> 4) & 0xf];
		*(out++) = table[in[i] & 0xf];
	}

	*out = 0;
	return in_sz*2;
}

