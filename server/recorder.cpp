#include <unistd.h>
#include <assert.h>
#include <bsd/string.h>
#include "recorder.h"
#include "bc-server.h"
#include "media_writer.h"

recorder::recorder(const bc_record *bc_rec)
	: stream_consumer("Recorder"), device_id(bc_rec->id), destroy_flag(false),
	  recording_type(BC_EVENT_CAM_T_CONTINUOUS), writer(0), current_event(0)
{
}

recorder::~recorder()
{
	stop_snapshot();
	delete writer;
}

void recorder::set_recording_type(bc_event_cam_type_t type)
{
	recording_type = type;
}

void recorder::destroy()
{
	destroy_flag = true;
	buffer_wait.notify_all();
}

void recorder::run()
{
	pthread_setname_np(pthread_self(), thread_name);
	std::shared_ptr<const stream_properties> saved_properties;
	bool bool_ret;

	bc_log_context_push(log);

	std::unique_lock<std::mutex> l(lock);
	while (!destroy_flag)
	{
		if (buffer.empty()) {
			buffer_wait.wait(l);
			continue;
		}

		stream_packet packet = buffer.front();
		buffer.pop_front();
		l.unlock();

		if (!packet.data()) {
			/* Null packets force recording end */
			recording_end();
			l.lock();
			continue;
		}

		if (!saved_properties)
			saved_properties = packet.properties();

		if (packet.properties() != saved_properties && *packet.properties() != *saved_properties) {
			bc_log(Debug, "recorder: stream properties changed");
			saved_properties = packet.properties();
			recording_end();
		}

		if (current_event && packet.is_key_frame() && packet.is_video_frame() &&
				bc_event_media_length(current_event) > max_record_time_sec) {
			recording_end();
		}

		/* Setup and write to recordings */
		if (!writer) {
			if (!packet.is_key_frame() || !packet.is_video_frame()) {
				/* Recordings must always start with a video keyframe; this should
				 * be ensured in most cases (continuous splits, initialization), but
				 * if absolutely necessary we'll drop packets. */
				goto end;
			}

			if (recording_start(packet.ts_clock, packet)) {
				bc_log(Error, "Cannot start recording!");
				recording_end();
				sleep(10);
				goto end;
			}
		}

		bool_ret = writer->write_packet(packet);
		if (!bool_ret) {
			bc_log(Error, "Failure in recording writing");
			recording_end();
			goto end;
		}

		/* Snapshot(s) producing */

		/* TODO Pass decoded picture to stream_packet if available. This is not
		 * always the case (e.g. V4L2 devices pass encoded packets and we don't
		 * decode it for motion detection because it is done by device itself, also
		 * we don't decode if we just pull recording from e.g. ONVIF camera that
		 * has on-camera motion detection.
		 */

		if (recording_type == BC_EVENT_CAM_T_MOTION || recording_type == BC_EVENT_CAM_T_CONTINUOUS) {
			if (snapshot != NULL) {
				int ret = snapshot->feed(packet);
				if (ret < 0) {
					bc_log(Error, "Failed while feeding snapshot saver with more frames");
					stop_snapshot();
				} else if (ret > 0) {
					bc_log(Debug, "Still need to feed more frames to finish snapshot");
				} else {
					bc_log(Debug, "Finalized snapshot");
					stop_snapshot();
					snapshots_done++;
					event_trigger_notifications(current_event);
				}
			} else if (snapshots_done < snapshots_limit
					&& packet.is_video_frame() && packet.is_key_frame()
					&& packet.ts_monotonic > (first_packet_ts_monotonic + buffer.duration()
						/* TODO higher precision for time storage */
						/* TODO support millisecond precision for delay option */
						+ (snapshotting_delay_since_motion_start_ms/1000))) {

				stop_snapshot();
				bc_log(Debug, "Making a snapshot");

				// Push frames to decoder until picture is taken
				// In some cases one AVPacket marked as keyframe is not enough, and next
				// packet must be pushed to decoder, too.

				snapshot = new snapshot_writer(snapshot_filename.c_str());
				if (snapshot == NULL)
				{
					bc_log(Error, "Failed to initialize snapshot writter");
					goto end;
				}

				int ret = snapshot->feed(packet);
				if (ret < 0) {
					bc_log(Error, "Failed to make snapshot");
					stop_snapshot();
				} else if (ret > 0) {
					bc_log(Debug, "Need to feed more frames to finish snapshot");
				} else {
					bc_log(Debug, "Saved snapshot from single keyframe");
					snapshots_done++;
					stop_snapshot();
					event_trigger_notifications(current_event);
				}
			}
		}

end:
		l.lock();
	}

	bc_log(Debug, "recorder destroying");
	l.unlock();
	recording_end();
	bc_event_cam_end(&current_event);
	delete this;
}

void recorder::event_trigger_notifications(bc_event_cam_t event)
{
    event_notification_executed = true;
	pid_t pid = fork();
	if (pid < 0) {
		bc_log(Bug, "cannot fork for event notification");
		return;
	}

	/* Parent process */
	if (pid)
		return;

	char id[24] = { 0 };
	snprintf(id, sizeof(id), "%lu", event->media.table_id);
	
	// Determine the event type string for the notification
	const char* event_type = "motion_event";
	switch (event->type) {
		case BC_EVENT_CAM_T_MOTION:
			event_type = "motion_event";
			break;
		case BC_EVENT_CAM_T_VEHICLE:
			event_type = "vehicle_event";
			break;
		case BC_EVENT_CAM_T_PERSON:
			event_type = "person_event";
			break;
		case BC_EVENT_CAM_T_ANIMAL:
			event_type = "animal_event";
			break;
		default:
			event_type = "motion_event";
			break;
	}
	
	execl("/usr/bin/php", "/usr/bin/php", "/usr/share/bluecherry/www/lib/mailer.php", event_type, id, NULL);
	exit(1);
}


/**
 * Get the path for a media file on the current device with the given start
 * time, creating folders if necessary. Repeated calls may return different
 * results; use writer to get the current media file's path.
 */
static int media_file_path(char *dst, size_t len, time_t start_ts, int device_id, bc_event_cam_type_t rec_type)
{
	struct tm tm;
	char date[12], fname[32];
	char basepath[PATH_MAX];
	int ret;

	ret = bc_get_media_loc(basepath, sizeof(basepath));
	if (ret < 0) {
		bc_log(Error, "No free storage locations for new recordings!");
		return -1;
	}

	localtime_r(&start_ts, &tm);
	strftime(date, sizeof(date), "%Y/%m/%d", &tm);

	// Handle different event types
	switch (rec_type) {
		case BC_EVENT_CAM_T_MOTION:
			strftime(fname, sizeof(fname), "/%H-%M-%S-motion.mp4", &tm);
			break;
		case BC_EVENT_CAM_T_VEHICLE:
			strftime(fname, sizeof(fname), "/%H-%M-%S-vehicle.mp4", &tm);
			break;
		case BC_EVENT_CAM_T_PERSON:
			strftime(fname, sizeof(fname), "/%H-%M-%S-person.mp4", &tm);
			break;
		case BC_EVENT_CAM_T_ANIMAL:
			strftime(fname, sizeof(fname), "/%H-%M-%S-animal.mp4", &tm);
			break;
		default:
			strftime(fname, sizeof(fname), "/%H-%M-%S.mp4", &tm);
			break;
	}

	/* Construct full path except final file name, for mkdir */
	size_t dir_len = snprintf(dst, len, "%s/%s/%06d",
				  basepath, date, device_id);
	if (dir_len + sizeof(fname) >= len) {
		bc_log(Error, "Too long resulting recording filename,"
				" not fitting to %zu-bytes buffer", len);
		return -1;
	}

	if (bc_mkdir_recursive(dst) < 0) {
		bc_log(Error, "Cannot create media directory %s", dst);
		return -1;
	}

	strlcat(dst + dir_len, fname, len - dir_len);

	return 0;
}

int recorder::recording_start(time_t start_ts, const stream_packet &first_packet)
{
	bc_event_cam_t nevent = NULL;

	if (!start_ts)
		start_ts = time(NULL);

	recording_end();

	snapshots_limit = 1;  /* TODO Optionize */
	snapshots_done = 0;
	snapshotting_delay_since_motion_start_ms = snapshot_delay_ms;
	event_notification_executed = false;

	char outfile[PATH_MAX];
	int ret = media_file_path(outfile, sizeof(outfile), start_ts, device_id, get_recording_type());
	if (ret < 0) {
		do_error_event(BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
		return -1;
	}

	writer = new media_writer;
	if (writer->open(std::string(outfile), *first_packet.properties().get()) != 0) {
		do_error_event(BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
		return -1;
	}

	bc_event_level_t level = (recording_type == BC_EVENT_CAM_T_MOTION || 
	                          recording_type == BC_EVENT_CAM_T_VEHICLE || 
	                          recording_type == BC_EVENT_CAM_T_PERSON || 
	                          recording_type == BC_EVENT_CAM_T_ANIMAL) ? BC_EVENT_L_WARN : BC_EVENT_L_INFO;
	nevent = bc_event_cam_start(device_id, start_ts, level, recording_type, outfile);

	if (!nevent) {
		do_error_event(BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
		unlink(outfile); // this file will be untracked anyway
		return -1;
	}

	bc_event_cam_end(&current_event);
	current_event = nevent;

	// Save timestamp of first packet
	first_packet_ts_monotonic = first_packet.ts_monotonic;
	char *ext = strrchr(outfile, '.');
	assert(ext);
	if (!ext)
		return -1;
	strcpy(ext + 1, "jpg");
	snapshot_filename = outfile;

	return 0;
}

void recorder::recording_end()
{
	/* Close the media entry in the db */
	if (current_event && bc_event_has_media(current_event))
	{
		if ((recording_type == BC_EVENT_CAM_T_MOTION ||
		     recording_type == BC_EVENT_CAM_T_VEHICLE ||
		     recording_type == BC_EVENT_CAM_T_PERSON ||
		     recording_type == BC_EVENT_CAM_T_ANIMAL) &&
			!event_notification_executed)
			event_trigger_notifications(current_event);

		bc_event_cam_end(&current_event);
	}

	if (writer != NULL)
	{
		writer->close();
		delete writer;
		writer = NULL;
	}

	stop_snapshot();
}

void recorder::stop_snapshot()
{
	if (snapshot != NULL)
	{
		delete snapshot;
		snapshot = NULL;
	}
}

void recorder::do_error_event(bc_event_level_t level, bc_event_cam_type_t type)
{
	if (!current_event || current_event->level != level || current_event->type != type) {
		recording_end();
		bc_event_cam_end(&current_event);

		current_event = bc_event_cam_start(device_id, time(NULL), level, type, NULL);
	}
}

void recorder::set_buffer_time(int prerecord)
{
	buffer.set_duration(prerecord);
}

