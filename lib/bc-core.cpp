/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdio.h>
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
#include <limits.h>
#include <assert.h>
#include <bsd/string.h>

#include "libbluecherry.h"
#include "lavf_device.h"
#include "v4l2_device.h"
#include "stream_elements.h"

#define reset_vbuf(__vb) do {				\
	memset((__vb), 0, sizeof(*(__vb)));		\
	(__vb)->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	\
	(__vb)->memory = V4L2_MEMORY_MMAP;		\
} while(0)

#define RETERR(__msg) ({		\
	if (err_msg)			\
		*err_msg = __msg;	\
	return -1;			\
})

#if 0
int bc_buf_key_frame(struct bc_handle *bc)
{
	struct v4l2_buffer *vb;

	if (bc->type == BC_DEVICE_LAVF)
		return lavf_device_frame_is_keyframe(&bc->rtp);

	if (bc->type != BC_DEVICE_V4L2)
		return 1;

	vb = bc_buf_v4l2(bc);

	if (bc->v4l2.vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MJPEG)
		return 1;

	if (vb && vb->flags & V4L2_BUF_FLAG_KEYFRAME)
		return 1;

	return 0;
}

int bc_buf_is_video_frame(struct bc_handle *bc)
{
	if (bc->type == BC_DEVICE_LAVF)
		return bc->rtp.frame.stream_index == bc->rtp.video_stream_index;
	return 1;
}
#endif

#if 0
int bc_buf_get(struct bc_handle *bc)
{
	int ret = EINVAL;

	if (bc->type == BC_DEVICE_LAVF) {
		ret = lavf_device_read(&bc->rtp);
	} else if (bc->type == BC_DEVICE_V4L2) {
		struct v4l2_buffer vb;
		bc_buf_return(bc);
		reset_vbuf(&vb);

		ret = ioctl(bc->v4l2.dev_fd, VIDIOC_DQBUF, &vb);
		bc->v4l2.local_bufs++;
		/* XXX better error handling here */
		if (ret)
			return EAGAIN;

		/* Update and store this buffer */
		bc->v4l2.buf_idx = vb.index;
		bc->v4l2.p_buf[bc->v4l2.buf_idx].vb = vb;

		if (!bc->got_vop) {
			if (!bc_buf_key_frame(bc))
				return EAGAIN;
			bc->got_vop = 1;
		}

		ret = 0;
	}

	return ret;
}
#endif

static inline char *split(char *s, char delim)
{
	s = strchr(s, delim);
	if (s && (*s = 0, !*++s))
		s = NULL;
	return s;
}

static inline void split_pp(char *s, const char **p1, const char **p2)
{
	char *tmp = split(s, '|');
	*p1 = tmp;
	*p2 = split(tmp, '|');
}

static int get_creds(BC_DB_RES dbres, char *creds, size_t size)
{
	const char *user, *pass;

	user = bc_db_get_val(dbres, "rtsp_username", NULL);
	pass = bc_db_get_val(dbres, "rtsp_password", NULL);

	if (!user || !pass)
		return -1;

	if (*user || *pass) {
		size_t s = snprintf(creds, size, "%s:%s@", user, pass);
		if (s >= size)
			return -1;
	} else {
		*creds = '\0';
	}

	return 0;
}

/**
 * Parse device path.
 * It is in the questionable format of 'hostname|port|path'.
 *
 * @returns 0 on success, -1 on error.
 */
static int parse_dev_path(char *dst, size_t sz, const char *val,
			  const char *creds, const char *defhost,
			  const char *defport, const char *proto)
{
	char *spec = strdupa(val);
	const char *port, *path;
	unsigned int r;

	split_pp(spec, &port, &path);

	/* XXX: Workaround. Sometimes hostname isn't present.
	 * Should be removed at some point. */
	const char *host = *spec ? spec : defhost;

	if (!path) {
		/* Broken spec. */
		return -1;
	}

	if (!port)
		port = defport;

	/* Create the URL */
	r = snprintf(dst, sz, "%s://%s%s:%s%s", proto, creds, host, port, path);

	if (r >= sz)
		return -1;

	return 0;
}

static int lavf_handle_init(struct bc_handle *bc, BC_DB_RES dbres)
{
	const char *val;
	assert(strlen(bc->driver));
	bc->type = BC_DEVICE_LAVF;

	/* FIXME bc_device_config is already parsed from DB and saved in void*
	 * __data, but completely unused and unreachable (bc_record type belongs to
	 * server and not to lib) */

	char creds[64];
	if (get_creds(dbres, creds, sizeof(creds)) < 0)
		return -1;

	bool rtsp_rtp_prefer_tcp = true;
	const char *protocol = bc_db_get_val(dbres, "protocol", NULL);
	const char *uri_schema;
	if (!strcmp(protocol, "IP-MJPEG")) {
		uri_schema = "http";
		val = bc_db_get_val(dbres, "mjpeg_path", NULL);
	} else {
		uri_schema = "rtsp";

		const char *rtsp_rtp_prefer_tcp_raw = bc_db_get_val(dbres, "rtsp_rtp_prefer_tcp", NULL);
		if (rtsp_rtp_prefer_tcp_raw && strlen(rtsp_rtp_prefer_tcp_raw)) {
			rtsp_rtp_prefer_tcp = atoi(rtsp_rtp_prefer_tcp_raw) ? true : false;
		} else {
			bc_log(Error, "rtsp_rtp_prefer_tcp field is absent in Devices table, "
					"DB schema not updated. Using default field value, \"true\"");
		}

		val = bc_db_get_val(dbres, "device", NULL);
	}

	if (val && *val) {
		char url[1024];

		int r = parse_dev_path(url, sizeof(url), val, creds,
				"XXX", /* host should be always present, this default doesn't matter */
				"554", /* port should be always present, this default doesn't matter */
				uri_schema);
		if (r)
			return -1;

		bc->input = new lavf_device(url, rtsp_rtp_prefer_tcp);
	}

	/* This `defhost` is a workaround for empty host part in mjpeg_path */
	char *defhost = strdupa(val);
	{
		char *p = strchr(defhost, '|');
		if (p)
			*p = 0;
	}

	val = bc_db_get_val(dbres, "mjpeg_path", NULL);
	if (val && *val) {
		int r = parse_dev_path(bc->mjpeg_url, sizeof(bc->mjpeg_url),
				       val, creds, defhost, "80", "http");

		if (r)
			return -1;
	}

	return 0;
}

struct bc_handle *bc_handle_get(BC_DB_RES dbres)
{
	const char *device, *driver;
	struct bc_handle *bc;
	int ret;

	if (dbres == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	device = bc_db_get_val(dbres, "device", NULL);
	driver = bc_db_get_val(dbres, "driver", NULL);
	if (!device || !driver) {
		errno = EINVAL;
		return NULL;
	}

	bc = (struct bc_handle*) malloc(sizeof(*bc));
	if (bc == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	memset(bc, 0, sizeof(*bc));

	strcpy(bc->device, device);
	strcpy(bc->driver, driver);

	bc_ptz_check(bc, dbres);

	if (!strncmp(driver, "RTSP-", 5)
			|| !strncmp(driver, "HTTP-", 5))
		ret = lavf_handle_init(bc, dbres);
	else {
		bc->type = BC_DEVICE_V4L2;
		bc->input = new v4l2_device(dbres);
		ret = reinterpret_cast<v4l2_device*>(bc->input)->has_error();
	}

	bc->source = new stream_source("Input Source");

	if (ret) {
		bc_handle_free(bc);
		return NULL;
	}

	return bc;
}

void bc_handle_free(struct bc_handle *bc)
{
	/* Don't want this call to change errno at all */
	int save_err = errno;

	if (!bc)
		return;

	if (bc->input)
		bc->input->stop();

	delete bc->input;
	delete bc->source;
	free(bc);

	errno = save_err;
}

int bc_device_config_init(struct bc_device_config *cfg, BC_DB_RES dbres)
{
	const char *dev = bc_db_get_val(dbres, "device", NULL);
	const char *name = bc_db_get_val(dbres, "device_name", NULL);
	const char *driver = bc_db_get_val(dbres, "driver", NULL);
	const char *schedule = bc_db_get_val(dbres, "schedule", NULL);
	const char *motion_map = bc_db_get_val(dbres, "motion_map", NULL);
	const char *signal_type = bc_db_get_val(dbres, "signal_type", NULL);
	const char *rtsp_username = bc_db_get_val(dbres, "rtsp_username", NULL);
	const char *rtsp_password = bc_db_get_val(dbres, "rtsp_password", NULL);
	const char *rtsp_rtp_prefer_tcp_raw = bc_db_get_val(dbres, "rtsp_rtp_prefer_tcp", NULL);

	if (!dev || !name || !driver || !schedule || !motion_map)
		return -1;

	if (rtsp_rtp_prefer_tcp_raw && strlen(rtsp_rtp_prefer_tcp_raw)) {
		cfg->rtsp_rtp_prefer_tcp = atoi(rtsp_rtp_prefer_tcp_raw) ? true : false;
	} else {
		bc_log(Error, "rtsp_rtp_prefer_tcp field is absent in Devices table, "
				"DB schema not updated. Using default field value, \"true\"");
		cfg->rtsp_rtp_prefer_tcp = true;
	}

	strlcpy(cfg->dev, dev, sizeof(cfg->dev));
	strlcpy(cfg->name, name, sizeof(cfg->name));
	strlcpy(cfg->driver, driver, sizeof(cfg->driver));
	strlcpy(cfg->schedule, schedule, sizeof(cfg->schedule));
	strlcpy(cfg->motion_map, motion_map, sizeof(cfg->motion_map));

	if (signal_type)
		strlcpy(cfg->signal_type, signal_type, sizeof(cfg->signal_type));
	if (rtsp_username)
		strlcpy(cfg->rtsp_username, rtsp_username, sizeof(cfg->rtsp_username));
	if (rtsp_password)
		strlcpy(cfg->rtsp_password, rtsp_password, sizeof(cfg->rtsp_password));

	cfg->width = bc_db_get_val_int(dbres, "resolutionX");
	cfg->height = bc_db_get_val_int(dbres, "resolutionY");
	cfg->interval = bc_db_get_val_int(dbres, "video_interval");
	cfg->prerecord = (int16_t)bc_db_get_val_int(dbres, "buffer_prerecording");
	cfg->postrecord = (int16_t)bc_db_get_val_int(dbres, "buffer_postrecording");
	cfg->motion_analysis_stw = (int64_t)bc_db_get_val_int(dbres, "motion_analysis_stw");
	if (cfg->motion_analysis_stw == -1) {
		bc_log(Error, "motion_analysis_stw field is likely absent, DB schema not updated. Using default value");
		cfg->motion_analysis_stw = 200000;  // 0.2 second
	}
	cfg->motion_analysis_percentage = bc_db_get_val_int(dbres, "motion_analysis_percentage");
	if (cfg->motion_analysis_percentage == -1) {
		bc_log(Error, "motion_analysis_percentage field is likely absent, DB schema not updated. Using default value");
		cfg->motion_analysis_percentage = 50;
	}
	cfg->debug_level = (int8_t)bc_db_get_val_int(dbres, "debug_level");
	cfg->aud_disabled = bc_db_get_val_int(dbres, "audio_disabled") != 0;
	cfg->schedule_override_global = bc_db_get_val_int(dbres, "schedule_override_global") != 0;
	cfg->hue        = bc_db_get_val_int(dbres, "hue");
	cfg->contrast   = bc_db_get_val_int(dbres, "contrast");
	cfg->brightness = bc_db_get_val_int(dbres, "brightness");
	cfg->saturation = bc_db_get_val_int(dbres, "saturation");
	cfg->video_quality = bc_db_get_val_int(dbres, "video_quality");

	return 0;
}

