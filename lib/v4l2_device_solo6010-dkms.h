#ifndef V4L2DEVICE_SOLO6010_DKMS_H
#define V4L2DEVICE_SOLO6010_DKMS_H

#include "libbluecherry.h"
#include <linux/videodev2.h>

#define BC_BUFFERS		16
#define BC_BUFFERS_JPEG		8

/* Some things that are driver specific */
#ifndef V4L2_BUF_FLAG_MOTION_ON
#define V4L2_BUF_FLAG_MOTION_ON		0x0400
#define V4L2_BUF_FLAG_MOTION_DETECTED	0x0800
#endif

#ifndef V4L2_CID_MOTION_ENABLE
#define V4L2_CID_MOTION_ENABLE		(V4L2_CID_PRIVATE_BASE+0)
#define V4L2_CID_MOTION_THRESHOLD	(V4L2_CID_PRIVATE_BASE+1)
#define V4L2_CID_MOTION_TRACE		(V4L2_CID_PRIVATE_BASE+2)
#endif

/* Camera capability flags */
#define BC_CAM_CAP_V4L2_MOTION 0x00000002
#define BC_CAM_CAP_OSD         0x00000004
#define BC_CAM_CAP_SOLO        0x00000008
#define BC_CAM_CAP_MJPEG_URL   0x00000010
/* Set for all BC_DEVICE_V4L2 handles operating
 * in PAL. NTSC is assumed if unset. */
#define BC_CAM_CAP_V4L2_PAL    0x00000020

/* ATTENTION! */
#define v4l2_device v4l2_device_solo6010_dkms

struct v4l2_buffer;

class v4l2_device_solo6010_dkms : public input_device
{
public:
	v4l2_device_solo6010_dkms(BC_DB_RES dbres);
	virtual ~v4l2_device_solo6010_dkms();

	bool has_error() const { return dev_fd < 0; }

	virtual int start();
	virtual void stop();

	virtual int read_packet();
	virtual const stream_packet &packet() const { return current_packet; }

	virtual bool has_audio() const { return 0; }

	int caps() const { return cam_caps; }
	int device_fd() const { return dev_fd; }

	int set_resolution(uint16_t width, uint16_t height, uint8_t interval);
	int set_osd(const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
	int set_mjpeg();
	int set_control(unsigned int ctrl, int val);

	int set_motion(bool on);
	int set_motion_thresh(const char *map, size_t size);
	int set_motion_thresh_global(char value);

	v4l2_buffer *buf_v4l2() __attribute__((pure));

	virtual void getStatusXml(pugi::xml_node& xmlnode);
private:
	bool			started;
	int			dev_fd;
	int			cam_caps;
	struct v4l2_format	vfmt;
	struct v4l2_capability	vcap;
	struct v4l2_streamparm	vparm;
	enum   AVCodecID	codec_id;
	/* Userspace buffer accounting */
	struct {
		void			*data;
		size_t			size;
		struct v4l2_buffer	vb;
	}			p_buf[BC_BUFFERS];
	unsigned int		local_bufs;
	int			buf_idx;
	int			gop;
	unsigned int		buffers;
	int			card_id;
	int			dev_id;
	int			got_vop;
	stream_packet           current_packet;

	int is_key_frame() __attribute__((pure));

	void v4l2_local_bufs();
	int v4l2_bufs_prepare();
	void buf_return();

	void update_properties();
};

#endif

