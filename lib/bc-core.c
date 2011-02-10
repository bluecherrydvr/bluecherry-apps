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

#include <libbluecherry.h>

#define reset_vbuf(__vb) do {				\
	memset((__vb), 0, sizeof(*(__vb)));		\
	(__vb)->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	\
	(__vb)->memory = V4L2_MEMORY_MMAP;		\
} while(0)

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

static struct v4l2_buffer *bc_buf_v4l2(struct bc_handle *bc)
{
	if (bc->buf_idx < 0)
		return NULL;

	return &bc->p_buf[bc->buf_idx].vb;
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
		return mpeg4_is_key_frame(bc->rtp_sess.vid_buf,
					  bc->rtp_sess.vid_len);

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

static int bc_v4l2_bufs_prepare(struct bc_handle *bc)
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

static int v4l2_handle_start(struct bc_handle *bc)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i;

	/* For mpeg, we get the max, and for mjpeg the min */
	if (bc->vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MPEG)
		bc->buffers = BC_BUFFERS;
	else
		bc->buffers = BC_BUFFERS_JPEG;

	if (bc_v4l2_bufs_prepare(bc))
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

	bc->local_bufs = 0;
	bc->buf_idx = -1;

	return 0;
}

int bc_handle_start(struct bc_handle *bc)
{
	int ret = -1;

	if (bc->started)
		return 0;

	if (bc->cam_caps & BC_CAM_CAP_RTSP)
		ret = rtp_session_start(&bc->rtp_sess);
	else if (bc->cam_caps & BC_CAM_CAP_V4L2)
		ret = v4l2_handle_start(bc);

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
		struct rtp_session *rs = &bc->rtp_sess;
		int ret = 0;

		/* Reset for next packet */
		if (rs->vid_valid)
			rs->vid_valid = rs->vid_len = 0;
		if (rs->aud_valid)
			rs->aud_valid = rs->aud_len = 0;

		/* Loop till we get a whole packet */
		for (;;) {
			ret = rtp_session_read(rs);
			if (ret)
				return ret;

			if (!bc->got_vop) {
				if (!rs->vid_valid)
					continue;
				else
					bc->got_vop = 1;
			}

			if (rs->vid_valid || rs->aud_valid)
				break;
		}

		return 0;
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

	/* If no motion detection, then carry on normally */
	if (!(bc_buf_v4l2(bc)->flags & V4L2_BUF_FLAG_MOTION_ON)) {
		/* Reset this counter in case motion gets turned back on */
		bc->mot_cnt = 0;

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

/* Internal rtp session init that uses a database result */
static int rtp_session_init_dbres(struct rtp_session *rs,
				  void *dbres)
{
	const char *val;
	char *device;
	char *p, *t;

	memset(rs, 0, sizeof(*rs));

	val = bc_db_get_val(dbres, "rtsp_username");
	if (!val)
		return -1;
	strcpy(rs->userinfo, val);

	strcat(rs->userinfo, ":");

	val = bc_db_get_val(dbres, "rtsp_password");
	if (!val)
		return -1;
	strcat(rs->userinfo, val);

	val = bc_db_get_val(dbres, "device");
	if (!val)
		return -1;

	device = strdupa(val);
	if (!device)
		return -1;

	p = t = device;
	while (*t != '|' && *t != '\0')
		t++;
	if (*t == '\0')
		return -1;

	*(t++) = '\0';

	strcpy(rs->server, p);

	p = t;
	while (*t != '|' && *t != '\0')
		t++;
	if (*t == '\0')
		return -1;

	*(t++) = '\0';

	rs->port = atoi(p);
	strcpy(rs->uri, t);

	return 0;
}

static int rtsp_handle_init(struct bc_handle *bc, BC_DB_RES dbres)
{
	const char *val;

	bc->cam_caps |= BC_CAM_CAP_RTSP;
	if (rtp_session_init_dbres(&bc->rtp_sess, dbres)) {
		errno = ENOMEM;
		return -1;
	}

	val = bc_db_get_val(dbres, "mjpeg_path");
	if (val) {
		snprintf(bc->mjpeg_url, sizeof(bc->mjpeg_url),
			 "http://%s@%s%s", bc->rtp_sess.userinfo,
			 bc->rtp_sess.server, val);
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

	device = bc_db_get_val(dbres, "device");
	driver = bc_db_get_val(dbres, "driver");
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
