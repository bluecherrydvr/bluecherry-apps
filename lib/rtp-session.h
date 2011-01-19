/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __RTP_SESSION_H
#define __RTP_SESSION_H

#include <stdio.h>

typedef enum {
	RTP_MEDIA_VIDEO,
	RTP_MEDIA_AUDIO
} rtp_media_type_t;

#define MAX_SESSION_ID_LEN	32
#define ADTS_HEADER_LENGTH	7
#define MAX_RESPONSE_LENGTH	4096

struct rtp_response {
	char	data[MAX_RESPONSE_LENGTH];
	int	status;
	char	*content;
	int	ptr;
	int	skip;
};

struct rtp_session {
	char		userinfo[256];
	char		uri[1024];
	char		server[256];
	unsigned int	port;
	unsigned int	tunnel_id;
	unsigned int	seq_num;
	rtp_media_type_t media;
	char		sess_id[MAX_SESSION_ID_LEN];
	int		net_fd;
	unsigned char	adts_header[ADTS_HEADER_LENGTH];
	int		is_mpeg_4aac;
	/* Where to keep full frames */
	unsigned char	frame_buf[1024 * 128];
	int		frame_len;
	int		frame_valid;
	/* Buffer for RTP packets */
	unsigned char	outbuf[2048];
	int		outbuf_len;
	int		framerate;
};

int rtp_session_init(struct rtp_session *rs, void *dbres);
void rtp_session_stop(struct rtp_session *rs);
int rtp_session_start(struct rtp_session *rs);
int rtp_session_read(struct rtp_session *rs);

#endif /* __RTP_SESSION_H */
