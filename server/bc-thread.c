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

static void update_osd(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	char buf[20];
	struct tm tm;
	time_t t;

	t = time(NULL);
	strftime(buf, 20, "%F %T", localtime_r(&t, &tm));
	bc_set_osd(bc, "%s %s", bc_rec->name, buf);
}

static void *bc_device_thread(void *data)
{
	struct bc_record *bc_rec = data;
	struct bc_handle *bc = bc_rec->bc;
	int file_started = 0;
	int ret;

	pthread_mutex_lock(&bc_rec->lock);
	bc_log("I(%d): Starting record: %s", bc_rec->id, bc_rec->name);
	pthread_mutex_unlock(&bc_rec->lock);

	for (;;) {
		double audio_pts, video_pts;

		if ((ret = bc_buf_get(bc)) == EAGAIN) {
			continue;
		} else if (ret == ERESTART) {
			if (file_started) {
				file_started = 0;
				bc_alsa_close(bc_rec);
				bc_close_avcodec(bc_rec);
			}
			continue;
		} else if (ret) {
			bc_log("error getting buffer: %m");
			continue;
			/* XXX Do something */
		}

		if (!file_started) {
			bc_alsa_open(bc_rec);
			if (bc_open_avcodec(bc_rec)) {
				bc_log("E(%d): error opening avcodec: %m",
				       bc_rec->id);
				continue;
			}
			file_started = 1;
		}

		if (bc_buf_key_frame(bc))
			update_osd(bc_rec);

		if (bc_rec->audio_st) {
			AVStream *st;

			st = bc_rec->video_st;
			video_pts = (double)st->pts.val * st->time_base.num /
				st->time_base.den;

			st = bc_rec->audio_st;
			audio_pts = (double)st->pts.val * st->time_base.num /
				st->time_base.den;

			while (audio_pts < video_pts) {
				if (bc_aud_out(bc_rec)) {
					break;
					/* XXX Do something */
				}
				audio_pts = (double)st->pts.val *
						st->time_base.num /
						st->time_base.den;
			}
		}

		if (bc_vid_out(bc_rec)) {
			bc_log("error writing frame to outfile: %m");
			/* XXX Do something */
		}
	}

	if (file_started)
		bc_close_avcodec(bc_rec);

	return NULL;
}

void bc_alsa_close(struct bc_record *bc_rec)
{
	if (!bc_rec->pcm)
		return;

	snd_pcm_close(bc_rec->pcm);
	bc_rec->pcm = NULL;
}

int bc_alsa_open(struct bc_record *bc_rec)
{
	snd_pcm_hw_params_t *params = NULL;
	snd_pcm_t *pcm = NULL;
	char adev[256];
	int err;

	sprintf(adev, "hw:CARD=Softlogic0,DEV=0,SUBDEV=%d", bc_rec->id - 1);
	if ((err = snd_pcm_open(&pcm, adev, SND_PCM_STREAM_CAPTURE, 0)) < 0)
		return -1;

	snd_pcm_hw_params_alloca(&params);

	if (snd_pcm_hw_params_any(pcm, params) < 0)
		return -1;

	bc_rec->pcm = pcm;

	if (snd_pcm_hw_params_set_access(pcm, params,
					 SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
		return -1;

	if (snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_U8) < 0)
		return -1;

	if (snd_pcm_hw_params_set_channels(pcm, params, 1) < 0)
		return -1;

	if (snd_pcm_hw_params_set_rate(pcm, params, 8000, 0) < 0)
		return -1;

	if (snd_pcm_hw_params(pcm, params) < 0)
		return -1;

	if ((err = snd_pcm_prepare(pcm)) < 0)
		return -1;

	bc_rec->snd_err = 0;
	g723_init(&bc_rec->g723_state);

	return 0;
}

int bc_start_record(struct bc_record *bc_rec, char **rows, int ncols, int row)
{
	struct bc_handle *bc;
	int width, height;
	int ret;

	/* Open the device */
	if ((bc = bc_handle_get(bc_rec->dev)) == NULL) {
		bc_log("E(%d): error opening device: %m", bc_rec->id);
		return -1;
	}

	bc_rec->bc = bc;

	/* Set the format */
	width = bc_db_get_val_int(rows, ncols, row, "resolutionX");
	height = bc_db_get_val_int(rows, ncols, row, "resolutionY");

	ret = bc_set_format(bc, V4L2_PIX_FMT_MPEG, width, height);

	if (ret) {
		bc_log("E(%d): error setting format: %m", bc_rec->id);
		bc_handle_free(bc);
		return -1;
	}

	/* Check motion detection */
	ret = bc_db_get_val_bool(rows, ncols, row, "motion_detection_on");
	ret = bc_set_motion(bc, ret);
	if (ret) {
		bc_log("E(%d): error setting motion: %m", bc_rec->id);
		bc_handle_free(bc);
		return -1;
	}

	if (bc_handle_start(bc)) {
		bc_log("E(%d): error starting stream: %m", bc_rec->id);
		bc_handle_free(bc);
		return -1;
	}
 
	if (pthread_create(&bc_rec->thread, NULL, bc_device_thread,
			   bc_rec) != 0) {
		bc_log("E(%d): failed to start thread: %m", bc_rec->id);
		bc_handle_free(bc);
		return -1;
	}

	return 0;
}
