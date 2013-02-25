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
#include <limits.h>
#include <bsd/string.h>

#include "libbluecherry.h"
#include "rtp_device.h"
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

	if (bc->type == BC_DEVICE_RTP)
		return rtp_device_frame_is_keyframe(&bc->rtp);

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
	if (bc->type == BC_DEVICE_RTP)
		return bc->rtp.frame.stream_index == bc->rtp.video_stream_index;
	return 1;
}
#endif

#if 0
int bc_buf_get(struct bc_handle *bc)
{
	int ret = EINVAL;

	if (bc->type == BC_DEVICE_RTP) {
		ret = rtp_device_read(&bc->rtp);
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

static inline char *split(char *s)
{
	for (;*s && *s != '|'; s++);
	if (*s == '|')
		*s++ = '\0';
	return *s ? s : NULL;
}

static inline void split_pp(char *s, const char **p1, const char **p2)
{
	char *tmp = split(s);
	*p1 = tmp;
	*p2 = split(tmp);
}

static int rtsp_handle_init(struct bc_handle *bc, BC_DB_RES dbres)
{
	const char *username, *password, *val;
	char url[1024];
	unsigned int r;

	bc->type = BC_DEVICE_RTP;

	username = bc_db_get_val(dbres, "rtsp_username", NULL);
	password = bc_db_get_val(dbres, "rtsp_password", NULL);
	val = bc_db_get_val(dbres, "device", NULL);
	if (!username || !password || !val)
		return -1;

	/* Device is in the questionable format of 'hostname|port|path' */
	char *device = strdupa(val);
	const char *port, *path;

	split_pp(device, &port, &path);

	if (!path)
		return -1;

	if (!port)
		port = "554";

	/* Create a URL */
	if (*username || *password)
		r = snprintf(url, sizeof(url), "rtsp://%s:%s@%s:%s%s", username,
		             password, device, port, path);
	else
		r = snprintf(url, sizeof(url), "rtsp://%s:%s%s", device, port, path);

	if (r >= sizeof(url))
		return -1;

	bc->input = new rtp_device(url);

	val = bc_db_get_val(dbres, "mjpeg_path", NULL);
	if (val && *val) {
		bc->mjpeg_url[0] = '\0';

		char *device = strdupa(val);
		const char *port, *path;

		split_pp(device, &port, &path);

		if (!path) {
			if (!port)
				path = device;
			else
				return -1;
		}

		if (!port)
			port = "80";

		if (*username || *password)
			r = snprintf(bc->mjpeg_url, sizeof(bc->mjpeg_url), "http://%s:%s@%s:%s%s",
			             username, password, device, port, ((*path) ? path : "/"));
		else
			r = snprintf(bc->mjpeg_url, sizeof(bc->mjpeg_url), "http://%s:%s%s",
			             device, port, ((*path) ? path : "/"));
		if (r >= sizeof(bc->mjpeg_url))
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

	if (!strncmp(driver, "RTSP-", 5))
		ret = rtsp_handle_init(bc, dbres);
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

	if (!dev || !name || !driver || !schedule || !motion_map)
		return -1;

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
	cfg->prerecord = bc_db_get_val_int(dbres, "buffer_prerecording");
	cfg->postrecord = bc_db_get_val_int(dbres, "buffer_postrecording");
	cfg->debug_level = bc_db_get_val_int(dbres, "debug_level");
	cfg->aud_disabled = bc_db_get_val_int(dbres, "audio_disabled") != 0;
	cfg->schedule_override_global = bc_db_get_val_int(dbres, "schedule_override_global") != 0;
	cfg->hue        = bc_db_get_val_int(dbres, "hue");
	cfg->contrast   = bc_db_get_val_int(dbres, "contrast");
	cfg->brightness = bc_db_get_val_int(dbres, "brightness");
	cfg->saturation = bc_db_get_val_int(dbres, "saturation");

	return 0;
}

