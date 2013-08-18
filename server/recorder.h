#ifndef RECORDER_H
#define RECORDER_H

#include "stream_elements.h"

struct AVCodecContext;
struct bc_record;
class media_writer;

class recorder : public stream_consumer
{
public:
	recorder(const bc_record *input);
	virtual ~recorder();

	void set_recording_type(bc_event_cam_type_t type);

	void destroy();
	void run();

	/* added by ruminsam
	 * set the pre recording time. */
	void set_buffer_time(int prerecord);

private:
	int device_id;
	bool destroy_flag;
	bc_event_cam_type_t recording_type;

	media_writer *writer;
	bc_event_cam_t current_event;

	int recording_start(time_t start, const stream_packet &first_packet);
	/* Get the path for a media file on the current device with the given start time,
	 * creating folders if necessary. Repeated calls may return different results; use
	 * writer to get the current media file's path. */
	std::string media_file_path(time_t start);
	void recording_end();
	void do_error_event(bc_event_level_t level, bc_event_cam_type_t type);
};

#endif


