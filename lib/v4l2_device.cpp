#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "v4l2_device.h"

extern "C" {
#include <libavutil/mathematics.h>
}

/**
 * Select the best between two formats.
 */
static uint32_t best_pixfmt(uint32_t old, uint32_t cur)
{
	const uint32_t pref[] = {
		V4L2_PIX_FMT_H264,
		V4L2_PIX_FMT_MPEG4,
		0
	};

	for (int i = 0; pref[i]; i++) {
		if (pref[i] == old)
			return old;
		if (pref[i] == cur)
			return cur;
	}
	return 0;
}

/**
 * Get the best of the supported pixel formats the card provides.
 */
static uint32_t get_best_pixfmt(int fd)
{
	struct v4l2_fmtdesc fmt;
	uint32_t sel = 0;

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	for (;ioctl(fd, VIDIOC_ENUM_FMT, &fmt) == 0; fmt.index++)
		sel = best_pixfmt(sel, fmt.pixelformat);

	return sel;
}

v4l2_device::v4l2_device(BC_DB_RES dbres)
	: dev_fd(-1), cam_caps(0), codec_id(AV_CODEC_ID_NONE), local_bufs(0), buf_idx(0), gop(0),
	  buffers(0), dev_id(0), got_vop(0)
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

	/* SOLO, XXX: there should be some way to detect these caps thru V4L2 */
	if (!strncmp(bc_db_get_val(dbres, "driver", NULL), "solo6", 5)) {
		cam_caps |= BC_CAM_CAP_V4L2_MOTION | BC_CAM_CAP_OSD
			| BC_CAM_CAP_SOLO;
	}

	/* Select the CODEC */
	uint32_t fmt;
	fmt = get_best_pixfmt(dev_fd);
	switch (fmt) {
	case V4L2_PIX_FMT_MPEG4:
		codec_id = AV_CODEC_ID_MPEG4;
		break;
	case V4L2_PIX_FMT_H264:
		codec_id = AV_CODEC_ID_H264;
		break;
	default:
		bc_log(Error, "Unknown '%c%c%c%c' pixel format",
			fmt, fmt >> 8, fmt >> 16, fmt >> 24);
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
	unsigned int c;

	for (unsigned int i = c = 0; i < buffers; i++) {
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
	switch (vfmt.fmt.pix.pixelformat) {
	case V4L2_PIX_FMT_H264:
	case V4L2_PIX_FMT_MPEG4:
		break;
	default:
		return 1;
	}

	if (vb && vb->flags & V4L2_BUF_FLAG_KEYFRAME)
		return 1;

	return 0;
}

int v4l2_device::v4l2_bufs_prepare()
{
	struct v4l2_requestbuffers req;

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

	for (unsigned int i = 0; i < buffers; i++) {
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

	/* For mpeg, we get the max, and for mjpeg the min */
	switch (vfmt.fmt.pix.pixelformat) {
	case V4L2_PIX_FMT_H264:
	case V4L2_PIX_FMT_MPEG4:
		buffers = BC_BUFFERS;
		break;
	default:
		buffers = BC_BUFFERS_JPEG;
		break;
	}

	if (v4l2_bufs_prepare())
		return -1;

	if (ioctl(dev_fd, VIDIOC_STREAMON, &type) < 0) {
		set_error_message("STREAMON failed");
		return -1;
	}

	/* Queue all buffers */
	for (unsigned int i = 0; i < buffers; i++) {
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

	_started = true;
	update_properties();
	return 0;
}

void v4l2_device::stop()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (dev_fd < 0)
		return;

	for (unsigned int i = 0; i < buffers; i++) {
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
	for (unsigned int i = 0; i < buffers; i++)
		munmap(p_buf[i].data, p_buf[i].size);

	local_bufs = buffers;
	buf_idx = -1;

	current_properties.reset();
	_started = false;
}

void v4l2_device::buf_return()
{
	unsigned int local = (buffers / 2) - 1;
	unsigned int thresh = ((buffers - local) / 2) + local;

	/* Maintain a balance of queued and dequeued buffers */
	if (local_bufs < thresh)
		return;

	v4l2_local_bufs();

	for (unsigned int i = 0; i < buffers && local_bufs > local; i++) {
		struct v4l2_buffer vb;

		reset_vbuf(&vb);
		vb.index = i;

		if (ioctl(dev_fd, VIDIOC_QBUF, &vb) < 0)
			continue;

		local_bufs--;
	}

	if (local_bufs == buffers)
		bc_log(Error, "V4L2 device cannot queue any buffers");
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

	if (!got_vop) {
		if (!is_key_frame())
			return EAGAIN;
		got_vop = 1;
	}

	uint8_t *dbuf = new uint8_t[p_buf[buf_idx].vb.bytesused];
	memcpy(dbuf, p_buf[buf_idx].data, p_buf[buf_idx].vb.bytesused);

	current_packet = stream_packet(dbuf, current_properties);
	current_packet.seq      = next_packet_seq++;
	current_packet.size     = p_buf[buf_idx].vb.bytesused;
	current_packet.ts_clock = time(NULL);
	current_packet.type     = AVMEDIA_TYPE_VIDEO;
	current_packet.ts_monotonic = bc_gettime_monotonic();

	if (is_key_frame())
		current_packet.flags |= stream_packet::KeyframeFlag;
	if (vb.flags & V4L2_BUF_FLAG_MOTION_DETECTED)
		current_packet.flags |= stream_packet::MotionFlag;

	/* PTS calculated assuming that no packet is lost in driver and all frames have perfect timing.
	 * Real timestamps from driver would be ideal. Hopefully OK during timeperframe changes. */
	AVRational tb = { vparm.parm.capture.timeperframe.numerator, vparm.parm.capture.timeperframe.denominator };
	current_packet.pts = av_rescale_q(current_packet.seq, tb, AV_TIME_BASE_Q);

	return 0;
}

int v4l2_device::set_resolution(uint16_t width, uint16_t height,
		uint8_t interval)
{
	int re = 0;
	bool fmt_changed = (width && vfmt.fmt.pix.width != width)
		|| (height && vfmt.fmt.pix.height != height);
	bool int_changed = interval
		&& vparm.parm.capture.timeperframe.numerator != interval;

	if (!fmt_changed && !int_changed)
		return re;

	bool needs_restart = is_started();
	if (needs_restart)
		stop();

	if (!re && int_changed) {
		vparm.parm.capture.timeperframe.numerator = interval;
		if (ioctl(dev_fd, VIDIOC_S_PARM, &vparm) < 0)
			re = -1;
		else if (ioctl(dev_fd, VIDIOC_G_PARM, &vparm) < 0)
			re = -1;
		else {
			/* Reset GOP */
			gop = lround(vparm.parm.capture.timeperframe.denominator / vparm.parm.capture.timeperframe.numerator);
			if (!gop)
				gop = 1;
			struct v4l2_control vc;
			vc.id = V4L2_CID_MPEG_VIDEO_GOP_SIZE;
			vc.value = gop;
			if (ioctl(dev_fd, VIDIOC_S_CTRL, &vc) < 0)
				re = -1;
		}
	}

	// Format must be set last, because it may implicitly turn on the encoder,
	// which could cause other changes to fail...
	if (fmt_changed) {
		enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		uint32_t fmt;
		switch (codec_id) {
		case AV_CODEC_ID_MPEG4:
			fmt = V4L2_PIX_FMT_MPEG4;
			break;
		case AV_CODEC_ID_H264:
			fmt = V4L2_PIX_FMT_H264;
			break;
		default:
			this->set_error_message("unsupported codec_id");
			return -1;
		}

		vfmt.fmt.pix.pixelformat = fmt;
		if (width)
			vfmt.fmt.pix.width = width;
		if (height)
			vfmt.fmt.pix.height = height;

		if (ioctl(dev_fd, VIDIOC_S_FMT, &vfmt) < 0)
			re = -1;
		else if (ioctl(dev_fd, VIDIOC_G_FMT, &vfmt) < 0)
			re = -1;

		// S_FMT may turn the stream on implicitly, so make sure it's off if we're not
		// about to turn it on again.
		else if (!needs_restart)
			ioctl(dev_fd, VIDIOC_STREAMOFF, &type);
	}

	// start() does update_properties()
	if (needs_restart)
		start();
	return re;
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

#if 0
void *v4l2_device::buf_data()
{
	if (buf_idx < 0)
		return NULL;

	return p_buf[buf_idx].data;
}

unsigned int v4l2_device::buf_size()
{
	if (buf_idx < 0)
		return 0;

	return p_buf[buf_idx].vb.bytesused;
}
#endif

int v4l2_device::set_osd(const char *fmt, ...)
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

void v4l2_device::update_properties()
{
	stream_properties *p = new stream_properties;

	p->video.codec_id = codec_id;
	p->video.pix_fmt = AV_PIX_FMT_YUV420P;
	p->video.width = vfmt.fmt.pix.width;
	p->video.height = vfmt.fmt.pix.height;
	p->video.time_base.num = vparm.parm.capture.timeperframe.numerator;
	p->video.time_base.den = vparm.parm.capture.timeperframe.denominator;

	if (p->video.codec_id == AV_CODEC_ID_NONE) {
		bc_log(Warning, "Invalid Video Format, assuming H.264");
		p->video.codec_id = AV_CODEC_ID_H264;
	}

#if 0
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	st = NULL;

	/* We don't fail when this happens. Video with no sound is
	 * better than no video at all. */
	if (audio_enabled() && bc_alsa_open(bc_rec))
		bc_alsa_close(bc_rec);

	/* Setup new audio stream */
	if (has_audio(bc_rec)) {
		enum AVCodecID codec_id = AV_CODEC_ID_PCM_S16LE;

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
		st->codec->sample_fmt = AV_SAMPLE_FMT_S16;
		st->codec->channels = 1;
		st->codec->time_base = (AVRational){1, 8000};

		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
			st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
no_audio:
		st = NULL;
	}
#endif

	current_properties = std::shared_ptr<stream_properties>(p);
}

static uint16_t solo_value_map[] = {
	0xffff, 1152, 1024, 768, 512, 3842
};

int v4l2_device::set_motion(bool on)
{
	if (!(caps() & BC_CAM_CAP_V4L2_MOTION)) {
		bc_log(Error, "Motion detection is not implemented for non-solo V4L2 devices.");
		return -ENOSYS;
	}

	struct v4l2_control vc;
	vc.id = V4L2_CID_MOTION_ENABLE;
	vc.value = on ? 1 : 0;
	return ioctl(dev_fd, VIDIOC_S_CTRL, &vc);
}

int v4l2_device::set_motion_thresh_global(char value)
{
	int val = clamp(value, '0', '5') - '0';
	if (caps() & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;
		vc.id = V4L2_CID_MOTION_THRESHOLD;
		/* Upper 16 bits are 0 for the global threshold */
		vc.value = solo_value_map[val];
		return ioctl(dev_fd, VIDIOC_S_CTRL, &vc);
	}

	return 0;
}


int v4l2_device::set_motion_thresh(const char *map, size_t size)
{
	if (!(caps() & BC_CAM_CAP_V4L2_MOTION))
		return -ENOSYS;

	struct v4l2_control vc;
	unsigned int vh = 15;
	vc.id = V4L2_CID_MOTION_THRESHOLD;

	if (caps() & BC_CAM_CAP_V4L2_PAL)
		vh = 18;

	if (size < 22 * vh) {
		bc_log(Debug, "Received motion threshold map of wrong size");
		return -1;
	}

	for (unsigned int y = 0, pos = 0; y < vh; y++) {
		for (unsigned int x = 0; x < 22; x++, pos++) {
			int val = clamp(map[pos], '0', '5') - '0';

			vc.value = solo_value_map[val];

			/* One more than the actual block number, because
			 * 0 sets the global threshold. */
			vc.value |= (y << 23) + (x << 17) + (1 << 16);

			/* Set motion threshold on a 2x2 sector. Our input map
			 * has half the resolution the devices work with, in
			 * both directions. */
			if (ioctl(dev_fd, VIDIOC_S_CTRL, &vc))
				goto error;

			vc.value += 1 << 16;
			if (ioctl(dev_fd, VIDIOC_S_CTRL, &vc))
			        goto error;

			vc.value += 1 << 23;
			if (ioctl(dev_fd, VIDIOC_S_CTRL, &vc))
			        goto error;

			vc.value -= 1 << 16;
			if (ioctl(dev_fd, VIDIOC_S_CTRL, &vc))
				goto error;
		}
	}

	return 0;

error:
	bc_log(Error, "Error setting motion threshold: %s", strerror(errno));
	return -1;
}
