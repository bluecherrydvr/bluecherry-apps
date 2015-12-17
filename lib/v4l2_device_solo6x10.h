#ifndef V4L2DEVICE_SOLO6X10_H
#define V4L2DEVICE_SOLO6X10_H

#include "libbluecherry.h"
#include <linux/videodev2.h>

struct v4l2_buffer;

class v4l2_device_solo6x10 : public input_device
{
public:
	v4l2_device_solo6x10(BC_DB_RES dbres);
	virtual ~v4l2_device_solo6x10();

	bool has_error() const { return dev_fd < 0; }

	virtual int start();
	virtual void stop();

	virtual int read_packet();
	virtual const stream_packet &packet() const { return current_packet; }

	virtual bool has_audio() const { return 0; }

	int caps() const { return cam_caps; }

	int set_resolution(uint16_t width, uint16_t height, uint8_t interval);
	int set_osd(const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
	int set_control(unsigned int ctrl, int val);

	int set_motion(bool on);
	int set_motion_thresh(const char *map, size_t size);
	int set_motion_thresh_global(char value);

	virtual void getStatusXml(pugi::xml_node& xmlnode);
private:
	bool			started;
	int			dev_fd;
	AVFormatContext         *demuxer;
	int			cam_caps;
	struct v4l2_format	vfmt;
	struct v4l2_capability	vcap;
	struct v4l2_streamparm	vparm;
	int fmt;
	char dev_file[PATH_MAX];
	/* Userspace buffer accounting */
	int			card_id;
	int			dev_id;
	int			got_vop;
	stream_packet           current_packet;

	void update_properties();
	void create_stream_packet(AVPacket *src);
};

#endif

