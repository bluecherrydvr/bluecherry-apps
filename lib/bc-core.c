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

#include <libbluecherry.h>

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

static struct v4l2_buffer *bc_buf_v4l2(struct bc_handle *bc)
{
	if (bc->buf_idx < 0)
		return NULL;

	return &bc->p_buf[bc->buf_idx].vb;
}

int bc_motion_is_on(struct bc_handle *bc)
{
	struct v4l2_buffer *vb;

	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

	vb = bc_buf_v4l2(bc);
	if (vb == NULL)
		return 0;

	return vb->flags & V4L2_BUF_FLAG_MOTION_ON ? 1 : 0;
}

int bc_motion_is_detected(struct bc_handle *bc)
{
	struct v4l2_buffer *vb;

	if (!bc_motion_is_on(bc))
		return 0;

	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

	vb = bc_buf_v4l2(bc);
	if (vb == NULL)
		return 0;

	return vb->flags & V4L2_BUF_FLAG_MOTION_DETECTED ? 1 : 0;
}

static inline void bc_v4l2_local_bufs(struct bc_handle *bc)
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

	bc->local_bufs = c;
}

static int mpeg4_is_key_frame(const unsigned char *data,
			      int len)
{
	int key = 0;
	int i;

	for (i = 0; i < len; i++) {
		if (len - i < 5)
			return 0;

		if (data[i] != 0x00)
			continue;
		if (data[i+1] != 0x00)
			continue;
		if (data[i+2] != 0x01)
			continue;
		if (data[i+3] != 0xb6)
			continue;

		if (!(data[i+4] & 0xc0))
			key = 1;
		break;
	}

	return key;
}

int bc_buf_key_frame(struct bc_handle *bc)
{
	struct v4l2_buffer *vb;

	if (bc->cam_caps & BC_CAM_CAP_RTSP)
		return rtp_session_frame_is_keyframe(&bc->rtp_sess);

	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 1;

	vb = bc_buf_v4l2(bc);

	/* For everything other than mpeg, every frame is a keyframe */
	if (bc->vfmt.fmt.pix.pixelformat != V4L2_PIX_FMT_MPEG)
		return 1;

	if (vb && vb->flags & V4L2_BUF_FLAG_KEYFRAME)
		return 1;

	return 0;
}

int bc_buf_is_video_frame(struct bc_handle *bc)
{
	if (bc->cam_caps & BC_CAM_CAP_RTSP)
		return bc->rtp_sess.frame.stream_index == bc->rtp_sess.video_stream_index;
	return 1;
}

static int bc_v4l2_bufs_prepare(struct bc_handle *bc, const char **err_msg)
{
	struct v4l2_requestbuffers req;
	int i;

	reset_vbuf(&req);
	req.count = bc->buffers;

	if (ioctl(bc->dev_fd, VIDIOC_REQBUFS, &req) < 0)
		RETERR("REQBUFS Failed");

	if (req.count != bc->buffers)
		RETERR("REQBUFS Returned wrong buffer count");

	for (i = 0; i < bc->buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QUERYBUF, &vb) < 0)
			RETERR("QUERYBUF Failed");

		bc->p_buf[i].size = vb.length;
		bc->p_buf[i].data = mmap(NULL, vb.length,
					 PROT_WRITE | PROT_READ, MAP_SHARED,
					 bc->dev_fd, vb.m.offset);
		if (bc->p_buf[i].data == MAP_FAILED)
			RETERR("MMAP Failed");
	}

	return 0;
}

static int v4l2_handle_start(struct bc_handle *bc, const char **err_msg)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i;

	/* For mpeg, we get the max, and for mjpeg the min */
	if (bc->vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MPEG)
		bc->buffers = BC_BUFFERS;
	else
		bc->buffers = BC_BUFFERS_JPEG;

	if (bc_v4l2_bufs_prepare(bc, err_msg))
		return -1;

	if (ioctl(bc->dev_fd, VIDIOC_STREAMON, &type) < 0)
		RETERR("STREAMON Failed");

	/* Queue all buffers */
	for (i = 0; i < bc->buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QUERYBUF, &vb) < 0)
			RETERR("QUERYBUF Failed");

		if (vb.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE))
			continue;

		if (ioctl(bc->dev_fd, VIDIOC_QBUF, &vb) < 0)
			RETERR("QBUF Failed");
	}

	bc->local_bufs = 0;
	bc->buf_idx = -1;

	return 0;
}

int bc_handle_start(struct bc_handle *bc, const char **err_msg)
{
	int ret = -1;

	if (err_msg)
		*err_msg = "No error (or unknown error)";

	if (bc->started)
		return 0;

	if (bc->cam_caps & BC_CAM_CAP_RTSP) {
		ret = rtp_session_start(&bc->rtp_sess);
		if (ret < 0)
			*err_msg = bc->rtp_sess.error_message;
	} else if (bc->cam_caps & BC_CAM_CAP_V4L2)
		ret = v4l2_handle_start(bc, err_msg);

	if (!ret) {
		bc->started = 1;
		bc->got_vop = 0;
	}

	return ret;
}

static void bc_buf_return(struct bc_handle *bc)
{
	int local = (bc->buffers / 2) - 1;
	int thresh = ((bc->buffers - local) / 2) + local;
	int i;

	/* Maintain a balance of queued and dequeued buffers */
	if (bc->local_bufs < thresh)
		return;

	bc_v4l2_local_bufs(bc);

	for (i = 0; i < bc->buffers && bc->local_bufs > local; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QBUF, &vb) < 0)
			continue;

		bc->local_bufs--;
	}

	if (bc->local_bufs == bc->buffers)
		bc_log("E: Unable to queue any buffers!");
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

	if (bc->cam_caps & BC_CAM_CAP_RTSP) {
		return rtp_session_read(&bc->rtp_sess);
	}

	bc_buf_return(bc);

	reset_vbuf(&vb);

	ret = ioctl(bc->dev_fd, VIDIOC_DQBUF, &vb);
	bc->local_bufs++;
	if (ret)
		return EAGAIN;

	/* Update and store this buffer */
	bc->buf_idx = vb.index;
	bc->p_buf[bc->buf_idx].vb = vb;

	/* If no motion detection, then carry on normally. For MJPEG,
	 * we never stall the buffer for motion. */
	if (bc->vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MJPEG ||
	    !bc_motion_is_on(bc)) {
		/* Reset this timestamp in case motion gets turned back on */
		bc->mot_last_ts = 0;

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

	time_t monotonic_now = bc_gettime_monotonic();

	if (bc_motion_is_detected(bc)) {
		if (!bc->mot_last_ts) {
			bc->got_vop = 0;
			// First time, send event
			__bc_start_motion_event(bc);
		}
		/* Reset this timestamp every time we get a new event */
		bc->mot_last_ts = monotonic_now;
	} else if (!bc->mot_last_ts) {
		return ret;
	}

	if (!bc->got_vop) {
		if (!bc_buf_key_frame(bc))
			return EAGAIN;
		bc->got_vop = 1;
	}

	if (monotonic_now - bc->mot_last_ts >= 3) {
		/* End of event */
		bc->mot_last_ts = 0;
		ret = ERESTART;
		__bc_stop_motion_event(bc);
	} else
		ret = 0;

	return ret;
}

int bc_set_interval(struct bc_handle *bc, u_int8_t interval)
{
	struct v4l2_control vc;
	double den = bc->vparm.parm.capture.timeperframe.denominator;
	double num = interval;

	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

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

static int v4l2_handle_init(struct bc_handle *bc, BC_DB_RES dbres)
{
	const char *p = bc->device;
	char dev_file[PATH_MAX];
	int id = -1;

	bc->card_id = bc_db_get_val_int(dbres, "card_id");

	bc->cam_caps |= BC_CAM_CAP_V4L2;
	if (!strncmp(bc->driver, "solo6", 5))
		bc->cam_caps |= BC_CAM_CAP_OSD | BC_CAM_CAP_SOLO;

	while (p[0] != '\0' && p[0] != '|')
		p++;
	if (p[0] == '\0') {
		errno = EINVAL;
		return -1;
	}
	p++;
	while (p[0] != '\0' && p[0] != '|')
		p++;
	if (p[0] == '\0' || p[1] == '\0') {
		errno = EINVAL;
		return -1;
	}
	id = atoi(p + 1);

	bc->dev_id = id;

	sprintf(dev_file, "/dev/video%d", bc->card_id + id + 1);

	/* Open the device */
	if ((bc->dev_fd = open(dev_file, O_RDWR)) < 0)
		return -1;

	/* Query the capabilites and verify them */
	if (ioctl(bc->dev_fd, VIDIOC_QUERYCAP, &bc->vcap) < 0)
		return -1;

	if (!(bc->vcap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
	    !(bc->vcap.capabilities & V4L2_CAP_STREAMING)) {
		errno = EINVAL;
		return -1;
	}

	/* Get the parameters */
	bc->vparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(bc->dev_fd, VIDIOC_G_PARM, &bc->vparm) < 0)
		return -1;

	/* Get the format */
	bc->vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(bc->dev_fd, VIDIOC_G_FMT, &bc->vfmt) < 0)
		return -1;

	return 0;
}

static int rtsp_handle_init(struct bc_handle *bc, BC_DB_RES dbres)
{
	const char *username, *password, *val, *port = "554";
	char *device, *path, *t;
	char url[1024];
	int r;

	bc->cam_caps |= BC_CAM_CAP_RTSP;

	username = bc_db_get_val(dbres, "rtsp_username", NULL);
	password = bc_db_get_val(dbres, "rtsp_password", NULL);
	val = bc_db_get_val(dbres, "device", NULL);
	if (!username || !password || !val)
		return -1;

	/* Device is in the questionable format of 'hostname|port|path' */
	device = strdupa(val);

	path = 0;
	int n = 0;
	for (t = device; *t; t++) {
		if (*t == '|') {
			*t = '\0';
			if (n == 0) {
				port = t+1;
			} else if (n == 1) {
				path = ++t;
				break;
			}
			n++;
		}
	}

	if (*t == '\0')
		return -1;

	/* Create a URL */
	if (*username || *password)
		r = snprintf(url, sizeof(url), "rtsp://%s:%s@%s:%s%s", username,
		             password, device, port, path);
	else
		r = snprintf(url, sizeof(url), "rtsp://%s:%s%s", device, port, path);

	if (r >= sizeof(url))
		return -1;

	rtp_session_init(&bc->rtp_sess, url);

	val = bc_db_get_val(dbres, "mjpeg_path", NULL);
	if (val && *val) {
		char *mjpeg_val = strdupa(val), *v;
		bc->mjpeg_url[0] = '\0';

		port = "80";
		path = 0;
		int n = 0;
		for (t = v = mjpeg_val; *t; t++) {
			if (*t == '|') {
				*t = '\0';
				if (v != t && n == 0)
					device = v;
				else if (v != t && n == 1)
					port = v;
				v = t+1;
				if (++n == 2) {
					path = v;
					break;
				}
			}
		}

		if (v == mjpeg_val) {
			/* No pipes means only the path */
			path = mjpeg_val;
		}

		if (!path)
			return -1;

		if (*username || *password)
			r = snprintf(bc->mjpeg_url, sizeof(bc->mjpeg_url), "http://%s:%s@%s:%s%s",
			             username, password, device, port, ((*path) ? path : "/"));
		else
			r = snprintf(bc->mjpeg_url, sizeof(bc->mjpeg_url), "http://%s:%s%s",
			             device, port, ((*path) ? path : "/"));
		if (r >= sizeof(bc->mjpeg_url))
			return -1;

		bc->cam_caps |= BC_CAM_CAP_MJPEG_URL;
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

	if ((bc = malloc(sizeof(*bc))) == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	memset(bc, 0, sizeof(*bc));

	strcpy(bc->device, device);
	strcpy(bc->driver, driver);
	bc->dev_fd = -1;

	bc_ptz_check(bc, dbres);

	if (!strncmp(driver, "RTSP-", 5))
		ret = rtsp_handle_init(bc, dbres);
	else
		ret = v4l2_handle_init(bc, dbres);

	if (ret) {
		bc_handle_free(bc);
		return NULL;
	}

	return bc;
}

static void v4l2_handle_stop(struct bc_handle *bc)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i;

	if (bc->dev_fd < 0)
		return;

	for (i = 0; i < bc->buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(bc->dev_fd, VIDIOC_QUERYBUF, &vb) < 0)
			continue;

		if (!(vb.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE)))
			continue;

		if (ioctl(bc->dev_fd, VIDIOC_DQBUF, &vb) <0)
			continue;
	}

	/* Stop the stream */
	ioctl(bc->dev_fd, VIDIOC_STREAMOFF, &type);

	/* Unmap all buffers */
	for (i = 0; i < bc->buffers; i++)
		munmap(bc->p_buf[i].data, bc->p_buf[i].size);

	bc->local_bufs = bc->buffers;
	bc->buf_idx = -1;
}

void bc_handle_stop(struct bc_handle *bc)
{
	int save_err = errno;

	if (!bc || !bc->started)
		return;

	if (bc->cam_caps & BC_CAM_CAP_V4L2)
		v4l2_handle_stop(bc);
	else if (bc->cam_caps & BC_CAM_CAP_RTSP)
		rtp_session_stop(&bc->rtp_sess);

	bc->started = 0;
	errno = save_err;
}

void bc_handle_reset(struct bc_handle *bc)
{
	if (bc->cam_caps & BC_CAM_CAP_V4L2)
		bc_handle_stop(bc);
}

void bc_handle_free(struct bc_handle *bc)
{
	/* Don't want this call to change errno at all */
	int save_err = errno;

	if (!bc)
		return;

	bc_handle_stop(bc);

	if (bc->dev_fd >= 0)
		close(bc->dev_fd);

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
	cfg->aud_disabled = bc_db_get_val_int(dbres, "audio_disabled") != 0;
	cfg->schedule_override_global = bc_db_get_val_int(dbres, "schedule_override_global") != 0;

	return 0;
}

