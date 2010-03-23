/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __BC_SERVER_H
#define __BC_SERVER_H

#include <libbluecherry.h>
#include <libavformat/avformat.h>

struct bc_rec {
	struct bc_handle	*bc;
	AVOutputFormat		*fmt_out;
	AVStream		*video_st;
	AVFormatContext		*oc;
	const char		*outfile;
	int			mot_cnt;
	int			mot_run_off;
};

int bc_mux_out(struct bc_rec *bc_rec);
void bc_close_avcodec(struct bc_rec *bc_rec);
int bc_open_avcodec(struct bc_rec *bc_rec);

#endif /* __BC_SERVER_H */
