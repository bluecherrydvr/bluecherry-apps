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

#include "bc-list.h"
#include "g723-dec.h"

#define BC_FILE_REC_BASE	"/var/lib/bluecherry/recordings/"

struct bc_record {
	struct bc_handle	*bc;
	AVOutputFormat		*fmt_out;
	AVStream		*video_st;
	AVStream		*audio_st;
	AVFormatContext		*oc;
	snd_pcm_t		*pcm;
	struct g723_state	g723_state;
	int			snd_err;
	char			outfile[PATH_MAX];
	int			id;
	char			*name;
	char			*dev;
	int			mot_cnt;
	pthread_t		thread;
	pthread_mutex_t		lock;
	struct bc_list_struct	list;
};

int bc_vid_out(struct bc_record *bc_rec);
int bc_aud_out(struct bc_record *bc_rec);

void bc_alsa_close(struct bc_record *bc_rec);
int bc_alsa_open(struct bc_record *bc_rec);

void bc_close_avcodec(struct bc_record *bc_rec);
int bc_open_avcodec(struct bc_record *bc_rec);

int bc_start_record(struct bc_record *bc_rec, char **rows, int ncols, int row);

#endif /* __BC_SERVER_H */
