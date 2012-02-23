/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>

#include "bc-server.h"

static int apply_device_cfg(struct bc_record *bc_rec);

static void bc_start_media_entry(struct bc_record *bc_rec, time_t start_ts)
{
	struct tm tm;
	char date[12], mytime[10], dir[PATH_MAX];
	char stor[256];

	bc_get_media_loc(stor);

	/* XXX Need some way to reconcile time between media event and
	 * filename. They should match. */
	localtime_r(&start_ts, &tm);

	strftime(date, sizeof(date), "%Y/%m/%d", &tm);
	strftime(mytime, sizeof(mytime), "%H-%M-%S", &tm);
	sprintf(dir, "%s/%s/%06d", stor, date, bc_rec->id);
	bc_mkdir_recursive(dir);
	sprintf(bc_rec->outfile, "%s/%s.mkv", dir, mytime);
}

static void recording_end(struct bc_record *bc_rec)
{
	/* Close the media entry in the db */
	if (bc_rec->event && bc_event_has_media(bc_rec->event)) {
		bc_event_cam_end(&bc_rec->event);
		if ((bc_rec->sched_cur == 'M' && !bc_rec->sched_last) ||
		    bc_rec->sched_last == 'M')
			bc_dev_info(bc_rec, "Motion event stopped");
	}

	bc_close_avcodec(bc_rec);
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

static int recording_start(struct bc_record *bc_rec, time_t start_ts)
{
	bc_event_cam_t event = NULL;

	if (!start_ts)
		start_ts = time(NULL);

	recording_end(bc_rec);
	/* XXX needs to handle failure */
	bc_start_media_entry(bc_rec, start_ts);

	if (bc_open_avcodec(bc_rec)) {
		do_error_event(bc_rec, BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
		return -1;
	}

	if (bc_rec->sched_cur == 'M') {
		bc_dev_info(bc_rec, "Motion event started");
		event = bc_event_cam_start(bc_rec->id, start_ts, BC_EVENT_L_WARN,
		                           BC_EVENT_CAM_T_MOTION,
		                           bc_rec->outfile);
	} else if (bc_rec->sched_cur == 'C') {
		event = bc_event_cam_start(bc_rec->id, start_ts, BC_EVENT_L_INFO,
		                           BC_EVENT_CAM_T_CONTINUOUS,
		                           bc_rec->outfile);
	}

	if (event == NULL) {
		do_error_event(bc_rec, BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
		return -1;
	}

	bc_event_cam_end(&bc_rec->event);
	bc_rec->event = event;

	/* XXX this should not be recording dependent, but it uses oc at the moment XXX */
	if (bc_streaming_setup(bc_rec))
		bc_dev_err(bc_rec, "Error setting up live stream");


	return 0;
}

static void stop_handle_properly(struct bc_record *bc_rec)
{
	struct bc_output_packet *p, *n;

	recording_end(bc_rec);
	bc_streaming_destroy(bc_rec);
	bc_handle_stop(bc_rec->bc);
	bc_rec->mot_last_ts = 0;
	bc_rec->mot_first_buffered_packet = 0;
	for (p = bc_rec->prerecord_head; p; p = n) {
		n = p->next;
		free(p->data);
		free(p);
	}
	bc_rec->prerecord_head = bc_rec->prerecord_tail = 0;
}

static void try_formats(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;

	if (bc_set_interval(bc, bc_rec->cfg.interval)) {
		bc_rec->reset_vid = 1;
		if (errno != EAGAIN)
			bc_dev_warn(bc_rec, "Failed to set video interval: %m");
	}

	if (bc_set_format(bc, bc_rec->fmt, bc_rec->cfg.width, bc_rec->cfg.height)) {
		bc_rec->reset_vid = 1;
		if (errno != EAGAIN)
			bc_dev_warn(bc_rec, "Error setting format: %m");
	}
}

static void update_osd(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	time_t t = time(NULL);
	char buf[20];
	struct tm tm;

	if (!(bc->cam_caps & BC_CAM_CAP_OSD))
		return;

	if (t == bc_rec->osd_time)
		return;

	bc_rec->osd_time = t;
	strftime(buf, 20, "%F %T", localtime_r(&t, &tm));
	bc_set_osd(bc, "%s %s", bc_rec->cfg.name, buf);
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
	int ret = 0;

	check_schedule(bc_rec);

	if (bc_rec->reset_vid ||
	    (bc_event_media_length(bc_rec->event) > BC_MAX_RECORD_TIME &&
	    !bc_rec->sched_last)) {
		if (bc_rec->reset_vid) {
			bc_rec->reset_vid = 0;
			ret = 1;
		}
		recording_end(bc_rec);
		bc_handle_reset(bc);
		try_formats(bc_rec);
		bc_dev_info(bc_rec, "Reset media file");
	} else if (bc_rec->sched_last) {
		stop_handle_properly(bc_rec);
		bc_event_cam_end(&bc_rec->event);
		bc_set_motion(bc, bc_rec->sched_cur == 'M' ? 1 : 0);
		bc_rec->sched_last = 0;
		bc_dev_info(bc_rec, "Switching to new schedule '%s'",
		       bc_rec->sched_cur == 'M' ? "motion" : (bc_rec->sched_cur
		       == 'N' ? "stopped" : "continuous"));
	}

	if (bc_rec->sched_cur == 'N')
		ret = 1;

	if (ret)
		sleep(1);

	return ret;
}

/* Append a packet to the prerecord buffer, making a deep copy in the process.
 * The buffer will always start with a keyframe, and always be at least
 * bc_rec->cfg.prerecord seconds in duration (walltime, which might not be ideal).
 * Return negative on error, 0 if the packet was dropped, 1 otherwise.
 */
static int prerecord_append(struct bc_record *bc_rec, const struct bc_output_packet *pkt, time_t monotonic_now)
{
	struct bc_output_packet *mpkt = (struct bc_output_packet*) malloc(sizeof(struct bc_output_packet));
	if (bc_output_packet_copy(mpkt, pkt) < 0) {
		free(mpkt);
		return -1;
	}
	mpkt->next = 0;
	mpkt->ts_monotonic = monotonic_now;

	if (mpkt->type == AVMEDIA_TYPE_VIDEO && (mpkt->flags & AV_PKT_FLAG_KEY)) {
		struct bc_output_packet *p, *n;
		struct bc_output_packet *keep_keyframe = 0;

		if (bc_rec->cfg.prerecord > 0) {
			keep_keyframe = bc_rec->prerecord_head;
			for (p = bc_rec->prerecord_head; p; p = p->next) {
				if (p->type != AVMEDIA_TYPE_VIDEO || !(p->flags & AV_PKT_FLAG_KEY))
					continue;
				if (monotonic_now - p->ts_monotonic >= bc_rec->cfg.prerecord)
					keep_keyframe = p;
			}
		}

		for (p = bc_rec->prerecord_head; p && p != keep_keyframe; p = n) {
			n = p->next;
			free(p->data);
			free(p);
		}

		if (!p) {
			bc_rec->prerecord_head = bc_rec->prerecord_tail = mpkt;
			return 1;
		} else
			bc_rec->prerecord_head = p;
	} else if (!bc_rec->prerecord_head) {
		/* Drop this packet (no keyframe in the stream yet) */
		free(mpkt->data);
		free(mpkt);
		return 0;
	}

	/* Append this packet */
	bc_rec->prerecord_tail->next = mpkt;
	bc_rec->prerecord_tail = mpkt;
	return 1;
}

static int check_motion(struct bc_record *bc_rec, const struct bc_output_packet *spkt)
{
	time_t monotonic_now;
	int ret;

	if (bc_rec->sched_cur != 'M')
		return 1;

	/* This may be necessary in some cases when the handle was stopped
	 * (and thus, motion reset to off) without a schedule change. */
	bc_set_motion(bc_rec->bc, 1);

	monotonic_now = bc_gettime_monotonic();

	if (prerecord_append(bc_rec, spkt, monotonic_now) < 0 ||
	    !bc_buf_is_video_frame(bc_rec->bc))
		return bc_rec->mot_last_ts > 0;

	ret = bc_motion_is_detected(bc_rec->bc);

	if (bc_rec->mot_first_buffered_packet) {
		/* Don't end the event and close the recording until we've dropped
		 * a frame out of the prerecording buffer that was not part of
		 * the recording; this allows us to resume the current event if more
		 * motion occurs within [prerecord] seconds. */
		struct bc_output_packet *p = bc_rec->prerecord_head;
		for (; p; p = p->next) {
			if (p == bc_rec->mot_first_buffered_packet)
				break;
		}

		if (!p) {
			/* We exhausted the prerecord buffer without more motion;
			 * end the current recording. If this frame had motion,
			 * we'll start a new recording immediately. */
			recording_end(bc_rec);
			bc_rec->mot_last_ts = 0;
			bc_rec->mot_first_buffered_packet = 0;
		} else if (ret) {
			/* There is new motion; we can resume this recording
			 * using buffered packets. Dump them, excluding this packet. */
			bc_dev_info(bc_rec, "Motion event continued");
			for (; p && p->next; p = p->next)
				bc_output_packet_write(bc_rec, p);
			bc_rec->mot_first_buffered_packet = 0;
		}
	}

	if (ret) {
		/* Motion frame; update the timestamp */
		bc_rec->mot_last_ts = monotonic_now;
	} else if (bc_rec->mot_last_ts) {
		/* Actively recording, but this frame has no motion */
		if (monotonic_now - bc_rec->mot_last_ts <= bc_rec->cfg.postrecord)
			ret = 1; /* Continue recording (postrecord time) */
		else if (!bc_rec->mot_first_buffered_packet) {
			bc_rec->mot_first_buffered_packet = bc_rec->prerecord_tail;
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
	struct bc_output_packet packet;
	int ret;

	bc_dev_info(bc_rec, "Camera configured");
	bc_av_log_set_handle_thread(bc_rec);

	while (!bc_rec->thread_should_die) {
		const char *err_msg;

		/* Set by bc_record_update_cfg */
		if (bc_rec->cfg_dirty) {
			if (apply_device_cfg(bc_rec))
				break;
		}

		update_osd(bc_rec);

		if (process_schedule(bc_rec))
			continue;

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
				bc_dev_info(bc_rec, "RTP stream started: %s",
				            rtp_device_stream_info(&bc->rtp));
			}

		}

		if ((bc_rec->bc->cam_caps & BC_CAM_CAP_SOLO) && has_audio(bc_rec)
		    && bc_rec->oc && bc_rec->audio_st)
		{
			double audio_pts = (double)bc_rec->audio_st->pts.val *
			                   bc_rec->audio_st->time_base.num /
			                   bc_rec->audio_st->time_base.den;
			double video_pts = (double)bc_rec->video_st->pts.val *
			                   bc_rec->video_st->time_base.num /
			                   bc_rec->video_st->time_base.den;

			if (audio_pts < video_pts) {
				if (get_output_audio_packet(bc_rec, &packet) > 0)
					bc_output_packet_write(bc_rec, &packet);
			}
		}

		ret = bc_buf_get(bc);
		if (ret == EAGAIN) {
			continue;
		} else if (ret != 0) {
			if (bc->type == BC_DEVICE_RTP) {
				bc_dev_err(bc_rec, "RTSP read error: %s",
				           (*bc->rtp.error_message) ?
				            bc->rtp.error_message :
				            "Unknown error");
			}

			stop_handle_properly(bc_rec);
			/* XXX this should be something other than NOT_FOUND */
			do_error_event(bc_rec, BC_EVENT_L_ALRM, BC_EVENT_CAM_T_NOT_FOUND);
			goto error;
		}

		/* Prepare output packets; nothing is allocated. */
		if (bc_buf_is_video_frame(bc))
			ret = get_output_video_packet(bc_rec, &packet);
		else
			ret = get_output_audio_packet(bc_rec, &packet);

		/* Error, or no packet */
		if (ret < 1)
			continue;

		/* XXX Requires schedule changes so we can always do this if needed */
		if (bc_streaming_is_active(bc_rec)) {
			bc_streaming_packet_write(bc_rec, &packet);
		}

		if (!check_motion(bc_rec, &packet))
			continue;

		if (!bc_rec->oc) {
			if (bc_rec->sched_cur == 'M' && bc_rec->prerecord_head) {
				/* Dump the prerecording buffer */
				struct bc_output_packet *p;
				if (recording_start(bc_rec, bc_rec->prerecord_head->ts_clock))
					goto error;
				bc_rec->output_pts_base = bc_rec->prerecord_head->pts;
				/* Skip the last packet; it's identical to the current packet,
				 * which we write in the normal path below. */
				for (p = bc_rec->prerecord_head; p && p->next; p = p->next)
					bc_output_packet_write(bc_rec, p);
			} else if (!bc_buf_key_frame(bc) || !bc_buf_is_video_frame(bc)) {
				/* Always start a recording with a key video frame. This will result in
				 * the lost of some video between recordings, up to the keyframe interval,
				 * at least on RTP devices. XXX Ideally, we should move the recording split
				 * slightly to allow a proper cut for both video and audio. */
				continue;
			} else {
				if (recording_start(bc_rec, 0))
					goto error;
				bc_rec->output_pts_base = packet.pts;
			}
		}

		bc_output_packet_write(bc_rec, &packet);
		continue;

error:
		sleep(10);
	}

	stop_handle_properly(bc_rec);
	bc_event_cam_end(&bc_rec->event);
	bc_set_osd(bc, " ");

	return bc_rec->thread_should_die;
}

static void get_aud_dev(struct bc_record *bc_rec)
{
	bc_rec->aud_dev[0] = '\0';

	if (!(bc_rec->bc->cam_caps & BC_CAM_CAP_SOLO))
		return;

	sprintf(bc_rec->aud_dev, "hw:CARD=Softlogic%d,DEV=0,SUBDEV=%d",
		bc_rec->bc->v4l2.card_id, bc_rec->bc->v4l2.dev_id);

	bc_rec->aud_rate = 8000;
	bc_rec->aud_channels = 1;
	bc_rec->aud_format = AUD_FMT_PCM_U8 | AUD_FMT_FLAG_G723_24;
}

struct bc_record *bc_alloc_record(int id, BC_DB_RES dbres)
{
	struct bc_handle *bc = NULL;
	struct bc_record *bc_rec;
	const char *signal_type = bc_db_get_val(dbres, "signal_type", NULL);
	const char *video_type = bc_db_get_val(dbres, "video_type", NULL);

	if (bc_db_get_val_bool(dbres, "disabled"))
		return NULL;

	if (signal_type && video_type && strcasecmp(signal_type, video_type)) {
		bc_status_component_error("Video type mismatch for device %d "
			"(driver is %s, device is %s)", id, video_type, signal_type);
		return NULL;
	}

	bc_rec = (struct bc_record*) malloc(sizeof(*bc_rec));
	if (bc_rec == NULL) {
		bc_log("E(%d): Out of memory trying to start record", id);
		return NULL;
	}
	memset(bc_rec, 0, sizeof(*bc_rec));

	pthread_mutex_init(&bc_rec->cfg_mutex, NULL);

	if (bc_device_config_init(&bc_rec->cfg, dbres)) {
		bc_status_component_error("Database error while initializing device %d", id);
		free(bc_rec);
		return NULL;
	}
	memcpy(&bc_rec->cfg_update, &bc_rec->cfg, sizeof(bc_rec->cfg));

	bc_rec->id = id;
	bc_rec->fmt = V4L2_PIX_FMT_MPEG;

	bc = bc_handle_get(dbres);
	if (bc == NULL) {
		/* XXX should be an event */
		bc_dev_err(bc_rec, "Error opening device: %m");
		bc_status_component_error("Error opening device %d: %m", id);
		free(bc_rec);
		return NULL;
	}

	bc->__data = bc_rec;
	bc_rec->bc = bc;

	get_aud_dev(bc_rec);
	bc_rec->sched_cur = 'N';

	bc->rtp.want_audio = !bc_rec->cfg.aud_disabled;

	/* Initialize device state */
	try_formats(bc_rec);
	if (bc_set_motion_thresh(bc, bc_rec->cfg.motion_map,
	    sizeof(bc_rec->cfg.motion_map)))
	{
		bc_dev_warn(bc_rec, "Cannot set motion thresholds; corrupt configuration?");
	}
	check_schedule(bc_rec);

	if (pthread_create(&bc_rec->thread, NULL, bc_device_thread,
			   bc_rec) != 0) {
		bc_status_component_error("Failed to start thread: %m");
		bc_handle_free(bc);
		free(bc_rec);
		bc_rec = NULL;
	}

	/* Throttle thread starting */
	sleep(1);

	return bc_rec;
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

	/* Update standard controls */
	bc_set_control(bc_rec->bc, V4L2_CID_HUE,
			bc_db_get_val_int(dbres, "hue"));
	bc_set_control(bc_rec->bc, V4L2_CID_CONTRAST,
			bc_db_get_val_int(dbres, "contrast"));
	bc_set_control(bc_rec->bc, V4L2_CID_SATURATION,
			bc_db_get_val_int(dbres, "saturation"));
	bc_set_control(bc_rec->bc, V4L2_CID_BRIGHTNESS,
			bc_db_get_val_int(dbres, "brightness"));

	return 0;
}

static int apply_device_cfg(struct bc_record *bc_rec)
{
	struct bc_device_config *current = &bc_rec->cfg;
	struct bc_device_config *update  = &bc_rec->cfg_update;
	int motion_map_changed;

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

	memcpy(current, update, sizeof(struct bc_device_config));
	bc_rec->cfg_dirty = 0;
	pthread_mutex_unlock(&bc_rec->cfg_mutex);

	try_formats(bc_rec);
	if (motion_map_changed) {
		if (bc_set_motion_thresh(bc_rec->bc, bc_rec->cfg.motion_map,
		    sizeof(bc_rec->cfg.motion_map)))
		{
			bc_dev_warn(bc_rec, "Cannot set motion thresholds; corrupt configuration?");
		}
	}
	check_schedule(bc_rec);

	return 0;
}

