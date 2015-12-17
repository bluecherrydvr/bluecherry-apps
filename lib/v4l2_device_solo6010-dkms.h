#ifndef V4L2DEVICE_SOLO6010_DKMS_H
#define V4L2DEVICE_SOLO6010_DKMS_H

#include "libbluecherry.h"
#include <linux/videodev2.h>

struct v4l2_buffer;

class v4l2_device_solo6010_dkms : public input_device
{
public:
	v4l2_device_solo6010_dkms(BC_DB_RES dbres);
	virtual ~v4l2_device_solo6010_dkms();

	virtual bool has_error() const { return dev_fd < 0; }

	virtual int start();
	virtual void stop();

	virtual int read_packet();
	virtual const stream_packet &packet() const { return current_packet; }

	virtual bool has_audio() const { return 0; }

	virtual int caps() const { return cam_caps; }

	virtual int set_resolution(uint16_t width, uint16_t height, uint8_t interval);
	virtual int set_osd(const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
	virtual int set_mjpeg();
	virtual int set_control(unsigned int ctrl, int val);

	virtual int set_motion(bool on);
	virtual int set_motion_thresh(const char *map, size_t size);
	virtual int set_motion_thresh_global(char value);

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

