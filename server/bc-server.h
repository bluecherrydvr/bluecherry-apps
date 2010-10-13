/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __BC_SERVER_H
#define __BC_SERVER_H

#include <pthread.h>

#include <libbluecherry.h>
#include <libavformat/avformat.h>
#include <alsa/asoundlib.h>

#include "g723-dec.h"

#define BC_FILE_REC_BASE	"/var/lib/bluecherry/recordings/"

struct bc_record {
	struct bc_handle	*bc;
	AVOutputFormat		*fmt_out;
	AVStream		*video_st;
	AVStream		*audio_st;
	AVFormatContext		*oc;
	snd_pcm_t		*pcm;

	/* mp2 encoding accounting */
	short			pcm_buf[4096];
	int			pcm_buf_size;
	struct g723_state	g723_state;

	int			snd_err;
	char			outfile[PATH_MAX];
	int			id;
	char			*name;
	char			*dev;
	int			mot_cnt;
	pthread_t		thread;
	struct bc_list_struct	list;

	/* Audio setup */
	char			*aud_dev;
	int			aud_rate;
	int			aud_channels;
	unsigned int		aud_format;

	/* Video setup */
	int			vid_interval;

	/* Event/Media handling */
	bc_media_entry_t	media;
	bc_event_cam_t		event;
};

/* Types for aud_format */
#define AUD_FMT_PCM_U8		0x00000001
#define AUD_FMT_PCM_S8		0x00000002
#define AUD_FMT_PCM_U16_LE	0x00000004
#define AUD_FMT_PCM_S16_LE	0x00000008
#define AUD_FMT_PCM_U16_BE	0x00000010
#define AUD_FMT_PCM_S16_BE	0x00000020

/* Flags for aud_format */
#define AUD_FMT_FLAG_G723_24	0x01000000

extern pthread_mutex_t av_lock;

int bc_vid_out(struct bc_record *bc_rec);
int bc_aud_out(struct bc_record *bc_rec);

void bc_close_avcodec(struct bc_record *bc_rec);
int bc_open_avcodec(struct bc_record *bc_rec);

int bc_start_record(struct bc_record *bc_rec, char **rows, int ncols, int row);

#endif /* __BC_SERVER_H */
