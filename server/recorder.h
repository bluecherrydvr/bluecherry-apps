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

	int get_device_id() { return device_id; }
	bc_event_cam_t get_current_event() { return current_event; }

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

	bool snapshotting_proceeding;
	int snapshots_limit;  /* TODO Optionize */
	int snapshots_done;
	int snapshotting_delay_since_motion_start_ms;
    bool event_notification_executed;

	int recording_start(time_t start, const stream_packet &first_packet);
	void recording_end();
	void do_error_event(bc_event_level_t level, bc_event_cam_type_t type);
    void event_trigger_notifications(bc_event_cam_t event);
};

#endif


