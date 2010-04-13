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

#include "list.h"

#define BC_FILE_REC_BASE	"/var/lib/bluecherry/recordings/"

struct bc_rec {
	struct bc_handle	*bc;
	AVOutputFormat		*fmt_out;
	AVStream		*video_st;
	AVFormatContext		*oc;
	char			outfile[PATH_MAX];
	int			id;
	char			*name;
	char			*dev;
	int			width;
	int			height;
	int			mot_cnt;
	int			mot_run_off;
	pthread_t		thread;
	struct list_head	list;
};

int bc_mux_out(struct bc_rec *bc_rec);
void bc_close_avcodec(struct bc_rec *bc_rec);
int bc_open_avcodec(struct bc_rec *bc_rec);
int bc_start_record(struct bc_rec *bc_rec);

#endif /* __BC_SERVER_H */
