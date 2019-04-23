/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <thread>

#include "bt.h"

#include "bc-server.h"
#include "lavf_device.h"
#include "v4l2_device_solo6x10.h"
#include "v4l2_device_solo6010-dkms.h"
#include "v4l2_device_tw5864.h"
#include "stream_elements.h"
#include "motion_processor.h"
#include "trigger_processor.h"
#include "motion_handler.h"
#include "recorder.h"

#define DEF_TH_LOG_LEVEL Warning

/* For Ubuntu Lucid */
#ifndef V4L2_CID_MPEG_VIDEO_H264_MIN_QP
#define V4L2_CID_MPEG_VIDEO_H264_MIN_QP (V4L2_CID_MPEG_BASE+353)
#endif

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

static void update_osd(struct bc_record *bc_rec)
{
#define OSD_TEXT_MAX 44
#define DATE_LEN 19
#define OSD_NAME_MAX_LEN (OSD_TEXT_MAX - DATE_LEN - 1 /* space */)
	input_device *d = bc_rec->bc->input;
	time_t t = time(NULL);
	char buf[DATE_LEN + 1 /* for null-termination */];
	char name_buf[OSD_NAME_MAX_LEN + 1 /* for null-termination */];
	struct tm tm;

	if (!(d->caps() & BC_CAM_CAP_OSD))
		return;

	if (t == bc_rec->osd_time)
		return;

	bc_rec->osd_time = t;

	tzset();

	strftime(buf, sizeof(buf), "%F %T", localtime_r(&t, &tm));
	/* Will trunkate if longer than OSD_NAME_MAX_LEN */
	snprintf(name_buf, sizeof(name_buf), "%s", bc_rec->cfg.name);
	d->set_osd("%s %s", name_buf, buf);
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
			char *sched_str;
			switch (sched_cur) {
				case 'C': sched_str = "continuous"; break;
				case 'M': sched_str = "motion"; break;
				case 'N': sched_str = "stopped"; break;
				case 'T': sched_str = "triggered"; break;
				default:
					log.log(Error, "Invalid schedule type '%c'", sched_cur);
					goto error;
			}

			log.log(Info, "Switching to new recording schedule '%s'", sched_str);

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
				m_handler->set_motion_analysis_ssw_length(cfg.motion_analysis_ssw_length);
				m_handler->set_motion_analysis_percentage(cfg.motion_analysis_percentage);

				rec = new recorder(this);
				rec->set_logging_context(log);
				rec->set_recording_type(BC_EVENT_CAM_T_MOTION);

				rec->set_buffer_time(cfg.prerecord);

				m_handler->connect(rec);
				std::thread rec_th(&recorder::run, rec);
				rec_th.detach();

				if (bc->input->caps() & BC_CAM_CAP_V4L2_MOTION) {
					bc->input->set_motion(true);
					bc->source->connect(m_handler->input_consumer(), stream_source::StartFromLastKeyframe);
				} else {
					m_processor = new motion_processor;
					m_processor->set_logging_context(log);
					update_motion_thresholds();

					m_processor->set_motion_algorithm(cfg.motion_algorithm);
					m_processor->set_frame_downscale_factor(cfg.motion_frame_downscale_factor);
					m_processor->set_min_motion_area_percent(cfg.min_motion_area);

					bc->source->connect(m_processor, stream_source::StartFromLastKeyframe);
					m_processor->output()->connect(m_handler->input_consumer());
					m_processor->start_thread();
				}

				std::thread th(&motion_handler::run, m_handler);
				th.detach();
			} else if (sched_cur == 'T') {
				/*
				 * source ==> trigger_processor (data passthru, flag setting) ==> motion_handler_m ==> recorder
				 */
				m_handler = new motion_handler;
				m_handler->set_logging_context(log);
				m_handler->set_buffer_time(cfg.prerecord, cfg.postrecord);

				rec = new recorder(this);
				rec->set_logging_context(log);
				rec->set_recording_type(BC_EVENT_CAM_T_MOTION);

				rec->set_buffer_time(cfg.prerecord);

				m_handler->connect(rec);
				std::thread rec_th(&recorder::run, rec);
				rec_th.detach();

				t_processor = new trigger_processor(id);
				t_processor->set_logging_context(log);
				bc->source->connect(t_processor, stream_source::StartFromLastKeyframe);
				t_processor->output()->connect(m_handler->input_consumer());

				std::thread th(&motion_handler::run, m_handler);
				th.detach();
			}

			if (cfg.reencode_enabled) {
				reenc = new reencoder(cfg.reencode_bitrate, cfg.reencode_frame_width, cfg.reencode_frame_height);
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

		/* Reencode packet for live streaming here */
		if (bc_streaming_is_active(this) && reenc && packet.type == AVMEDIA_TYPE_VIDEO) {
			if (reenc->push_packet(packet) && reenc->run_loop()) {
				bc_log(Debug, "got reencoded packet!");
				packet = reenc->packet();
			}
			else
				continue;
		}

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

	bc->input->set_osd(" ");
}


bc_record::bc_record(int i)
	: id(i), log("%d", i)
{
	bc = 0;

	memset(&cfg, 0, sizeof(cfg));
	memset(&cfg_update, 0, sizeof(cfg_update));
	cfg_dirty = 0;
	pthread_mutex_init(&cfg_mutex, NULL);

	stream_ctx[0] = 0;
	stream_ctx[1] = 0;
	rtsp_stream = 0;

	osd_time = 0;
	start_failed = 0;

	memset(&event, 0, sizeof(event));

	sched_cur = 'N';
	sched_last = 0;
	thread_should_die = 0;
	file_started = 0;

	m_processor = 0;
	t_processor = 0;
	m_handler = 0;
	rec = 0;
	reenc = 0;
}

bc_record *bc_record::create_from_db(int id, BC_DB_RES dbres)
{
	int ret;
	bc_record *bc_rec;
	struct bc_handle *bc = NULL;
	const char *state = "CONFIGURATION ERROR";

	if (bc_db_get_val_bool(dbres, "disabled"))
		return 0;

	bc_rec = new bc_record(id);

	if (bc_device_config_init(&bc_rec->cfg, dbres)) {
		bc_status_component_error("Database error while initializing device %d", id);
		goto fail;
	}
	memcpy(&bc_rec->cfg_update, &bc_rec->cfg, sizeof(bc_rec->cfg));

	bc_rec->log = log_context("%d/%s", id, bc_rec->cfg.name);
	bc_rec->log.set_level(bc_rec->cfg.debug_level ? DEF_TH_LOG_LEVEL : (log_level)-1);

	bc = bc_handle_get(dbres);
	if (!bc) {
		/* XXX should be an event */
		bc_rec->log.log(Error, "Error opening device");
		bc_status_component_error("Error opening device %d", id);
		state = "OPENING ERROR";
		goto fail;
	}

	bc->__data = bc_rec;
	bc_rec->bc = bc;

	bc->input->set_audio_enabled(!bc_rec->cfg.aud_disabled);

	/* Initialize device state */
	if (bc_rec->bc->input->set_resolution(bc_rec->cfg.width, bc_rec->cfg.height, bc_rec->cfg.interval))
		bc_rec->log.log(Warning, "Error setting format");
	bc_rec->update_motion_thresholds();
	check_schedule(bc_rec);


	/* The following operations are effective only for some V4L2 devices */
	ret  = bc->input->set_control(V4L2_CID_HUE, bc_rec->cfg.hue);
	ret |= bc->input->set_control(V4L2_CID_CONTRAST, bc_rec->cfg.contrast);
	ret |= bc->input->set_control(V4L2_CID_SATURATION, bc_rec->cfg.saturation);
	ret |= bc->input->set_control(V4L2_CID_BRIGHTNESS, bc_rec->cfg.brightness);
	if (ret) {
		bc_status_component_error("Error setting controls on device %d", id);
		goto fail;
	}
	ret |= bc->input->set_control(V4L2_CID_MPEG_VIDEO_H264_MIN_QP, 100 - bc_rec->cfg.video_quality);
	if (ret)
		bc_rec->log.log(Warning, "Failed to set H264 quantization, please update solo6x10 driver");


	/* Start device processing thread */
	if (pthread_create(&bc_rec->thread, NULL, bc_device_thread,
			   bc_rec) != 0) {
		bc_status_component_error("Failed to start thread");
		state = "INTERNAL ERROR";
		goto fail;
	}

	return bc_rec;

fail:
	bc_rec->notify_device_state(state);
	delete bc_rec;
	return NULL;
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

	if (t_processor) {
		t_processor->disconnect();
		t_processor->destroy();
		t_processor = 0;
	}

	if (m_handler) {
		m_handler->disconnect();
		m_handler->destroy();
		m_handler = 0;
	}

	if (bc)
		bc->input->set_motion(false);

	if (reenc) {
		delete reenc;
		reenc = 0;
	}
}

bool bc_record::update_motion_thresholds()
{
	bool re = false;

	if (m_processor) {
		if (m_processor->set_motion_thresh(cfg.motion_map, strlen(cfg.motion_map)))
			log.log(Warning, "Cannot set motion thresholds (wrong configuration field?)");
		else
			re = true;
	} else {
		if (bc->input->set_motion_thresh(cfg.motion_map, strlen(cfg.motion_map)))
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
	    current->reencode_enabled != update->reencode_enabled ||
	    current->reencode_bitrate != update->reencode_bitrate ||
	    current->reencode_frame_width != update->reencode_frame_width ||
	    current->reencode_frame_height != update->reencode_frame_height ||
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
	bool mrecord_changed = (current->prerecord != update->prerecord || current->postrecord != update->postrecord
			|| current->motion_analysis_ssw_length != update->motion_analysis_ssw_length
			|| current->motion_analysis_percentage != update->motion_analysis_percentage);
	bool debug_changed = (current->debug_level != update->debug_level);

	bool motion_config_changed = (current->motion_algorithm != update->motion_algorithm
			|| abs(current->motion_frame_downscale_factor - update->motion_frame_downscale_factor) > 0.0625
			|| current->min_motion_area != update->min_motion_area);

	memcpy(current, update, sizeof(struct bc_device_config));
	bc_rec->cfg_dirty = 0;
	pthread_mutex_unlock(&bc_rec->cfg_mutex);

	if (debug_changed) {
		bc_rec->log.set_level(current->debug_level ? DEF_TH_LOG_LEVEL : (log_level)-1);
		bc_rec->log.log(Info, "Device debug level changed to %d", current->debug_level);
	}

	if (format_changed) {
		stop_handle_properly(bc_rec);
		if (bc_rec->bc->input->set_resolution(bc_rec->cfg.width, bc_rec->cfg.height, bc_rec->cfg.interval))
			bc_rec->log.log(Warning, "Error setting format");
	}

	if (control_changed) {
		ret  = bc_rec->bc->input->set_control(V4L2_CID_HUE, current->hue);
		ret |= bc_rec->bc->input->set_control(V4L2_CID_CONTRAST, current->contrast);
		ret |= bc_rec->bc->input->set_control(V4L2_CID_SATURATION, current->saturation);
		ret |= bc_rec->bc->input->set_control(V4L2_CID_BRIGHTNESS, current->brightness);
		if (ret)
			return -1;
		ret |= bc_rec->bc->input->set_control(V4L2_CID_MPEG_VIDEO_H264_MIN_QP, 100 - current->video_quality);
		if (ret)
			bc_rec->log.log(Warning, "Failed to set H264 quantization, please update solo6x10 driver");
	}

	if (motion_map_changed)
		bc_rec->update_motion_thresholds();

	if (motion_config_changed && bc_rec->m_processor) {
		bc_rec->m_processor->set_motion_algorithm(bc_rec->cfg.motion_algorithm);
		bc_rec->m_processor->set_frame_downscale_factor(bc_rec->cfg.motion_frame_downscale_factor);
		bc_rec->m_processor->set_min_motion_area_percent(bc_rec->cfg.min_motion_area);
	}

	if (mrecord_changed && bc_rec->m_handler) {
		bc_rec->m_handler->set_buffer_time(bc_rec->cfg.prerecord, bc_rec->cfg.postrecord);
		bc_rec->m_handler->set_motion_analysis_ssw_length(bc_rec->cfg.motion_analysis_ssw_length);
		bc_rec->m_handler->set_motion_analysis_percentage(bc_rec->cfg.motion_analysis_percentage);
	}

	check_schedule(bc_rec);
	return 0;
}

