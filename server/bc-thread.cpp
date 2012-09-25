/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <thread>

#include "bc-server.h"
#include "rtp-session.h"
#include "v4l2device.h"
#include "stream_elements.h"
#include "motion_processor.h"
#include "recorder.h"

static int apply_device_cfg(struct bc_record *bc_rec);

static void recording_end(struct bc_record *bc_rec)
{
	bc_log("W: recording_end is not real");
}

static void do_error_event(struct bc_record *bc_rec, bc_event_level_t level,
                           bc_event_cam_type_t type)
{
	if (!bc_rec->event || bc_rec->event->level != level || bc_rec->event->type != type) {
		recording_end(bc_rec);
		bc_event_cam_end(&bc_rec->event);

		bc_rec->event = bc_event_cam_start(bc_rec->id, time(NULL), level, type, NULL);
	}
}

static void stop_handle_properly(struct bc_record *bc_rec)
{
	struct bc_output_packet *p, *n;

	recording_end(bc_rec);
	bc_streaming_destroy(bc_rec);
	bc_handle_stop(bc_rec->bc);
	bc_rec->mot_last_ts = 0;
	bc_rec->mot_first_buffered_packet = -1;
	bc_rec->prerecord_buffer.clear();
}

static void event_trigger_notifications(struct bc_record *bc_rec)
{
	if (bc_rec->sched_cur != 'M')
		return;

	pid_t pid = fork();
	if (pid < 0) {
		bc_dev_warn(bc_rec, "Cannot fork for event notification");
		return;
	}

	/* Parent process */
	if (pid)
		return;

	char id[24] = { 0 };
	snprintf(id, sizeof(id), "%d", bc_rec->event->media.table_id);
	execl("/usr/bin/php", "/usr/bin/php", "/usr/share/bluecherry/www/lib/mailer.php", id, NULL);
	exit(1);
}

static void try_formats(struct bc_record *bc_rec)
{
	if (bc_rec->bc->type != BC_DEVICE_V4L2)
		return;

	v4l2_device *d = reinterpret_cast<v4l2_device*>(bc_rec->bc->input);

	if (d->set_interval(bc_rec->cfg.interval)) {
		bc_rec->reset_vid = 1;
		if (errno != EAGAIN)
			bc_dev_warn(bc_rec, "Failed to set video interval: %m");
	}

	if (d->set_format(bc_rec->fmt, bc_rec->cfg.width, bc_rec->cfg.height)) {
		bc_rec->reset_vid = 1;
		if (errno != EAGAIN)
			bc_dev_warn(bc_rec, "Error setting format: %m");
	}
}

static void update_osd(struct bc_record *bc_rec)
{
	if (bc_rec->bc->type != BC_DEVICE_V4L2)
		return;

	v4l2_device *d = reinterpret_cast<v4l2_device*>(bc_rec->bc->input);
	time_t t = time(NULL);
	char buf[20];
	struct tm tm;

	if (!(d->caps() & BC_CAM_CAP_OSD))
		return;

	if (t == bc_rec->osd_time)
		return;

	bc_rec->osd_time = t;
	strftime(buf, 20, "%F %T", localtime_r(&t, &tm));
	d->set_osd("%s %s", bc_rec->cfg.name, buf);
}

static void check_schedule(struct bc_record *bc_rec)
{
	const char *schedule = global_sched;
	time_t t;
	struct tm tm;
	char sched_new;

	if (bc_rec->cfg.schedule_override_global)
		schedule = bc_rec->cfg.schedule;

	time(&t);
	localtime_r(&t, &tm);

	sched_new = schedule[tm.tm_hour + (tm.tm_wday * 24)];
	if (bc_rec->sched_cur != sched_new) {
		if (!bc_rec->sched_last)
			bc_rec->sched_last = bc_rec->sched_cur;
		bc_rec->sched_cur = sched_new;
	}
}

static int process_schedule(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;

	check_schedule(bc_rec);

	if (bc_rec->reset_vid ||
	    (bc_event_media_length(bc_rec->event) > BC_MAX_RECORD_TIME &&
	    !bc_rec->sched_last)) {
		if (bc_rec->reset_vid)
			bc_rec->reset_vid = 0;
		recording_end(bc_rec);
		bc_handle_reset(bc);
		try_formats(bc_rec);
		bc_dev_info(bc_rec, "Reset media file");
	} else if (bc_rec->sched_last) {
		recording_end(bc_rec);
		bc_event_cam_end(&bc_rec->event);
		bc_handle_reset(bc);
		bc_set_motion(bc, bc_rec->sched_cur == 'M' ? 1 : 0);
		bc_rec->sched_last = 0;
		bc_dev_info(bc_rec, "Switching to new recording schedule '%s'",
		       bc_rec->sched_cur == 'M' ? "motion" : (bc_rec->sched_cur
		       == 'N' ? "stopped" : "continuous"));
	}

	return (bc_rec->sched_cur != 'N');
}

static int check_motion(struct bc_record *bc_rec, const stream_packet &packet)
{
	time_t monotonic_now;
	int ret;

	if (bc_rec->sched_cur != 'M')
		return 1;

	/* This may be necessary in some cases when the handle was stopped
	 * (and thus, motion reset to off) without a schedule change. */
	bc_set_motion(bc_rec->bc, 1);

	monotonic_now = bc_gettime_monotonic();

	if (!bc_rec->prerecord_buffer.add_packet(packet) ||
	    !packet.is_video_frame())
		return bc_rec->mot_last_ts > 0;

	ret = bc_motion_is_detected(bc_rec->bc);

	if (bc_rec->mot_first_buffered_packet >= 0) {
		/* Don't end the event and close the recording until we've dropped
		 * a frame out of the prerecording buffer that was not part of
		 * the recording; this allows us to resume the current event if more
		 * motion occurs within [prerecord] seconds. */
		for (auto it = bc_rec->prerecord_buffer.begin(); it != bc_rec->prerecord_buffer.end(); it++) {
			if (it->seq != bc_rec->mot_first_buffered_packet)
				continue;

			if (ret) {
				/* There is new motion; we can resume this recording
				 * using buffered packets. Dump them, excluding this packet. */
				bc_dev_info(bc_rec, "Motion event continued");
				for (; it != bc_rec->prerecord_buffer.end()-1; it++)
					bc_output_packet_write(bc_rec, *it);
				bc_rec->mot_first_buffered_packet = -1;
			}

			goto continue_motion_event;
		}

		/* We exhausted the prerecord buffer without more motion;
		 * end the current recording. If this frame had motion,
		 * we'll start a new recording immediately. */
		recording_end(bc_rec);
		bc_rec->mot_last_ts = 0;
		bc_rec->mot_first_buffered_packet = -1;

continue_motion_event:
		;
	}

	if (ret) {
		/* Motion frame; update the timestamp */
		bc_rec->mot_last_ts = monotonic_now;
	} else if (bc_rec->mot_last_ts) {
		/* Actively recording, but this frame has no motion */
		if (monotonic_now - bc_rec->mot_last_ts <= bc_rec->cfg.postrecord)
			ret = 1; /* Continue recording (postrecord time) */
		else if (bc_rec->mot_first_buffered_packet < 0) {
			bc_rec->mot_first_buffered_packet = packet.seq;
			if (bc_rec->event)
				bc_rec->event->end_time = time(NULL);
		}
	}

	return ret;
}

static void *bc_device_thread(void *data)
{
	struct bc_record *bc_rec = (struct bc_record*) data;
	struct bc_handle *bc = bc_rec->bc;
	stream_packet packet;
	int ret;

	bc_dev_info(bc_rec, "Camera configured");
	bc_av_log_set_handle_thread(bc_rec);

	recorder *rec = 0;

	while (!bc_rec->thread_should_die) {
		const char *err_msg;

		/* Set by bc_record_update_cfg */
		if (bc_rec->cfg_dirty) {
			if (apply_device_cfg(bc_rec))
				break;
		}

		update_osd(bc_rec);

		int schedule_recording = process_schedule(bc_rec);

		if (!bc->started) {
			if (bc_handle_start(bc, &err_msg)) {
				if (!bc_rec->start_failed)
					bc_dev_err(bc_rec, "Error starting stream: %s", err_msg);
				bc_rec->start_failed++;
				do_error_event(bc_rec, BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
				goto error;
			} else if (bc_rec->start_failed) {
				bc_rec->start_failed = 0;
				bc_dev_info(bc_rec, "Device started after failure(s)");
			}

			if (bc->type == BC_DEVICE_RTP) {
				const char *info = reinterpret_cast<rtp_device*>(bc->input)->stream_info();
				bc_dev_info(bc_rec, "RTP stream started: %s", info);
			}

			if (bc_streaming_setup(bc_rec))
				bc_dev_err(bc_rec, "Error setting up live stream");
		}

		if (bc_rec->sched_cur == 'M' && !bc_rec->motion) {
			bc_rec->motion = new motion_processor(bc->input);
			bc->source->connect(bc_rec->motion);
			std::thread th(&motion_processor::run, bc_rec->motion);
			th.detach();
		} else if (bc_rec->sched_cur != 'M' && bc_rec->motion) {
			bc->source->disconnect(bc_rec->motion);
			bc_rec->motion->destroy();
			bc_rec->motion = 0;
		}

		if (bc_rec->sched_cur == 'C' && !rec) {
			rec = new recorder(bc_rec);
			bc->source->connect(rec);
			std::thread th(&recorder::run, rec);
			th.detach();
		} else if (bc_rec->sched_cur != 'C' && rec) {
			bc->source->disconnect(rec);
			rec->destroy();
			rec = 0;
		}

		ret = bc->input->read_packet();
		if (ret == EAGAIN) {
			continue;
		} else if (ret != 0) {
			if (bc->type == BC_DEVICE_RTP) {
				const char *err = reinterpret_cast<rtp_device*>(bc->input)->get_error_message();
				bc_dev_err(bc_rec, "RTSP read error: %s", *err ? err : "Unknown error");
			}

			stop_handle_properly(bc_rec);
			/* XXX this should be something other than NOT_FOUND */
			do_error_event(bc_rec, BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
			goto error;
		}

		packet = bc->input->packet();
		bc->source->send(packet);

		/* Send packet to streaming clients */
		if (bc_streaming_is_active(bc_rec))
			bc_streaming_packet_write(bc_rec, packet);

		continue;
error:
		sleep(10);
	}

	stop_handle_properly(bc_rec);
	bc_event_cam_end(&bc_rec->event);

	if (bc->type == BC_DEVICE_V4L2)
		reinterpret_cast<v4l2_device*>(bc->input)->set_osd(" ");

	return bc_rec->thread_should_die;
}

bc_record::bc_record(int i)
	: id(i)
{
	bc = 0;

	memset(&cfg, 0, sizeof(cfg));
	memset(&cfg_update, 0, sizeof(cfg_update));
	cfg_dirty = 0;
	pthread_mutex_init(&cfg_mutex, NULL);

	fmt_out = 0;
	video_st = 0;
	audio_st = 0;
	oc = 0;
	output_pts_base = 0;

	stream_ctx = 0;
	rtsp_stream = 0;

	osd_time = 0;
	start_failed = 0;

	memset(outfile, 0, sizeof(outfile));

	memset(&event, 0, sizeof(event));
	event_snapshot_done = 0;

	sched_cur = 'N';
	sched_last = 0;
	thread_should_die = 0;
	file_started = 0;

	reset_vid = 0;
	fmt = 0;

	mot_last_ts = 0;
	mot_first_buffered_packet = -1;
}

bc_record *bc_record::create_from_db(int id, BC_DB_RES dbres)
{
	bc_record *bc_rec;
	struct bc_handle *bc = NULL;

	if (bc_db_get_val_bool(dbres, "disabled"))
		return 0;

	const char *signal_type = bc_db_get_val(dbres, "signal_type", NULL);
	const char *video_type = bc_db_get_val(dbres, "video_type", NULL);
	if (signal_type && video_type && strcasecmp(signal_type, video_type)) {
		bc_status_component_error("Video type mismatch for device %d "
			"(driver is %s, device is %s)", id, video_type, signal_type);
		return 0;
	}

	bc_rec = new bc_record(id);

	if (bc_device_config_init(&bc_rec->cfg, dbres)) {
		bc_status_component_error("Database error while initializing device %d", id);
		delete bc_rec;
		return 0;
	}
	memcpy(&bc_rec->cfg_update, &bc_rec->cfg, sizeof(bc_rec->cfg));

	// XXX useless?
	bc_rec->fmt = V4L2_PIX_FMT_MPEG;

	bc = bc_handle_get(dbres);
	if (!bc) {
		/* XXX should be an event */
		bc_dev_err(bc_rec, "Error opening device: %m");
		bc_status_component_error("Error opening device %d: %m", id);
		delete bc_rec;
		return 0;
	}

	bc->__data = bc_rec;
	bc_rec->bc = bc;

	bc->input->set_audio_enabled(!bc_rec->cfg.aud_disabled);
	bc_rec->prerecord_buffer.set_duration(bc_rec->cfg.prerecord);

	/* Initialize device state */
	try_formats(bc_rec);
	if (bc_set_motion_thresh(bc, bc_rec->cfg.motion_map,
	    sizeof(bc_rec->cfg.motion_map)))
	{
		bc_dev_warn(bc_rec, "Cannot set motion thresholds; corrupt configuration?");
	}
	check_schedule(bc_rec);

	if (bc->type == BC_DEVICE_V4L2) {
		v4l2_device *v4l2 = static_cast<v4l2_device*>(bc->input);
		v4l2->set_control(V4L2_CID_HUE, bc_rec->cfg.hue);
		v4l2->set_control(V4L2_CID_CONTRAST, bc_rec->cfg.contrast);
		v4l2->set_control(V4L2_CID_SATURATION, bc_rec->cfg.saturation);
		v4l2->set_control(V4L2_CID_BRIGHTNESS, bc_rec->cfg.brightness);
	}

	if (pthread_create(&bc_rec->thread, NULL, bc_device_thread,
			   bc_rec) != 0) {
		bc_status_component_error("Failed to start thread: %m");
		delete bc_rec;
		return 0;
	}

	// XXX useless?
	/* Throttle thread starting */
	sleep(1);

	return bc_rec;
}

// XXX Many other members of bc_record are ignored here.
bc_record::~bc_record()
{
	if (bc) {
		bc_handle_free(bc);
		bc = 0;
	}

	pthread_mutex_destroy(&cfg_mutex);
}

int bc_record_update_cfg(struct bc_record *bc_rec, BC_DB_RES dbres)
{
	struct bc_device_config cfg_tmp;
	memset(&cfg_tmp, 0, sizeof(cfg_tmp));

	if (bc_db_get_val_int(dbres, "disabled") > 0) {
		bc_rec->thread_should_die = "Disabled in config";
		return 0;
	}

	if (bc_device_config_init(&cfg_tmp, dbres)) {
		bc_log("E(%d): Database error while updating device configuration", bc_rec->id);
		return -1;
	}

	pthread_mutex_lock(&bc_rec->cfg_mutex);
	if (memcmp(&bc_rec->cfg_update, &cfg_tmp, sizeof(struct bc_device_config))) {
		memcpy(&bc_rec->cfg_update, &cfg_tmp, sizeof(struct bc_device_config));
		bc_rec->cfg_dirty = 1;
	}
	pthread_mutex_unlock(&bc_rec->cfg_mutex);

	return 0;
}

static int apply_device_cfg(struct bc_record *bc_rec)
{
	struct bc_device_config *current = &bc_rec->cfg;
	struct bc_device_config *update  = &bc_rec->cfg_update;
	int motion_map_changed, format_changed;

	pthread_mutex_lock(&bc_rec->cfg_mutex);

	bc_dev_info(bc_rec, "Applying configuration changes");

	if (strcmp(current->dev, update->dev) || strcmp(current->driver, update->driver) ||
	    strcmp(current->signal_type, update->signal_type) ||
	    strcmp(current->rtsp_username, update->rtsp_username) ||
	    strcmp(current->rtsp_password, update->rtsp_password) ||
	    current->aud_disabled != update->aud_disabled)
	{
		bc_rec->thread_should_die = "configuration changed";
		pthread_mutex_unlock(&bc_rec->cfg_mutex);
		return -1;
	}

	motion_map_changed = memcmp(current->motion_map, update->motion_map, sizeof(update->motion_map));
	format_changed = (current->width != update->width || current->height != update->height ||
	                  current->interval != update->interval);
	bool control_changed = (current->hue != update->hue || current->contrast != update->contrast ||
	                        current->saturation != update->saturation ||
	                        current->brightness != update->brightness);

	memcpy(current, update, sizeof(struct bc_device_config));
	bc_rec->cfg_dirty = 0;
	pthread_mutex_unlock(&bc_rec->cfg_mutex);

	if (format_changed) {
		stop_handle_properly(bc_rec);
		bc_streaming_destroy(bc_rec);
		try_formats(bc_rec);
	}

	if (control_changed) {
		v4l2_device *v4l2 = static_cast<v4l2_device*>(bc_rec->bc->input);
		v4l2->set_control(V4L2_CID_HUE, current->hue);
		v4l2->set_control(V4L2_CID_CONTRAST, current->contrast);
		v4l2->set_control(V4L2_CID_SATURATION, current->saturation);
		v4l2->set_control(V4L2_CID_BRIGHTNESS, current->brightness);
	}

	if (motion_map_changed) {
		if (bc_set_motion_thresh(bc_rec->bc, bc_rec->cfg.motion_map,
		    sizeof(bc_rec->cfg.motion_map)))
		{
			bc_dev_warn(bc_rec, "Cannot set motion thresholds; corrupt configuration?");
		}
	}

	bc_rec->prerecord_buffer.set_duration(bc_rec->cfg.prerecord);

	check_schedule(bc_rec);
	return 0;
}

