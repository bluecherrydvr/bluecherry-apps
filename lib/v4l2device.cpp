#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "v4l2device.h"

v4l2_device::v4l2_device(BC_DB_RES dbres)
	: dev_fd(-1), cam_caps(0), codec_id(CODEC_ID_NONE), local_bufs(0), buf_idx(0), gop(0), buffers(0), dev_id(0), got_vop(0)
{
	memset(&p_buf, 0, sizeof(p_buf));

	const char *p = bc_db_get_val(dbres, "device", NULL);
	char dev_file[PATH_MAX];
	int id = -1;
	const char *signal_type = bc_db_get_val(dbres, "signal_type", NULL);

	card_id = bc_db_get_val_int(dbres, "card_id");

	while (p[0] != '\0' && p[0] != '|')
		p++;
	if (p[0] == '\0')
		goto error;
	p++;
	while (p[0] != '\0' && p[0] != '|')
		p++;
	if (p[0] == '\0' || p[1] == '\0')
		goto error;
	id = atoi(p + 1);

	dev_id = id;

	sprintf(dev_file, "/dev/video%d", card_id + id + 1);

	/* Open the device */
	if ((dev_fd = open(dev_file, O_RDWR)) < 0)
		goto error;

	/* Query the capabilites and verify them */
	memset(&vcap, 0, sizeof(vcap));
	if (ioctl(dev_fd, VIDIOC_QUERYCAP, &vcap) < 0)
		goto error;

	if (!(vcap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
	    !(vcap.capabilities & V4L2_CAP_STREAMING)) {
		goto error;
	}

	if (signal_type && strcasecmp(signal_type, "PAL") == 0)
		cam_caps |= BC_CAM_CAP_V4L2_PAL;

	/* SOLO */
	if (!strncmp(bc_db_get_val(dbres, "driver", NULL), "solo6", 5)) {
		cam_caps |= BC_CAM_CAP_V4L2_MOTION | BC_CAM_CAP_OSD | BC_CAM_CAP_SOLO;

		if (strstr((char*)vcap.card, "6010"))
			codec_id = CODEC_ID_MPEG4;
		else if (strstr((char*)vcap.card, "6110"))
			codec_id = CODEC_ID_H264;
	}

	/* Get the parameters */
	memset(&vparm, 0, sizeof(vparm));
	vparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(dev_fd, VIDIOC_G_PARM, &vparm) < 0)
		goto error;

	/* Get the format */
	memset(&vfmt, 0, sizeof(vfmt));
	vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(dev_fd, VIDIOC_G_FMT, &vfmt) < 0)
		goto error;

	return;
error:
	if (dev_fd >= 0) {
		close(dev_fd);
		dev_fd = -1;
	}
}

v4l2_device::~v4l2_device()
{
	if (dev_fd >= 0) {
		close(dev_fd);
		dev_fd = -1;
	}
}

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

v4l2_buffer *v4l2_device::buf_v4l2()
{
	if (buf_idx < 0)
		return NULL;

	return &p_buf[buf_idx].vb;
}

void v4l2_device::v4l2_local_bufs()
{
	int i, c;

	for (i = c = 0; i < buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(dev_fd, VIDIOC_QUERYBUF, &vb) < 0)
			continue;
		if (vb.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE))
			continue;

		c++;
	}

	local_bufs = c;
}

int v4l2_device::is_key_frame()
{
	struct v4l2_buffer *vb = buf_v4l2();

	/* For everything other than mpeg, every frame is a keyframe */
	if (vfmt.fmt.pix.pixelformat != V4L2_PIX_FMT_MPEG)
		return 1;

	if (vb && vb->flags & V4L2_BUF_FLAG_KEYFRAME)
		return 1;

	return 0;
}

int v4l2_device::v4l2_bufs_prepare()
{
	struct v4l2_requestbuffers req;
	int i;

	reset_vbuf(&req);
	req.count = buffers;

	if (ioctl(dev_fd, VIDIOC_REQBUFS, &req) < 0) {
		set_error_message("REQBUFS failed");
		return -1;
	}

	if (req.count != buffers) {
		set_error_message("REQBUFS returned wrong buffer count");
		return -1;
	}

	for (i = 0; i < buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(dev_fd, VIDIOC_QUERYBUF, &vb) < 0) {
			set_error_message("QUERYBUF failed");
			return -1;
		}

		p_buf[i].size = vb.length;
		p_buf[i].data = mmap(NULL, vb.length,
				PROT_WRITE | PROT_READ, MAP_SHARED,
				dev_fd, vb.m.offset);
		if (p_buf[i].data == MAP_FAILED) {
			set_error_message("mmap failed");
			return -1;
		}
	}

	return 0;
}

int v4l2_device::start()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i;

	/* For mpeg, we get the max, and for mjpeg the min */
	if (vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MPEG)
		buffers = BC_BUFFERS;
	else
		buffers = BC_BUFFERS_JPEG;

	if (v4l2_bufs_prepare())
		return -1;

	if (ioctl(dev_fd, VIDIOC_STREAMON, &type) < 0) {
		set_error_message("STREAMON failed");
		return -1;
	}

	/* Queue all buffers */
	for (i = 0; i < buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(dev_fd, VIDIOC_QUERYBUF, &vb) < 0) {
			set_error_message("QUERYBUF failed");
			return -1;
		}

		if (vb.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE))
			continue;

		if (ioctl(dev_fd, VIDIOC_QBUF, &vb) < 0) {
			set_error_message("QBUF failed");
			return -1;
		}
	}

	local_bufs = 0;
	buf_idx = -1;

	return 0;
}

void v4l2_device::stop()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i;

	if (dev_fd < 0)
		return;

	for (i = 0; i < buffers; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(dev_fd, VIDIOC_QUERYBUF, &vb) < 0)
			continue;

		if (!(vb.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE)))
			continue;

		if (ioctl(dev_fd, VIDIOC_DQBUF, &vb) <0)
			continue;
	}

	/* Stop the stream */
	ioctl(dev_fd, VIDIOC_STREAMOFF, &type);

	/* Unmap all buffers */
	for (i = 0; i < buffers; i++)
		munmap(p_buf[i].data, p_buf[i].size);

	local_bufs = buffers;
	buf_idx = -1;
}

void v4l2_device::reset()
{
	// XXX
	stop();
}

void v4l2_device::buf_return()
{
	int local = (buffers / 2) - 1;
	int thresh = ((buffers - local) / 2) + local;
	int i;

	/* Maintain a balance of queued and dequeued buffers */
	if (local_bufs < thresh)
		return;

	v4l2_local_bufs();

	for (i = 0; i < buffers && local_bufs > local; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(dev_fd, VIDIOC_QBUF, &vb) < 0)
			continue;

		local_bufs--;
	}

	if (local_bufs == buffers)
		bc_log("E: Unable to queue any buffers!");
}

int v4l2_device::read_packet()
{
	struct v4l2_buffer vb;
	buf_return();
	reset_vbuf(&vb);

	int ret = ioctl(dev_fd, VIDIOC_DQBUF, &vb);
	local_bufs++;
	/* XXX better error handling here */
	if (ret)
		return EAGAIN;

	/* Update and store this buffer */
	buf_idx = vb.index;
	p_buf[buf_idx].vb = vb;

	uint8_t *dbuf = new uint8_t[p_buf[buf_idx].vb.bytesused];
	memcpy(dbuf, p_buf[buf_idx].data, p_buf[buf_idx].vb.bytesused);

	current_packet = stream_packet(dbuf);
	current_packet.size     = p_buf[buf_idx].vb.bytesused;
	current_packet.ts_clock = time(NULL);
	current_packet.type     = AVMEDIA_TYPE_VIDEO;
	current_packet.pts      = AV_NOPTS_VALUE;
	current_packet.flags    = is_key_frame() ? AV_PKT_FLAG_KEY : 0;

	if (!got_vop) {
		if (!is_key_frame())
			return EAGAIN;
		got_vop = 1;
	}

	return 0;
}

int v4l2_device::set_interval(uint8_t interval)
{
	struct v4l2_control vc;
	double den = vparm.parm.capture.timeperframe.denominator;
	double num = interval;

	if (!interval)
		return 0;

	if (vparm.parm.capture.timeperframe.numerator == interval)
		return 0;

	// XXX ??
#if 0
	if (bc->started) {
		errno = EAGAIN;
		return -1;
	}
#endif

	vparm.parm.capture.timeperframe.numerator = interval;
	if (ioctl(dev_fd, VIDIOC_S_PARM, &vparm) < 0)
		return -1;
	ioctl(dev_fd, VIDIOC_G_PARM, &vparm);

	/* Reset GOP */
	gop = lround(den / num);
	if (!gop)
		gop = 1;
	vc.id = V4L2_CID_MPEG_VIDEO_GOP_SIZE;
	vc.value = gop;
	if (ioctl(dev_fd, VIDIOC_S_CTRL, &vc) < 0)
		return -1;

	return 0;
}

int v4l2_device::set_mjpeg()
{
	vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	if (ioctl(dev_fd, VIDIOC_S_FMT, &vfmt) < 0)
		return -1;

	return 0;
}

/* Check range and convert our 0-100 to valid ranges in the hardware */
int v4l2_device::set_control(unsigned int ctrl, int val)
{
	struct v4l2_queryctrl qctrl;
	struct v4l2_control vc;
	float step;

	memset(&qctrl, 0, sizeof(qctrl));
	qctrl.id = ctrl;
	if (ioctl(dev_fd, VIDIOC_QUERYCTRL, &qctrl) < 0)
		return -1;

	step = (float)(qctrl.maximum - qctrl.minimum) / (float)101;
	val = roundf(((float)val * step) + qctrl.minimum);

	vc.id = ctrl;
	vc.value = val;

	return ioctl(dev_fd, VIDIOC_S_CTRL, &vc);
}

int v4l2_device::set_format(uint32_t fmt, uint16_t width, uint16_t height)
{
	if (vfmt.fmt.pix.pixelformat == fmt &&
	    vfmt.fmt.pix.width == width &&
	    vfmt.fmt.pix.height == height)
		return 0;

	// XXX
#if 0
	if (bc->started) {
		errno = EAGAIN;
		return -1;
	}
#endif

	vfmt.fmt.pix.pixelformat = fmt;
	vfmt.fmt.pix.width = width;
	vfmt.fmt.pix.height = height;

	if (ioctl(dev_fd, VIDIOC_S_FMT, &vfmt) < 0)
		return -1;
	ioctl(dev_fd, VIDIOC_G_FMT, &vfmt);

	return 0;
}

int v4l2_device::set_osd(char *fmt, ...)
{
	char buf[256];
	va_list ap;
	struct v4l2_ext_control ctrl;
	struct v4l2_ext_controls ctrls;

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

	if (ioctl(dev_fd, VIDIOC_S_EXT_CTRLS, &ctrls) < 0)
		return -1;

	return 0;
}

int v4l2_device::setup_output(AVFormatContext *oc)
{
	AVStream *st;
	int fden   = vparm.parm.capture.timeperframe.denominator;
	int fnum   = vparm.parm.capture.timeperframe.numerator;
	int width  = vfmt.fmt.pix.width;
	int height = vfmt.fmt.pix.height;

	/* Setup new video stream */
	if ((st = avformat_new_stream(oc, NULL)) == NULL)
		return -1;

	st->time_base.den = fden;
	st->time_base.num = fnum;

	if (codec_id == CODEC_ID_NONE) {
		bc_log("Invalid Video Format, assuming MP4V-ES");
		st->codec->codec_id = CODEC_ID_MPEG4;
	} else
		st->codec->codec_id = codec_id;

	/* h264 requires us to work around libavcodec broken defaults */
	if (st->codec->codec_id == CODEC_ID_H264) {
		st->codec->me_range = 16;
		st->codec->me_subpel_quality = 7;
		st->codec->qmin = 10;
		st->codec->qmax = 51;
		st->codec->max_qdiff = 4;
		st->codec->qcompress = 0.6;
		st->codec->i_quant_factor = 0.71;
		st->codec->b_frame_strategy = 1;
	}

	st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
	st->codec->pix_fmt = PIX_FMT_YUV420P;
	st->codec->width = width;
	st->codec->height = height;
	st->codec->time_base.num = fnum;
	st->codec->time_base.den = fden;

	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	st = NULL;

#if 0
	/* We don't fail when this happens. Video with no sound is
	 * better than no video at all. */
	if (audio_enabled() && bc_alsa_open(bc_rec))
		bc_alsa_close(bc_rec);

	/* Setup new audio stream */
	if (has_audio(bc_rec)) {
		enum CodecID codec_id = CODEC_ID_PCM_S16LE;

		/* If we can't find an encoder, just skip it */
		if (avcodec_find_encoder(codec_id) == NULL) {
			bc_dev_warn(bc_rec, "Failed to find audio codec (%08x) "
				    "so not recording", codec_id);
			goto no_audio;
		}

		if ((st = avformat_new_stream(oc, NULL)) == NULL)
			goto no_audio;
		st->codec->codec_id = codec_id;
		st->codec->codec_type = AVMEDIA_TYPE_AUDIO;

		st->codec->sample_rate = 8000;
		st->codec->sample_fmt = SAMPLE_FMT_S16;
		st->codec->channels = 1;
		st->codec->time_base = (AVRational){1, 8000};

		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
			st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
no_audio:
		st = NULL;
	}
#endif
	
	return 0;
}


