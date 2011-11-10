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
	char new;

	if (bc_rec->cfg.schedule_override_global)
		schedule = bc_rec->cfg.schedule;

	time(&t);
	localtime_r(&t, &tm);

	new = schedule[tm.tm_hour + (tm.tm_wday * 24)];
	if (bc_rec->sched_cur != new) {
		if (!bc_rec->sched_last)
			bc_rec->sched_last = bc_rec->sched_cur;
		bc_rec->sched_cur = new;
	}
}

static int process_schedule(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	int ret = 0;

	check_schedule(bc_rec);

	if (bc_rec->reset_vid ||
	    (bc_media_length(&bc_rec->media) > BC_MAX_RECORD_TIME &&
	    !bc_rec->sched_last)) {
		if (bc_rec->reset_vid) {
			bc_rec->reset_vid = 0;
			ret = 1;
		}
		bc_close_avcodec(bc_rec);
		bc_handle_reset(bc);
		try_formats(bc_rec);
		bc_dev_info(bc_rec, "Reset media file");
	} else if (bc_rec->sched_last) {
		bc_close_avcodec(bc_rec);
		bc_handle_stop(bc);
		if (bc)
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

static void *bc_device_thread(void *data)
{
	struct bc_record *bc_rec = data;
	struct bc_handle *bc = bc_rec->bc;
	int ret;

	bc_dev_info(bc_rec, "Camera configured");
	bc_av_log_set_handle_thread(bc_rec);

	for (;;) {
		const char *err_msg;

		if (bc_rec->thread_should_die)
			break;

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
				if (!(bc_rec->start_failed++ % 60)) {
					bc_event_cam_fire(bc_rec->id, BC_EVENT_L_ALRM,
							 BC_EVENT_CAM_T_NOT_FOUND);
					bc_dev_err(bc_rec, "Error starting stream: %s",
						   err_msg);
				}
				sleep(1);
				continue;
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
				if (bc_aud_out(bc_rec))
					/* Do nothing */;
				continue;
			}
		}

		ret = bc_buf_get(bc);
		if (!ret) {
			if (!bc_rec->oc && (!bc_buf_key_frame(bc) || !bc_buf_is_video_frame(bc))) {
				/* Always start a recording with a key video frame. This will result in
				 * the lost of some video between recordings, up to the keyframe interval,
				 * at least on RTP devices. XXX Ideally, we should move the recording split
				 * slightly to allow a proper cut for both video and audio. */
				continue;
			}
		
			/* Do this after we get the first frame,
			 * since we need that in order to decode
			 * the first frame for avcodec params like
			 * width, height and frame rate. */
			if (bc_open_avcodec(bc_rec)) {
				bc_dev_err(bc_rec, "Error opening avcodec");
				continue;
			}

			if (bc_buf_is_video_frame(bc)) {
				bc_vid_out(bc_rec);
				/* Error is logged by bc_vid_out. Frame writing errors
				 * are often non-fatal, but can be noisy when repeated.
				 * It would be a good idea to delay-and-restart if enough
				 * of them happen consecutively. */
			} else {
				bc_aud_out(bc_rec);
				/* Do nothing? XXX */
			}
		} else if (ret == ERESTART) {
			bc_close_avcodec(bc_rec);
		} else if (ret != EAGAIN) {
			/* Try restarting the connection. Do not
			 * report failure here. It will get reported
			 * if we fail to reconnect. (XXX what?) */
			
			if (bc->type == BC_DEVICE_RTP) {
				bc_dev_err(bc_rec, "RTSP read error: %s",
				           (*bc->rtp.error_message) ?
				            bc->rtp.error_message :
				            "Unknown error");
			}
			
			bc_close_avcodec(bc_rec);
			bc_handle_stop(bc);
		}
	}

	bc_close_avcodec(bc_rec);
	bc_handle_stop(bc);
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

static void update_motion_map(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	const char *motion_map = bc_rec->cfg.motion_map;
	int vh;
	int i;

	vh = strcasecmp(bc_rec->cfg.signal_type, "NTSC") ? 18 : 15;

#if 0 // Global threshold..
	if (!motion_map) {
		int val = bc_motion_val(BC_MOTION_TYPE_SOLO, '3');
		bc_set_motion_thresh_global(bc, val);

		memset(bc_rec->cfg.motion_map, '3', 22*vh);
		bc_rec->cfg.motion_map[22*vh] = 0;
		return;
	}
#endif

	if (strlen(motion_map) < 22 * vh) {
		bc_dev_warn(bc_rec, "Motion map is too short");
		return;
	}

	/* Our input map is 32x32, but the device is actually 64x64.
	 * Fields are doubled accordingly. */
	for (i = 0; i < (vh*2); i++) {
		int j;
		for (j = 0; j < 44; j++) {
			int pos = ((i/2)*22)+(j/2);
			int val = bc_motion_val(BC_MOTION_TYPE_SOLO, motion_map[pos]);
			bc_set_motion_thresh(bc, val, (i*64)+j);
		}
	}
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
		bc_log("E(%d): Video type mismatch, driver has %s and record wants %s",
		       id, video_type, signal_type);
		return NULL;
	}

	bc_rec = malloc(sizeof(*bc_rec));
	if (bc_rec == NULL) {
		bc_log("E(%d): Out of memory trying to start record", id);
		return NULL;
	}
	memset(bc_rec, 0, sizeof(*bc_rec));

	pthread_mutex_init(&bc_rec->cfg_mutex, NULL);

	if (bc_device_config_init(&bc_rec->cfg, dbres)) {
		bc_log("E(%d): Database error while initializing device", id);
		free(bc_rec);
		return NULL;
	}
	memcpy(&bc_rec->cfg_update, &bc_rec->cfg, sizeof(bc_rec->cfg));

	bc_rec->id = id;
	bc_rec->fmt = V4L2_PIX_FMT_MPEG;

	bc = bc_handle_get(dbres);
	if (bc == NULL) {
		bc_dev_err(bc_rec, "Error opening device: %m");
		free(bc_rec);
		return NULL;
	}

	bc->__data = bc_rec;
	bc_rec->bc = bc;

	if (!strcasecmp(bc_rec->cfg.driver, "solo6110"))
		bc_rec->codec_id = CODEC_ID_H264;
	else if (!strcasecmp(bc_rec->cfg.driver, "solo6010"))
		bc_rec->codec_id = CODEC_ID_MPEG4;
	else
		bc_rec->codec_id = CODEC_ID_NONE;

	get_aud_dev(bc_rec);
	bc_rec->sched_cur = 'N';

	bc->rtp.want_audio = !bc_rec->cfg.aud_disabled;

	/* Initialize device state */
	try_formats(bc_rec);
	update_motion_map(bc_rec);
	check_schedule(bc_rec);

	if (pthread_create(&bc_rec->thread, NULL, bc_device_thread,
			   bc_rec) != 0) {
		bc_dev_err(bc_rec, "Failed to start thread: %m");
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
	struct bc_device_config *new     = &bc_rec->cfg_update;
	int motion_map_changed;

	pthread_mutex_lock(&bc_rec->cfg_mutex);

	bc_dev_info(bc_rec, "Applying configuration changes");

	if (strcmp(current->dev, new->dev) || strcmp(current->driver, new->driver) ||
	    strcmp(current->signal_type, new->signal_type) ||
	    strcmp(current->rtsp_username, new->rtsp_username) ||
	    strcmp(current->rtsp_password, new->rtsp_password) ||
	    current->aud_disabled != new->aud_disabled)
	{
		bc_rec->thread_should_die = "configuration changed";
		pthread_mutex_unlock(&bc_rec->cfg_mutex);
		return -1;
	}

	motion_map_changed = memcmp(current->motion_map, new->motion_map, sizeof(new->motion_map));

	memcpy(current, new, sizeof(struct bc_device_config));
	bc_rec->cfg_dirty = 0;
	pthread_mutex_unlock(&bc_rec->cfg_mutex);

	try_formats(bc_rec);
	if (motion_map_changed)
		update_motion_map(bc_rec);
	check_schedule(bc_rec);

	return 0;
}

