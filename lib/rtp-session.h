/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __RTP_SESSION_H
#define __RTP_SESSION_H

#include <libavcodec/avcodec.h>

#define ADTS_HEADER_LENGTH	7

struct rtp_session {
	void		*curl, *slist;
	char		userinfo[256];
	char		uri[1024];
	char		server[256];
	unsigned int	port;

	/* State */
	int		setup_vid;
	unsigned int	heart_beat;

	/* Tunnel tracking (interleave) */
	int		tid_a, tid_v;

	/* Tracking for UDP */
	int		aud_port, vid_port;
	int		aud_serv_port, vid_serv_port;
	int		aud_fd, vid_fd;

	/* URI for audio and video */
	char		aud_uri[1024];
	char		vid_uri[1024];

	/* Video frames */
	unsigned char	vid_buf[1024 * 128];
	int		vid_len;
	int		vid_valid;

	/* Audio frames */
	unsigned char	aud_buf[1024 * 4];
	int		aud_len;
	int		aud_valid;
	unsigned char	adts_header[ADTS_HEADER_LENGTH];

	/* Information we gather from SDP */
	int		framerate;
	enum CodecID	vid_codec, aud_codec;
	int		samplerate, bitrate, channels;
};

void rtp_session_init(struct rtp_session *rs, const char *userinfo,
		      const char *uri, const char *server,
		      unsigned int port);
void rtp_session_stop(struct rtp_session *rs);
int rtp_session_start(struct rtp_session *rs);
int rtp_session_read(struct rtp_session *rs);

#endif /* __RTP_SESSION_H */
