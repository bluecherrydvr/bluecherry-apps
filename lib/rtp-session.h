/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __RTP_SESSION_H
#define __RTP_SESSION_H

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#define ADTS_HEADER_LENGTH	7

struct rtp_session {
	char		userinfo[256], server[256], uri[256];
	int		port;
	char		*url;
	int		stream_id_vid;
	int		stream_id_aud;

	/* libav shtuff */
	AVFormatContext	*fmt_ctx;
	AVCodecContext	*codec_ctx;
	AVCodecContext	*codec_ctx_aud;
	AVPacket	pkt_vid, pkt_aud;
};

int rtp_session_init(struct rtp_session *rs, const char *userinfo,
		     const char *uri, const char *server,
		     unsigned int port);
void rtp_session_stop(struct rtp_session *rs);
int rtp_session_start(struct rtp_session *rs, const char **err_msg);
int rtp_session_read(struct rtp_session *rs);

#endif /* __RTP_SESSION_H */
