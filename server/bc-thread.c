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

static void try_formats(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;

	if (bc_set_interval(bc, bc_rec->interval)) {
		bc_rec->reset_vid = 1;
		if (errno != EAGAIN)
			bc_dev_warn(bc_rec, "Failed to set video interval: %m");
	}

	if (bc_set_format(bc, bc_rec->fmt, bc_rec->width, bc_rec->height)) {
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
	bc_set_osd(bc, "%s %s", bc_rec->name, buf);
}

static int process_schedule(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	int ret = 0;

	pthread_mutex_lock(&bc_rec->sched_mutex);

	if (bc_rec->reset_vid ||
	    (bc_media_length(&bc_rec->media) > BC_MAX_RECORD_TIME &&
	    !bc_rec->sched_last)) {
		bc_close_avcodec(bc_rec);
		bc_handle_stop(bc);
		bc_rec->reset_vid = 0;
		try_formats(bc_rec);
		if (bc_rec->reset_vid)
			ret = 1;
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

	pthread_mutex_unlock(&bc_rec->sched_mutex);

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

	for (;;) {
		if (bc_rec->thread_should_die)
			break;

		update_osd(bc_rec);

		if (process_schedule(bc_rec))
			continue;

		if (bc_handle_start(bc)) {
			if (!(bc_rec->start_failed++ % 60)) {
				int saved_err = errno;

				bc_event_cam(bc_rec->id, BC_EVENT_L_ALRM,
					     BC_EVENT_CAM_T_NOT_FOUND);
				bc_dev_err(bc_rec, "Error starting stream: %s",
					   strerror(saved_err));
			}
			sleep(1);
			continue;
		} else if (bc_rec->start_failed) {
			bc_rec->start_failed = 0;
			bc_dev_info(bc_rec, "Device started after failure(s)");
		}

		ret = bc_buf_get(bc);
		if (ret == EAGAIN) {
			continue;
		} else if (ret == ERESTART) {
			bc_close_avcodec(bc_rec);
			continue;
		} else if (ret) {
			/* Try restarting the connection. Do not report failure
			 * here. It will get reported if we fail to reconnect. */
			bc_close_avcodec(bc_rec);
			bc_handle_stop(bc);
			continue;
		}

		/* Do this after we get the first frame, since we need that
		 * in order to decode the first frame for avcodec params
		 * like width, height and frame rate. */
		if (bc_open_avcodec(bc_rec)) {
			bc_dev_err(bc_rec, "Error opening avcodec");
			continue;
		}

		while (!bc_aud_out(bc_rec));
			/* Do nothing */;

		if (bc_vid_out(bc_rec)) {
			bc_dev_err(bc_rec, "Error writing frame to outfile: %m");
			/* XXX Do something */
		}
	}

	bc_close_avcodec(bc_rec);
	bc_set_osd(bc, " ");

	return bc_rec->thread_should_die;
}

static void get_aud_dev(struct bc_record *bc_rec)
{
	bc_rec->aud_dev[0] = '\0';

	if (!(bc_rec->bc->cam_caps & BC_CAM_CAP_SOLO))
		return;

	sprintf(bc_rec->aud_dev, "hw:CARD=Softlogic%d,DEV=0,SUBDEV=%d",
		bc_rec->bc->card_id, bc_rec->bc->dev_id);

	bc_rec->aud_rate = 8000;
	bc_rec->aud_channels = 1;
	bc_rec->aud_format = AUD_FMT_PCM_U8 | AUD_FMT_FLAG_G723_24;
}

struct bc_record *bc_alloc_record(int id, BC_DB_RES dbres)
{
	struct bc_handle *bc = NULL;
	struct bc_record *bc_rec;
	const char *dev = bc_db_get_val(dbres, "device");
	const char *name = bc_db_get_val(dbres, "device_name");
	const char *driver = bc_db_get_val(dbres, "driver");

	if (bc_db_get_val_bool(dbres, "disabled"))
		return NULL;

	if (!dev || !name || !driver) {
		bc_log("E(%d): Could not get info about device from db", id);
		return NULL;
	}

	bc_rec = malloc(sizeof(*bc_rec));
	if (bc_rec == NULL) {
		bc_log("E(%d/%s): Out of memory trying to start record", id, name);
		return NULL;
	}
	memset(bc_rec, 0, sizeof(*bc_rec));
	memset(bc_rec->motion_map, '3', 22 * 18);

	pthread_mutex_init(&bc_rec->sched_mutex, NULL);

	bc_rec->id = id;
	strcpy(bc_rec->dev, dev);
	strcpy(bc_rec->name, name);
	strcpy(bc_rec->driver, driver);

	bc = bc_handle_get(dbres);
	if (bc == NULL) {
		bc_dev_err(bc_rec, "Error opening device: %m");
		free(bc_rec);
		return NULL;
	}

	bc->__data = bc_rec;
	bc_rec->bc = bc;

	if (!strcasecmp(driver, "solo6110"))
		bc_rec->codec_id = CODEC_ID_H264;
	else if (!strcasecmp(driver, "solo6010"))
		bc_rec->codec_id = CODEC_ID_MPEG4;
	else
		bc_rec->codec_id = CODEC_ID_NONE;

	get_aud_dev(bc_rec);

	bc_rec->sched_cur = 'N';
	bc_update_record(bc_rec, dbres);

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

static void check_motion_map(struct bc_record *bc_rec,
			     const char *signal,
			     const char *motion_map)
{
	int vh;
	struct bc_handle *bc = bc_rec->bc;
	int i;

	if (signal == NULL)
		return;

	vh = strcasecmp(signal, "NTSC") ? 18 : 15;

	if (!motion_map) {
		for (i = 0; i < vh; i++) {
			int j;
			for (j = 0; j < 22; j++) {
				int pos = (i * 22) + j;
				int val = bc_motion_val(BC_MOTION_TYPE_SOLO, '3');
				if (bc_rec->motion_map[pos] == '3')
					continue;
				bc_set_motion_thresh(bc, val, (i * 32) + j);
				bc_rec->motion_map[pos] = '3';
			}
		}
		return;
	}

	if (strlen(motion_map) < 22 * vh) {
		bc_dev_warn(bc_rec, "Motion map is too short");
		return;
	}

	if (!memcmp(bc_rec->motion_map, motion_map, strlen(motion_map)))
		return;

	for (i = 0; i < vh; i++) {
		int j;
		for (j = 0; j < 22; j++) {
			int pos = (i * 22) + j;
			int val = bc_motion_val(BC_MOTION_TYPE_SOLO,
						motion_map[pos]);
			if (bc_rec->motion_map[pos] == motion_map[pos])
				continue;
			bc_set_motion_thresh(bc, val, (i * 32) + j);
			bc_rec->motion_map[pos] = motion_map[pos];
		}
	}
}

static void check_schedule(struct bc_record *bc_rec, const char *sched)
{
	time_t t;;
	struct tm tm;
	char new;

	pthread_mutex_lock(&bc_rec->sched_mutex);

	time(&t);
	localtime_r(&t, &tm);

	new = sched[tm.tm_hour + (tm.tm_wday * 24)];
	if (bc_rec->sched_cur != new) {
		if (!bc_rec->sched_last)
			bc_rec->sched_last = bc_rec->sched_cur;
		bc_rec->sched_cur = new;
	}

	pthread_mutex_unlock(&bc_rec->sched_mutex);
}

void bc_update_record(struct bc_record *bc_rec, BC_DB_RES dbres)
{
	struct bc_handle *bc = bc_rec->bc;
	const char *sched;
	const char *name = bc_db_get_val(dbres, "device_name");

	if (bc_db_get_val_int(dbres, "disabled") > 0) {
		bc_rec->thread_should_die = "Disabled in config";
		return;
	}

	if (strcmp(name, bc_rec->name))
		strcpy(bc_rec->name, name);

	bc_rec->interval = bc_db_get_val_int(dbres, "video_interval");
	bc_rec->width = bc_db_get_val_int(dbres, "resolutionX");
	bc_rec->height = bc_db_get_val_int(dbres, "resolutionY");
	bc_rec->fmt = V4L2_PIX_FMT_MPEG;

	try_formats(bc_rec);

	/* Set the motion threshold blocks */
	check_motion_map(bc_rec, bc_db_get_val(dbres, "signal_type"),
			 bc_db_get_val(dbres, "motion_map"));

	/* Update standard controls */
	bc_set_control(bc, V4L2_CID_HUE,
			bc_db_get_val_int(dbres, "hue"));
	bc_set_control(bc, V4L2_CID_CONTRAST,
			bc_db_get_val_int(dbres, "contrast"));
	bc_set_control(bc, V4L2_CID_SATURATION,
			bc_db_get_val_int(dbres, "saturation"));
	bc_set_control(bc, V4L2_CID_BRIGHTNESS,
			bc_db_get_val_int(dbres, "brightness"));

	if (bc_db_get_val_int(dbres, "schedule_override_global") > 0)
		sched = bc_db_get_val(dbres, "schedule");
	else
		sched = global_sched;

	check_schedule(bc_rec, sched);

	return;
}
