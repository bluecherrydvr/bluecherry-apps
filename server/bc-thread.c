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

	if (bc_rec->debug_video)
		return;

	if (bc_set_interval(bc, bc_rec->interval)) {
		bc_rec->reset_vid = 1;
		if (errno != EAGAIN)
			bc_log("W(%d): failed to set video interval: %m",
			       bc_rec->id);
	}

	if (bc_set_format(bc, bc_rec->fmt, bc_rec->width, bc_rec->height)) {
		bc_rec->reset_vid = 1;
		if (errno != EAGAIN)
			bc_log("W(%d): error setting format: %m", bc_rec->id);
	}
}

static void update_osd(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	time_t t = time(NULL);
	char buf[20];
	struct tm tm;

	if (bc_rec->debug_video)
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
	    (bc_media_length(&bc_rec->media) > 3600 && !bc_rec->sched_last)) {
		bc_close_avcodec(bc_rec);
		bc_handle_stop(bc);
		bc_rec->reset_vid = 0;
		try_formats(bc_rec);
		if (bc_rec->reset_vid)
			ret = 1;
		bc_log("I(%d): Reset media file", bc_rec->id);
	} else if (bc_rec->sched_last) {
		bc_close_avcodec(bc_rec);
		bc_handle_stop(bc);
		if (bc)
			bc_set_motion(bc, bc_rec->sched_cur == 'M' ? 1 : 0);
		bc_rec->sched_last = 0;
		bc_log("I(%d): Switching to new schedule '%s'", bc_rec->id,
		       bc_rec->sched_cur == 'M' ? "motion" : (bc_rec->sched_cur
		       == 'N' ? "stopped" : "continuous"));
	}

	if (bc_rec->sched_cur == 'N' || bc_rec->debug_video)
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

	bc_log("I(%d): Starting record: %s", bc_rec->id, bc_rec->name);

	for (;;) {
		if (bc_rec->thread_should_die)
			break;

		update_osd(bc_rec);

		if (process_schedule(bc_rec))
			continue;

		if (bc_handle_start(bc)) {
			bc_log("E(%d): error starting stream: %m", bc_rec->id);
			sleep(1);
			continue;
		}

		ret = bc_buf_get(bc);
		if (ret == EAGAIN) {
			continue;
		} else if (ret == ERESTART) {
			bc_close_avcodec(bc_rec);
			continue;
		} else if (ret) {
			bc_log("E(%d): Failed to get vid buf", bc_rec->id);
			continue;
			/* XXX Do something */
		}

		if (bc_open_avcodec(bc_rec)) {
			bc_log("E(%d): error opening avcodec", bc_rec->id);
			continue;
		}

		if (bc_rec->audio_st) {
			double audio_pts, video_pts;
			AVStream *st;

			st = bc_rec->video_st;
			video_pts = (double)st->pts.val * st->time_base.num /
				st->time_base.den;

			st = bc_rec->audio_st;
			audio_pts = (double)st->pts.val * st->time_base.num /
				st->time_base.den;

			while (audio_pts < video_pts) {
				if (bc_aud_out(bc_rec))
					break;

				audio_pts = (double)st->pts.val *
						st->time_base.num /
						st->time_base.den;
			}
		}

		if (bc_vid_out(bc_rec)) {
			bc_log("E(%d): Error writing frame to outfile: %m",
			       bc_rec->id);
			/* XXX Do something */
		}
	}

	bc_close_avcodec(bc_rec);
	bc_set_osd(bc, " ");

	return bc_rec->thread_should_die;
}

struct bc_record *bc_alloc_record(int id, char **rows, int ncols, int row)
{
	struct bc_handle *bc = NULL;
	struct bc_record *bc_rec;
	char *dev = bc_db_get_val(rows, ncols, row, "source_video");
	char *aud_dev = bc_db_get_val(rows, ncols, row, "source_audio_in");
	char *name = bc_db_get_val(rows, ncols, row, "device_name");

	if (!dev || !name)
		return NULL;

	if (bc_db_get_val_int(rows, ncols, row, "disabled") > 0)
		return NULL;

	bc_rec = malloc(sizeof(*bc_rec));
	if (bc_rec == NULL) {
		bc_log("E(%d): out of memory trying to start record", id);
		return NULL;
	}
	memset(bc_rec, 0, sizeof(*bc_rec));
	memset(bc_rec->motion_map, '3', 22 * 18);

	if (!strcmp(dev, FAKE_VIDEO_DEV))
		bc_rec->debug_video = 1;

	pthread_mutex_init(&bc_rec->sched_mutex, NULL);

	if (!bc_rec->debug_video) {
		bc = bc_handle_get(dev);
		if (bc == NULL) {
			bc_log("E(%d): error opening device: %m", bc_rec->id);
			free(bc_rec);
			return NULL;
		}

		bc->__data = bc_rec;

		bc_rec->bc = bc;
	}

	bc_rec->id = id;

	bc_rec->dev = strdup(dev);
	bc_rec->name = strdup(name);

	if (!bc_rec->dev || !bc_rec->name) {
		bc_log("E(%d): out of memory trying to start record", id);
		goto record_fail;
	}

	if (aud_dev) {
		bc_rec->aud_dev = strdup(aud_dev);
		if (!bc_rec->aud_dev) {
			bc_log("E(%d): out of memory trying to start record", id);
			goto record_fail;
		}
		bc_rec->aud_rate = bc_db_get_val_int(rows, ncols, row,
						     "audio_rate");
		bc_rec->aud_channels = bc_db_get_val_int(rows, ncols, row,
							 "audio_channels");
		bc_rec->aud_format = bc_db_get_val_int(rows, ncols, row,
						       "audio_format");
	}

	bc_rec->sched_cur = 'N';
	bc_update_record(bc_rec, rows, ncols, row);

	if (pthread_create(&bc_rec->thread, NULL, bc_device_thread,
			   bc_rec) != 0) {
		bc_log("E(%d): failed to start thread: %m", bc_rec->id);
		goto record_fail;
	}

	return bc_rec;

record_fail:
	if (bc)
		bc_handle_free(bc);

	free(bc_rec->dev);
	free(bc_rec->name);
	free(bc_rec->aud_dev);
	free(bc_rec);

	return NULL;
}

static void check_motion_map(struct bc_record *bc_rec, char *signal,
			     char *motion_map)
{
	int vh = strcasecmp(signal, "NTSC") ? 18 : 15;
	struct bc_handle *bc = bc_rec->bc;
	int i;

	if (!motion_map) {
		for (i = 0; i < vh; i++) {
			int j;
			for (j = 0; j < 22; j++) {
				int pos = (i * 22) + j;
				int val = bc_motion_val(BC_MOTION_TYPE_SOLO, '3');
				if (bc_rec->motion_map[pos] == '3')
					continue;
				bc_set_motion_thresh(bc, val, (i * 64) + j);
				bc_rec->motion_map[pos] = '3';
			}
		}
		return;
	}

	if (strlen(motion_map) < 22 * vh) {
		bc_log("W(%d): motion map is too short", bc_rec->id);
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
			bc_set_motion_thresh(bc, val, (i * 64) + j);
			bc_rec->motion_map[pos] = motion_map[pos];
		}
	}
}

static void check_schedule(struct bc_record *bc_rec, char *sched)
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

void bc_update_record(struct bc_record *bc_rec, char **rows, int ncols, int row)
{
	struct bc_handle *bc = bc_rec->bc;
	char *sched;

	if (bc_db_get_val_int(rows, ncols, row, "disabled") > 0) {
		bc_rec->thread_should_die = "Disabled in config";
		return;
	}

	bc_rec->interval = bc_db_get_val_int(rows, ncols, row,
					     "video_interval");
	bc_rec->width = bc_db_get_val_int(rows, ncols, row, "resolutionX");
	bc_rec->height = bc_db_get_val_int(rows, ncols, row, "resolutionY");
	bc_rec->fmt = V4L2_PIX_FMT_MPEG;

	try_formats(bc_rec);

	/* Set the motion threshold blocks */
	check_motion_map(bc_rec, bc_db_get_val(rows, ncols, row, "signal_type"),
			 bc_db_get_val(rows, ncols, row, "motion_map"));

	/* Update standard controls */
	if (!bc_rec->debug_video) {
		bc_set_control(bc, V4L2_CID_HUE,
				bc_db_get_val_int(rows, ncols, row, "hue"));
		bc_set_control(bc, V4L2_CID_CONTRAST,
				bc_db_get_val_int(rows, ncols, row, "contrast"));
		bc_set_control(bc, V4L2_CID_SATURATION,
				bc_db_get_val_int(rows, ncols, row, "saturation"));
		bc_set_control(bc, V4L2_CID_BRIGHTNESS,
				bc_db_get_val_int(rows, ncols, row, "brightness"));
	}

	if (bc_db_get_val_int(rows, ncols, row, "schedule_override_global") > 0)
		sched = bc_db_get_val(rows, ncols, row, "schedule");
	else
		sched = global_sched;

	check_schedule(bc_rec, sched);

	return;
}
