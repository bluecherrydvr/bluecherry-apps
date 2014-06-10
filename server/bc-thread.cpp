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

#include "bt.h"

#include "bc-server.h"
#include "lavf_device.h"
#include "v4l2_device.h"
#include "stream_elements.h"
#include "motion_processor.h"
#include "motion_handler.h"
#include "recorder.h"

#define DEF_TH_LOG_LEVEL Warning

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
	bc_streaming_destroy(bc_rec);
	bc_rec->bc->input->stop();
}

static void try_formats(struct bc_record *bc_rec)
{
	if (bc_rec->bc->type != BC_DEVICE_V4L2)
		return;

	v4l2_device *d = reinterpret_cast<v4l2_device*>(bc_rec->bc->input);

	if (d->set_resolution(bc_rec->cfg.width, bc_rec->cfg.height, bc_rec->cfg.interval)) {
		bc_rec->log.log(Warning, "Error setting format");
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
	return (void*)bc_rec->thread_should_die;
}

static
void bc_rec_thread_cleanup(void *data)
{
	bt("bc_record unexpectedly cancelled", RET_ADDR);
}

void bc_record::notify_device_state(const char *state)
{
	pid_t pid = fork();
	if (pid < 0) {
		bc_log(Bug, "cannot fork for event notification");
		return;
	}

	/* Parent process */
	if (pid)
		return;

	char id_str[20];
	snprintf(id_str, sizeof(id_str), "%d", id);
	execl("/usr/bin/php", "/usr/bin/php", "/usr/share/bluecherry/www/lib/mailer.php", "device_state", id_str, state, NULL);
	exit(1);
}

void bc_record::run()
{
	stream_packet packet;
	int ret;
	unsigned iteration = 0;

	log.log(Info, "Setting up device");
	bc_log_context_push(log);

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	pthread_cleanup_push(bc_rec_thread_cleanup, NULL);

	while (1) {
		const char *local_thread_should_die;
		char local_cfg_dirty;
		pthread_mutex_lock(&cfg_mutex);
		local_thread_should_die = thread_should_die;
		local_cfg_dirty = cfg_dirty;
		pthread_mutex_unlock(&cfg_mutex);
		if (local_thread_should_die)
			break;
		if (local_cfg_dirty) {
			if (apply_device_cfg(this))
				break;
		}

		update_osd(this);
		if (!(iteration++ % 50))
			check_schedule(this);

		if (!bc->input->is_started()) {
			if (bc->input->start() < 0) {
				if (!start_failed) {
					log.log(Error, "Error starting device stream: %s", bc->input->get_error_message());
					/* Notification hook to PHP: device is offline */
					notify_device_state("OFFLINE");
				}
				start_failed++;
				do_error_event(this, BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
				goto error;
			} else if (start_failed) {
				start_failed = 0;
				log.log(Error, "Stream started after failures");
			}

			if (bc->type == BC_DEVICE_LAVF) {
				const char *info = reinterpret_cast<lavf_device*>(bc->input)->stream_info();
				log.log(Info, "Stream started: %s", info);
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
				rec->set_logging_context(log);
				bc->source->connect(rec, stream_source::StartFromLastKeyframe);
				std::thread th(&recorder::run, rec);
				th.detach();
			} else if (sched_cur == 'M') {
				m_handler = new motion_handler;
				m_handler->set_logging_context(log);
				m_handler->set_buffer_time(cfg.prerecord, cfg.postrecord);
				bc->source->connect(m_handler->input_consumer(), stream_source::StartFromLastKeyframe);

				rec = new recorder(this);
				rec->set_logging_context(log);
				rec->set_recording_type(BC_EVENT_CAM_T_MOTION);

				rec->set_buffer_time(cfg.prerecord);

				m_handler->connect(rec);
				std::thread rec_th(&recorder::run, rec);
				rec_th.detach();

				if (bc->type == BC_DEVICE_V4L2) {
					static_cast<v4l2_device*>(bc->input)->set_motion(true);
				} else {
					m_processor = new motion_processor;
					m_processor->set_logging_context(log);
					update_motion_thresholds();
					bc->source->connect(m_processor, stream_source::StartFromLastKeyframe);
					m_processor->output()->connect(m_handler->create_flag_consumer());

					m_processor->start_thread();
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
			if (bc->type == BC_DEVICE_LAVF) {
				const char *err = reinterpret_cast<lavf_device*>(bc->input)->get_error_message();
				log.log(Error, "Read error from stream: %s", *err ? err : "Unknown error");
			}

			stop_handle_properly(this);
			/* XXX this should be something other than NOT_FOUND */
			do_error_event(this, BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
			goto error;
		}

		/* End any active error events, because we successfully read a packet */
		if (event) {
			bc_event_cam_end(&event);
			/* Notification hook to PHP: device is back online */
			notify_device_state("BACK ONLINE");
		}

		packet = bc->input->packet();
		bc->source->send(packet);

		/* Send packet to streaming clients */
		if (bc_streaming_is_active(this))
			if (bc_streaming_packet_write(this, packet) == -1) {
				goto error;
			}
		continue;
error:
		sleep(10);
	}

	pthread_cleanup_pop(0);

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
	int ret;
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
	bc_rec->log.set_level(bc_rec->cfg.debug_level ? DEF_TH_LOG_LEVEL : (log_level)-1);

	bc = bc_handle_get(dbres);
	if (!bc) {
		/* XXX should be an event */
		bc_rec->log.log(Error, "Error opening device");
		bc_status_component_error("Error opening device %d", id);
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
		ret  = v4l2->set_control(V4L2_CID_HUE, bc_rec->cfg.hue);
		ret |= v4l2->set_control(V4L2_CID_CONTRAST, bc_rec->cfg.contrast);
		ret |= v4l2->set_control(V4L2_CID_SATURATION, bc_rec->cfg.saturation);
		ret |= v4l2->set_control(V4L2_CID_BRIGHTNESS, bc_rec->cfg.brightness);
		if (ret) {
			bc_status_component_error("Error setting controls on device %d", id);
			delete bc_rec;
			return NULL;
		}
		ret |= v4l2->set_control(V4L2_CID_MPEG_VIDEO_H264_MIN_QP, 100 - bc_rec->cfg.video_quality);
		if (ret)
			bc_rec->log.log(Warning, "Failed to set H264 quantization, please update solo6x10 driver");
	}

	if (pthread_create(&bc_rec->thread, NULL, bc_device_thread,
			   bc_rec) != 0) {
		bc_status_component_error("Failed to start thread");
		delete bc_rec;
		return 0;
	}

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

	if (bc && bc->type == BC_DEVICE_V4L2)
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

void bc_record::getStatusXml(pugi::xml_node &xmlnode)
{
	xmlnode.append_attribute("id") = id;
	// bc_handle
	pugi::xml_node bc_hdl = xmlnode.append_child("bc_handle");
	bc_hdl.append_attribute("device") = bc->device;
	bc_hdl.append_attribute("driver") = bc->driver;
	bc_hdl.append_attribute("mjpeg_url") = bc->mjpeg_url;
	bc_hdl.append_attribute("ptz_path") = bc->ptz_path;

	// bc_device_config
	pugi::xml_node bc_dev_cfg = xmlnode.append_child("bc_device_config");
	bc_dev_cfg.append_attribute("name") = cfg.name;
	bc_dev_cfg.append_attribute("dev") = cfg.dev;
	bc_dev_cfg.append_attribute("driver") = cfg.driver;
	bc_dev_cfg.append_attribute("rtsp_username") = cfg.rtsp_username;
	bc_dev_cfg.append_attribute("rtsp_password") = cfg.rtsp_password;
	bc_dev_cfg.append_attribute("rtsp_rtp_prefer_tcp") = cfg.rtsp_rtp_prefer_tcp;
	bc_dev_cfg.append_attribute("signal_type") = cfg.signal_type;
	bc_dev_cfg.append_attribute("motion_map") = cfg.motion_map;
	bc_dev_cfg.append_attribute("schedule") = cfg.schedule;
	bc_dev_cfg.append_attribute("width") = cfg.width;
	bc_dev_cfg.append_attribute("height") = cfg.height;
	bc_dev_cfg.append_attribute("interval") = cfg.interval;
	bc_dev_cfg.append_attribute("prerecord") = cfg.prerecord;
	bc_dev_cfg.append_attribute("postrecord") = cfg.postrecord;
	bc_dev_cfg.append_attribute("debug_level") = cfg.debug_level;
	bc_dev_cfg.append_attribute("aud_disabled") = cfg.aud_disabled;
	bc_dev_cfg.append_attribute("schedule_override_global") = cfg.schedule_override_global;
	bc_dev_cfg.append_attribute("hue") = cfg.hue;
	bc_dev_cfg.append_attribute("saturation") = cfg.saturation;
	bc_dev_cfg.append_attribute("contrast") = cfg.contrast;
	bc_dev_cfg.append_attribute("brightness") = cfg.brightness;

	// input info
	pugi::xml_node input = xmlnode.append_child("Input");
	bc->input->getStatusXml(input);

#if 0
	// Current event
	if (event) {
		// TODO FIXME Unguarded access, may crash if managing thread releases it meanwhile
		// Fixing this would take enormous time without making order in objects implementation
		// TODO Implement sane C++ classes for all entities.
		pugi::xml_node event_node = xmlnode.append_child("Event");
		event_node.append_attribute("id") = event->id;
		event_node.append_attribute("level") = (int)event->level;
		event_node.append_attribute("type") = (int)event->type;
		event_node.append_attribute("start_time") = (unsigned int)event->start_time;
		event_node.append_attribute("end_time") = (unsigned int)event->end_time;
		event_node.append_attribute("inserted") = (unsigned int)event->inserted;

		event_node.append_attribute("media.table_id") = (unsigned int)event->media.table_id;
		event_node.append_attribute("media.filepath") = event->media.filepath;
		event_node.append_attribute("media.bytes") = (unsigned int)event->media.bytes;
	}
#endif

	// Schedule
	xmlnode.append_attribute("sched_cur") = sched_cur;
	xmlnode.append_attribute("sched_last") = sched_last;
}

int bc_record_update_cfg(struct bc_record *bc_rec, BC_DB_RES dbres)
{
	struct bc_device_config cfg_tmp;
	memset(&cfg_tmp, 0, sizeof(cfg_tmp));

	if (bc_db_get_val_int(dbres, "disabled") > 0) {
		pthread_mutex_lock(&bc_rec->cfg_mutex);
		bc_rec->thread_should_die = "Disabled in config";
		pthread_mutex_unlock(&bc_rec->cfg_mutex);
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
	int ret;

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
	                        current->brightness != update->brightness
							|| current->video_quality != update->video_quality);
	bool mrecord_changed = (current->prerecord != update->prerecord || current->postrecord != update->postrecord);
	bool debug_changed = (current->debug_level != update->debug_level);

	memcpy(current, update, sizeof(struct bc_device_config));
	bc_rec->cfg_dirty = 0;
	pthread_mutex_unlock(&bc_rec->cfg_mutex);

	if (debug_changed) {
		bc_rec->log.set_level(current->debug_level ? DEF_TH_LOG_LEVEL : (log_level)-1);
		bc_rec->log.log(Info, "Device debug level changed to %d", current->debug_level);
	}

	if (format_changed) {
		stop_handle_properly(bc_rec);
		try_formats(bc_rec);
	}

	if (control_changed) {
		v4l2_device *v4l2 = static_cast<v4l2_device*>(bc_rec->bc->input);
		ret  = v4l2->set_control(V4L2_CID_HUE, current->hue);
		ret |= v4l2->set_control(V4L2_CID_CONTRAST, current->contrast);
		ret |= v4l2->set_control(V4L2_CID_SATURATION, current->saturation);
		ret |= v4l2->set_control(V4L2_CID_BRIGHTNESS, current->brightness);
		if (ret)
			return -1;
		ret |= v4l2->set_control(V4L2_CID_MPEG_VIDEO_H264_MIN_QP, 100 - current->video_quality);
		if (ret)
			bc_rec->log.log(Warning, "Failed to set H264 quantization, please update solo6x10 driver");
	}

	if (motion_map_changed)
		bc_rec->update_motion_thresholds();

	if (mrecord_changed)
		bc_rec->m_handler->set_buffer_time(bc_rec->cfg.prerecord, bc_rec->cfg.postrecord);

	check_schedule(bc_rec);
	return 0;
}

