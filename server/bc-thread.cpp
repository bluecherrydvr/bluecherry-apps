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
#include "rtp_device.h"
#include "v4l2_device.h"
#include "stream_elements.h"
#include "motion_processor.h"
#include "motion_handler.h"
#include "recorder.h"

static int apply_device_cfg(struct bc_record *bc_rec);

static void do_error_event(struct bc_record *bc_rec, bc_event_level_t level,
                           bc_event_cam_type_t type)
{
	if (!bc_rec->event || bc_rec->event->level != level || bc_rec->event->type != type) {
		bc_event_cam_end(&bc_rec->event);

		bc_rec->event = bc_event_cam_start(bc_rec->id, time(NULL), level, type, NULL);
	}
}

void stop_handle_properly(struct bc_record *bc_rec)
{
	struct bc_output_packet *p, *n;

	bc_streaming_destroy(bc_rec);
	bc_rec->bc->input->stop();
}

static void try_formats(struct bc_record *bc_rec)
{
	if (bc_rec->bc->type != BC_DEVICE_V4L2)
		return;

	v4l2_device *d = reinterpret_cast<v4l2_device*>(bc_rec->bc->input);

	if (d->set_resolution(bc_rec->cfg.width, bc_rec->cfg.height, bc_rec->cfg.interval)) {
		bc_rec->log.log(Warning, "Error setting format: %m");
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

	tzset();

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

	/* Update global sched */
	pthread_mutex_lock(&mutex_global_sched);
	sched_new = schedule[tm.tm_hour + (tm.tm_wday * 24)];
	pthread_mutex_unlock(&mutex_global_sched);

	if (bc_rec->sched_cur != sched_new) {
		if (!bc_rec->sched_last)
			bc_rec->sched_last = bc_rec->sched_cur;
		bc_rec->sched_cur = sched_new;
	}
}

static void *bc_device_thread(void *data)
{
	struct bc_record *bc_rec = (struct bc_record*) data;
	bc_rec->run();
	return bc_rec->thread_should_die;
}

void bc_record::run()
{
	stream_packet packet;
	int ret;
	unsigned iteration = 0;

	log.log(Info, "Setting up device");
	bc_log_context_push(log);

	while (!thread_should_die) {
		const char *err_msg;

		/* Set by bc_record_update_cfg */
		if (cfg_dirty) {
			if (apply_device_cfg(this))
				break;
		}

		update_osd(this);
		if (!(iteration++ % 50))
			check_schedule(this);

		if (!bc->input->is_started()) {
			if (bc->input->start() < 0) {
				if (!start_failed)
					log.log(Error, "Error starting device stream: %s", bc->input->get_error_message());
				start_failed++;
				do_error_event(this, BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
				goto error;
			} else if (start_failed) {
				start_failed = 0;
				log.log(Error, "Stream started after failures");
			}

			if (bc->type == BC_DEVICE_RTP) {
				const char *info = reinterpret_cast<rtp_device*>(bc->input)->stream_info();
				log.log(Info, "RTP stream started: %s", info);
			}

			if (bc_streaming_setup(this))
				log.log(Error, "Unable to setup live broadcast of device stream");
		}

		if (sched_last) {
			log.log(Info, "Switching to new recording schedule '%s'",
				sched_cur == 'M' ? "motion" : (sched_cur == 'N' ? "stopped" : "continuous"));

			destroy_elements();

			if (sched_cur == 'C') {
				rec = new recorder(this);
				rec->set_recording_type(BC_EVENT_CAM_T_CONTINUOUS);
				bc->source->connect(rec, stream_source::StartFromLastKeyframe);
				std::thread th(&recorder::run, rec);
				th.detach();
			} else if (sched_cur == 'M') {
				m_handler = new motion_handler;
				m_handler->set_buffer_time(cfg.prerecord, cfg.postrecord);
				bc->source->connect(m_handler->input_consumer(), stream_source::StartFromLastKeyframe);

				rec = new recorder(this);
				rec->set_recording_type(BC_EVENT_CAM_T_MOTION);
				m_handler->connect(rec);
				std::thread rec_th(&recorder::run, rec);
				rec_th.detach();

				if (bc->type == BC_DEVICE_V4L2) {
					static_cast<v4l2_device*>(bc->input)->set_motion(true);
				} else {
					m_processor = new motion_processor;
					update_motion_thresholds();
					bc->source->connect(m_processor, stream_source::StartFromLastKeyframe);
					m_processor->output()->connect(m_handler->create_flag_consumer());

					std::thread th(&motion_processor::run, m_processor);
					th.detach();
				}

				std::thread th(&motion_handler::run, m_handler);
				th.detach();
			}

			sched_last = 0;
		}

		ret = bc->input->read_packet();
		if (ret == EAGAIN) {
			continue;
		} else if (ret != 0) {
			if (bc->type == BC_DEVICE_RTP) {
				const char *err = reinterpret_cast<rtp_device*>(bc->input)->get_error_message();
				log.log(Error, "Read error from RTP stream: %s", *err ? err : "Unknown error");
			}

			stop_handle_properly(this);
			/* XXX this should be something other than NOT_FOUND */
			do_error_event(this, BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
			goto error;
		}

		/* End any active error events, because we successfully read a packet */
		if (event)
			bc_event_cam_end(&event);

		packet = bc->input->packet();
		bc->source->send(packet);

		/* Send packet to streaming clients */
		if (bc_streaming_is_active(this))
			bc_streaming_packet_write(this, packet);

		continue;
error:
		sleep(10);
	}

	destroy_elements();
	stop_handle_properly(this);
	bc_event_cam_end(&event);

	if (bc->type == BC_DEVICE_V4L2)
		reinterpret_cast<v4l2_device*>(bc->input)->set_osd(" ");
}

bc_record::bc_record(int i)
	: id(i), log("%d", i)
{
	bc = 0;

	memset(&cfg, 0, sizeof(cfg));
	memset(&cfg_update, 0, sizeof(cfg_update));
	cfg_dirty = 0;
	pthread_mutex_init(&cfg_mutex, NULL);

	stream_ctx = 0;
	rtsp_stream = 0;

	osd_time = 0;
	start_failed = 0;

	memset(&event, 0, sizeof(event));

	sched_cur = 'N';
	sched_last = 0;
	thread_should_die = 0;
	file_started = 0;

	m_processor = 0;
	m_handler = 0;
	rec = 0;
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

	bc_rec->log = log_context("%d/%s", id, bc_rec->cfg.name);
	bc_rec->log.set_level(bc_rec->cfg.debug_level ? Debug : (log_level)-1);

	bc = bc_handle_get(dbres);
	if (!bc) {
		/* XXX should be an event */
		bc_rec->log.log(Error, "Error opening device: %m");
		bc_status_component_error("Error opening device %d: %m", id);
		delete bc_rec;
		return 0;
	}

	bc->__data = bc_rec;
	bc_rec->bc = bc;

	bc->input->set_audio_enabled(!bc_rec->cfg.aud_disabled);

	/* Initialize device state */
	try_formats(bc_rec);
	bc_rec->update_motion_thresholds();
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
	destroy_elements();

	if (bc) {
		bc_handle_free(bc);
		bc = 0;
	}

	pthread_mutex_destroy(&cfg_mutex);
}

void bc_record::destroy_elements()
{
	if (rec) {
		rec->disconnect();
		rec->destroy();
		rec = 0;
	}

	if (m_processor) {
		m_processor->disconnect();
		m_processor->destroy();
		m_processor = 0;
	}

	if (m_handler) {
		m_handler->disconnect();
		m_handler->destroy();
		m_handler = 0;
	}

	if (bc->type == BC_DEVICE_V4L2)
		static_cast<v4l2_device*>(bc->input)->set_motion(false);
}

bool bc_record::update_motion_thresholds()
{
	bool re = false;

	if (m_processor) {
		if (m_processor->set_motion_thresh(cfg.motion_map, sizeof(cfg.motion_map)))
			log.log(Warning, "Cannot set motion thresholds (wrong configuration field?)");
		else
			re = true;
	}

	if (bc->type == BC_DEVICE_V4L2) {
		v4l2_device *v = static_cast<v4l2_device*>(bc->input);
		if (v->set_motion_thresh(cfg.motion_map, sizeof(cfg.motion_map)))
			log.log(Warning, "Cannot set motion thresholds (wrong configuration field?)");
		else
			re = true;
	}

	return re;
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
		bc_rec->log.log(Error, "Database error while updating device configuration");
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

	bc_rec->log.log(Info, "Applying configuration changes");

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
	bool mrecord_changed = (current->prerecord != update->prerecord || current->postrecord != update->postrecord);
	bool debug_changed = (current->debug_level != update->debug_level);

	memcpy(current, update, sizeof(struct bc_device_config));
	bc_rec->cfg_dirty = 0;
	pthread_mutex_unlock(&bc_rec->cfg_mutex);

	if (debug_changed) {
		bc_rec->log.set_level(current->debug_level ? Debug : (log_level)-1);
		bc_rec->log.log(Info, "Device debug level changed to %d", current->debug_level);
	}

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

	if (motion_map_changed)
		bc_rec->update_motion_thresholds();

	if (mrecord_changed)
		bc_rec->m_handler->set_buffer_time(bc_rec->cfg.prerecord, bc_rec->cfg.postrecord);

	check_schedule(bc_rec);
	return 0;
}

