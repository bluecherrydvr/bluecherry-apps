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
	void set_buffer_time(int prerecord);

	void destroy();
	void run();

private:
	int device_id;
	bool destroy_flag;
	bc_event_cam_type_t recording_type;

	media_writer *writer;
	bc_event_cam_t current_event;
	time_t first_packet_ts_monotonic;
	std::string snapshot_filename;

	int recording_start(time_t start, const stream_packet &first_packet);
	void recording_end();
	void do_error_event(bc_event_level_t level, bc_event_cam_type_t type);
};

#endif


