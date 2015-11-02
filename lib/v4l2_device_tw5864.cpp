#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <bsd/string.h>

#include <linux/version.h>

#include <algorithm>

#include "v4l2_device_tw5864.h"

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

v4l2_device_tw5864::v4l2_device_tw5864(BC_DB_RES dbres)
	: dev_fd(-1), cam_caps(0), dev_id(0), demuxer(NULL)
{
	const char *p = bc_db_get_val(dbres, "device", NULL);
	int input_number;
	const char *needed_signal_type = bc_db_get_val(dbres, "signal_type", NULL);
	int ret;
	const char *syspath_begin;
	const char *syspath_end;
	char syspath[PATH_MAX] = "";
	char *cmd = NULL;
	FILE *shell;
	char line[64] = "";

	card_id = bc_db_get_val_int(dbres, "card_id");

	// TODO Support "/dev/video%d" pattern, and not just "BCPCI|0000:07:05.0|7", in "device" field
	while (p[0] != '\0' && p[0] != '|')
		p++;
	if (p[0] == '\0')
		goto error;
	p++;
	syspath_begin = p;
	while (p[0] != '\0' && p[0] != '|')
		p++;
	syspath_end = p;
	strlcpy(syspath, syspath_begin, std::min((int)sizeof(syspath), (int)(syspath_end - syspath_begin + 1)));
	
	if (p[0] == '\0' || p[1] == '\0')
		goto error;
	input_number = atoi(p + 1);

	ret = asprintf(&cmd, "ls %s/video4linux | sed 's/video//' | sort -n | sed 's/^/video/' | tail -n +%d | head -n 1 | tr -d '\\n'",
			syspath, input_number + 1);
	if (ret == -1)
		goto error;

	bc_log(Debug, "Executing '%s'", cmd);
	shell = popen(cmd, "r");
	if (!shell) {
		bc_log(Error, "Failed to execute cmd to get input dev node");
		ret = 1;
		goto error;
	}

	fgets(line, sizeof(line), shell);
	pclose(shell);

	sprintf(dev_file, "/dev/%s", line);

	/* Open the device */
	bc_log(Debug, "Opening %s", dev_file);
	if ((dev_fd = open(dev_file, O_RDWR)) < 0)
		goto error;

#if 0
	if (needed_signal_type) {
		v4l2_std_id current_std, needed_std;

		if (strcasecmp(needed_signal_type, "PAL") == 0)
			needed_std = V4L2_STD_PAL_B;
		else
			needed_std = V4L2_STD_NTSC_M;

		ret = ioctl(dev_fd, VIDIOC_G_STD, &current_std);
		if (ret) {
			bc_log(Error, "Querying %s current video standard failed", dev_file);
			goto error;
		}

		if (!(current_std & needed_std)) {
			ret = ioctl(dev_fd, VIDIOC_S_STD, &needed_std);
			/* EBUSY means some channel is already streaming, and has been already set with new std */
			if (ret && errno != EBUSY) {
				bc_log(Error, "Setting %s needed video standard failed, errno %d", dev_file, errno);
				goto error;
			}
		}
	}
	/* Select the CODEC */
	fmt = get_best_pixfmt(dev_fd);

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
#endif
	return;
error:
	if (dev_fd >= 0) {
		close(dev_fd);
		dev_fd = -1;
	}
}

v4l2_device_tw5864::~v4l2_device_tw5864()
{
	if (dev_fd >= 0) {
		close(dev_fd);
		dev_fd = -1;
	}
}

int v4l2_device_tw5864::start()
{
	int ret;
	char *fmtname = "";
	char err[100] = "";
	struct v4l2_event_subscription sub = {0, };

	if (dev_fd == -1) {
		bc_log(Error, "V4L2 device %s requested to start, but it failed to initialize", dev_file);
		return -1;
	}

#if 0
	switch (fmt) {
	case V4L2_PIX_FMT_H264:
		fmtname = "h264";
		break;
	case V4L2_PIX_FMT_MPEG4:
		fmtname = "mpeg4";
		break;
	case V4L2_PIX_FMT_MJPEG:
	case V4L2_PIX_FMT_JPEG:
		fmtname = "mjpeg";
	default:
		bc_log(Error, "Unsupported stream format");
		return -1;
	}
#endif

	AVDictionary *open_opts = NULL;
	av_dict_set(&open_opts, "input_format", "h264", 0);
	av_dict_set(&open_opts, "format_whitelist", "v4l2", 0);

	AVInputFormat *input_fmt = av_find_input_format("v4l2");
	if (!input_fmt) {
		bc_log(Error, "v4l2 input format not found");
		return -1;
	}

	bc_log(Debug, "Opening %s", dev_file);
	ret = avformat_open_input(&demuxer, dev_file, input_fmt, &open_opts);
	av_dict_free(&open_opts);
	if (ret) {
		av_strerror(ret, err, sizeof(err));
		bc_log(Error, "Failed to open %s: %d (%s)", dev_file, ret, err);
		goto fail;
	}

	ret = avformat_find_stream_info(demuxer, NULL);
	if (ret < 0) {
		av_strerror(ret, err, sizeof(err));
		bc_log(Error, "Failed to analyze stream from %s: %d (%s)", dev_file, ret, err);
		goto fail;
	}
#if 0
	// Subscribe for motion events
	sub.type = V4L2_EVENT_MOTION_DET;

	ret = ioctl(dev_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	if (ret) {
		strerror_r(errno, err, sizeof(err));
		bc_log(Error, "Failed to subscribe to V4L2 motion events from %s: %d (%s)", dev_file, ret, err);
	}
#endif
	_started = true;
	update_properties();
	return 0;

fail:
	avformat_close_input(&demuxer);
	return ret;
}

void v4l2_device_tw5864::stop()
{
	if (dev_fd != -1) {
		close(dev_fd);
		dev_fd = -1;
	}

	avformat_close_input(&demuxer);

	current_properties.reset();
	_started = false;
}

int v4l2_device_tw5864::read_packet()
{
	int ret;
	AVPacket pkt = {0, };
	char err[100] = "";
	struct v4l2_event ev = {0, };
	int fcntl_flags = fcntl(dev_fd, F_GETFL);

	ret = av_read_frame(demuxer, &pkt);
	if (ret) {
		av_strerror(ret, err, sizeof(err));
		bc_log(Error, "Reading packet from %s ret %d (%s)", dev_file, ret, err);
		return ret;
	}

	create_stream_packet(&pkt);
	av_free_packet(&pkt);
#if 0
	fcntl(dev_fd, F_SETFL, fcntl_flags | O_NONBLOCK);  // enter non-blocking mode
	ret = ioctl(dev_fd, VIDIOC_DQEVENT, &ev);
	fcntl(dev_fd, F_SETFL, fcntl_flags);  // restore flags, exit non-blocking mode
	if (!ret && ev.type == V4L2_EVENT_MOTION_DET)
		current_packet.flags |= stream_packet::MotionFlag;
	else if (ret == -1 && errno == ENOENT)
		;  // No motion, OK
	else
		bc_log(Warning, "Unexpected result of DQEVENT request: ret %d, errno %d, ev.type %d\n", ret, ret == -1 ? errno : 0, ev.type);
#endif
	return 0;
}

// TODO Deduplicate with lavf_device
void v4l2_device_tw5864::create_stream_packet(AVPacket *src)
{
	uint8_t *buf = new uint8_t[src->size + FF_INPUT_BUFFER_PADDING_SIZE];
	/* XXX The padding is a hack to avoid overreads by optimized
	 * functions. */
	memcpy(buf, src->data, src->size);

	current_packet = stream_packet(buf, current_properties);
	current_packet.seq      = next_packet_seq++;
	current_packet.size     = src->size;
	current_packet.ts_clock = time(NULL);
	current_packet.pts      = av_rescale_q(src->pts, demuxer->streams[src->stream_index]->time_base, AV_TIME_BASE_Q);
	if (src->flags & AV_PKT_FLAG_KEY)
		current_packet.flags |= stream_packet::KeyframeFlag;
	current_packet.ts_monotonic = bc_gettime_monotonic();

	current_packet.type = AVMEDIA_TYPE_VIDEO;
}


int v4l2_device_tw5864::set_resolution(uint16_t width, uint16_t height,
		uint8_t interval)
{
#if 0
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
			int gop = lround(vparm.parm.capture.timeperframe.denominator / vparm.parm.capture.timeperframe.numerator);
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
#endif
	return 0;
}

int v4l2_device_tw5864::set_mjpeg()
{
	vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	if (ioctl(dev_fd, VIDIOC_S_FMT, &vfmt) < 0)
		return -1;

	return 0;
}

/* Check range and convert our 0-100 to valid ranges in the hardware */
int v4l2_device_tw5864::set_control(unsigned int ctrl, int val)
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

int v4l2_device_tw5864::set_osd(const char *fmt, ...)
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

void v4l2_device_tw5864::update_properties()
{
	stream_properties *p = new stream_properties;

	AVCodecContext *ic = demuxer->streams[0]->codec;
	p->video.codec_id = demuxer->streams[0]->codec->codec_id;
	p->video.width = demuxer->streams[0]->codec->width;
	p->video.height = demuxer->streams[0]->codec->height;
	p->video.time_base = demuxer->streams[0]->time_base;
	p->video.pix_fmt = AV_PIX_FMT_YUV420P;
	if (ic->extradata && ic->extradata_size)
		p->video.extradata.assign(ic->extradata, ic->extradata + ic->extradata_size);

	current_properties = std::shared_ptr<stream_properties>(p);
}

#if 0
static const uint16_t solo_value_map[] = {
	0xffff, 1152, 1024, 768, 512, 384
};

int v4l2_device_tw5864::set_motion(bool on)
{
	int ret;
	if (!(caps() & BC_CAM_CAP_V4L2_MOTION)) {
		bc_log(Error, "Motion detection is not implemented for non-solo V4L2 devices.");
		return -ENOSYS;
	}

	struct v4l2_control vc;
	vc.id = V4L2_CID_DETECT_MD_MODE;
	vc.value = on ? V4L2_DETECT_MD_MODE_THRESHOLD_GRID : V4L2_DETECT_MD_MODE_DISABLED;
	ret = ioctl(dev_fd, VIDIOC_S_CTRL, &vc);
	if (ret) {
		char err[100] = "";
		strerror_r(errno, err, sizeof(err));
		bc_log(Error, "Failed to switch motion detection %s (%d): %s", on ? "to threshold grid mode" : "off", errno, err);
	}
	return ret;
}

int v4l2_device_tw5864::set_motion_thresh_global(char value)
{
	int val = clamp(value, '0', '5') - '0';
	if (caps() & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;
		vc.id = V4L2_CID_DETECT_MD_GLOBAL_THRESHOLD;
		/* Upper 16 bits are 0 for the global threshold */
		vc.value = solo_value_map[val];
		return ioctl(dev_fd, VIDIOC_S_CTRL, &vc);
	}

	return 0;
}

int v4l2_device_tw5864::set_motion_thresh(const char *map, size_t size)
{
	int ret;
	if (!(caps() & BC_CAM_CAP_V4L2_MOTION))
		return -ENOSYS;

	uint16_t buf[45 * 45];
	struct my_v4l2_ext_control vc = {0, };
	struct v4l2_ext_controls vcs = {0, };

	memset(buf, 0xff, sizeof(buf));

	vc.id = V4L2_CID_DETECT_MD_THRESHOLD_GRID;
	vc.p_u16 = buf;
	vc.size = 2 * 45 * 45;

	vcs.ctrl_class = V4L2_CTRL_ID2CLASS(vc.id);
	vcs.count = 1;
	vcs.controls = (struct v4l2_ext_control *)&vc;

	const unsigned vh = (caps() & BC_CAM_CAP_V4L2_PAL) ? 18 : 15;
	if (size < 22 * vh) {
		bc_log(Debug, "Received motion threshold map of wrong size");
		return -1;
	}

	ret = set_motion(true);
	if (ret)
		return ret;

	for (unsigned y = 0, pos = 0; y < vh; y++) {
		for (unsigned x = 0; x < 22; x++) {
			int val = clamp(map[pos++], '0', '5') - '0';

			/* Set motion threshold on a 2x2 sector. Our input map
			 * has half the resolution the devices work with, in
			 * both directions. */
			buf[(2*y  ) * 45 + 2*x  ] = solo_value_map[val];
			buf[(2*y+1) * 45 + 2*x  ] = solo_value_map[val];
			buf[(2*y  ) * 45 + 2*x+1] = solo_value_map[val];
			buf[(2*y+1) * 45 + 2*x+1] = solo_value_map[val];
		}
	}

	ret = ioctl(dev_fd, VIDIOC_S_EXT_CTRLS, &vcs);
	if (ret) {
		char err[100] = "";
		strerror_r(errno, err, sizeof(err));
		bc_log(Error, "Error setting motion threshold (%x): %s",
				errno, err);
	} else {
		bc_log(Debug, "Setting motion threshold succeeded");
	}

	return ret;
}
#endif

void v4l2_device_tw5864::getStatusXml(pugi::xml_node& xmlnode)
{
}
