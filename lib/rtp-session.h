/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef SESSION_H
#define SESSION_H

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
	const char	*userinfo;
	const char	*uri;
	const char	*server;
	unsigned int	port;
	unsigned int	tunnel_id;
	unsigned int	seq_num;
	rtp_media_type_t media;
	char		sess_id[MAX_SESSION_ID_LEN];
	int		net_fd;
	int		out_fd;
	unsigned char	adts_header[ADTS_HEADER_LENGTH];
	int		is_mpeg_4aac;
};

struct rtp_session *rtp_session_alloc(const char *userinfo,
				      const char *uri,
				      const char *server,
				      unsigned int port,
				      rtp_media_type_t media);
void rtp_session_stop(struct rtp_session *rs);
int rtp_session_record(struct rtp_session *rs, const char *file);
int rtp_session_read(struct rtp_session *rs);

#endif
