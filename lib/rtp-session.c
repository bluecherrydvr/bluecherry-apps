/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "rtp-session.h"

#define RETERR(__msg) ({		\
	if (err_msg)			\
		*err_msg = __msg;	\
	return -1;			\
})

#define GOTOERR(__msg) ({		\
	if (err_msg)			\
		*err_msg = __msg;	\
	goto setup_fail;		\
})

int rtp_session_init(struct rtp_session *rs, const char *userinfo,
		     const char *uri, const char *server,
		     unsigned int port)
{
	int auth_valid = (userinfo && strlen(userinfo)) ? 1 : 0;

	memset(rs, 0, sizeof(*rs));

	strcpy(rs->userinfo, userinfo);
	strcpy(rs->server, server);
	strcpy(rs->uri, uri);
	rs->port = port;

	if (asprintf(&rs->url, "rtsp://%s%s%s:%d%s", auth_valid ? userinfo : "",
		     auth_valid ? "@" : "", server, port, uri) < 0) {
		rs->url = NULL;
		return -ENOMEM;
	}

	return 0;
}

void rtp_session_stop(struct rtp_session *rs)
{
	if (rs->pkt_vid.data) {
		av_free_packet(&rs->pkt_vid);
		rs->pkt_vid.data = NULL;
	}

	if (rs->fmt_ctx) {
		av_close_input_file(rs->fmt_ctx);
		rs->fmt_ctx = NULL;
	}
}

int rtp_session_start(struct rtp_session *rs, const char **err_msg)
{
	int i;

	*err_msg = NULL;

	if (av_open_input_file(&rs->fmt_ctx, rs->url, NULL, 0, NULL) < 0)
		GOTOERR("Unable to open RTSP stream");

	if (av_find_stream_info(rs->fmt_ctx) < 0)
		GOTOERR("Enable to get RTSP stream information");

	rs->stream_id_vid = rs->stream_id_aud = -1;
	for (i = 0; i < rs->fmt_ctx->nb_streams; i++) {
		if (rs->fmt_ctx->streams[i]->codec->codec_type != CODEC_TYPE_VIDEO)
			continue;
		rs->stream_id_vid = i;
		break;
	}

	if (rs->stream_id_vid == -1)
		GOTOERR("Unable to find a video stream");

	rs->codec_ctx = rs->fmt_ctx->streams[rs->stream_id_vid]->codec;
	rs->pkt_vid.data = NULL;

setup_fail:
	if (*err_msg) {
		rtp_session_stop(rs);
		return -1;
	}

	return 0;
}

int rtp_session_read(struct rtp_session *rs)
{
	if (rs->pkt_vid.data)
		av_free_packet(&rs->pkt_vid);

	if (av_read_frame(rs->fmt_ctx, &rs->pkt_vid) < 0)
		return -1;

	return 0;
}
