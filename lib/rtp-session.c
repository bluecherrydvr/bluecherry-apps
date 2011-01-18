/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <httpd.h>

#include "rtp-session.h"
#include "rtp-request.h"

typedef enum {
	MAIN = 1,
	LC   = 2,
	SSR  = 3,
	LTP  = 4
} mpeg_obj_type_t;


struct rtp_session *rtp_session_alloc(const char *userinfo,
				      const char *uri,
				      const char *server,
				      unsigned int port,
				      rtp_media_type_t media)
{
	struct rtp_session *rs = malloc(sizeof(*rs));

	if (!rs)
		return NULL;

	memset(rs, 0, sizeof(*rs));

	rs->userinfo = userinfo;
	rs->net_fd = -1;
	rs->out_fd = -1;
	rs->media = media;
	rs->server = server;
	rs->port = port;
	rs->uri = uri;

	return rs;
}

void rtp_session_stop(struct rtp_session *rs)
{
	if (rs->net_fd >= 0) {
		rtp_request_teardown(rs);
		close(rs->net_fd);
		rs->net_fd = -1;
	}
	if (rs->out_fd >= 0) {
		close(rs->out_fd);
		rs->out_fd = -1;
	}
}

static int network_client(const char *server, unsigned int port)
{
	struct sockaddr_in in_server;
	int fd;

	memset(&in_server, 0, sizeof in_server);
	in_server.sin_family = AF_INET;
	in_server.sin_addr.s_addr = inet_addr(server);

	if (in_server.sin_addr.s_addr == INADDR_NONE) {
		struct hostent *hp;

		hp = gethostbyname(server);
		if (hp != NULL) {
			memcpy(&in_server.sin_addr, hp->h_addr, hp->h_length);
			in_server.sin_family = hp->h_addrtype;
		} else {
			return -1;
		}
	}

	in_server.sin_port = htons(port);
	fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0)
		return -1;

	if (connect(fd, (struct sockaddr *) &in_server, sizeof in_server) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}

static void rtp_get_fmtp_param(char *res, const char *fmtp, const char *param)
{
	const char *p = fmtp, *t;

	res[0] = '\0';

	while ((p = strstr(p, param)) != NULL) {
		p += strlen(param);
		if (*p != '=')
			continue;
		p++;

	        t = strpbrk(p, ";\r\n");
	        if (t == NULL) {
			strcpy(res, p);
		} else {
			strncpy(res, p, t - p);
			res[t - p] = '\0';
		}
	}
}

static void adts_set_profile(unsigned char *header, unsigned int config)
{
	unsigned char profile;
	mpeg_obj_type_t obj_type = (config >> 11) & 0x1f;

	switch (obj_type)
	{
	case MAIN:
		profile = 0x00;
		break;

	case LC:
		profile = 0x01;
		break;

	case SSR:
		profile = 0x02;
		break;

	case LTP:
		profile = 0x03;
		break;

	default:
		fprintf(stderr, "Object Type %d not supported\n", obj_type);
		profile = 0x00;
		break;
	}

	header[2] &= ~0xc0;
	header[2] |= ((profile & 0x03) << 6);
}

static void set_adts_header(unsigned char header[ADTS_HEADER_LENGTH],
			    const char *config_str)
{
	unsigned int config;
	unsigned char val;

	sscanf(config_str, "%x", &config);

	memset(header, 0, ADTS_HEADER_LENGTH);

	/* Syncword */
	header[0] = 0xff;
	header[1] = 0xf0;

	/* Protection Absent, no ADTS error check */
	header[1] |= 1;

	adts_set_profile(header, config);

	/* Set sample frequency */
	val = (config >> 7) & 0x0f;
	header[2] &= ~0x3c;
	header[2] |= (val << 2);

	/* Set channel */
	val = (config >> 3) & 0x07;
	header[2] &= ~0x1;
	header[2] |= ((val & 0x4) >> 2);
	header[3] &= ~0xc0;
	header[3] |= ((val & 0x3) << 6);
}

static int sdp_check_type(const char *sdp, const char *type)
{
	if (strstr(sdp, type) == NULL)
		return 0;

	return 1;
}

static void sdp_get_attr(char *res, const char *sdp, const char *type,
			 const char *attr)
{
	char fa[32];
	const char *begin_pos, *end_pos;

	res[0] = '\0';
	snprintf(fa, sizeof(fa), "a=%s:", attr);
	fa[sizeof(fa) - 1] = '\0';

	begin_pos = strstr(sdp, type);
	if (begin_pos == NULL)
		return;

	begin_pos = strstr(begin_pos, fa);
	if (begin_pos == NULL)
		return;

	begin_pos += strlen(fa);
	end_pos = strpbrk(begin_pos, "\r\n");
	if (!end_pos)
		return;

	strncpy(res, begin_pos, end_pos - begin_pos);
	res[end_pos - begin_pos] = '\0';
}

static int rtp_putdata(int fd, const char *data, int len,
		       unsigned char *adts, int adts_len)
{
	int skip = 12;

	if (fd < 0)
		return -1;

	/* CSRCs */
	skip += 4 * (data[0] & 0x0f);

	/* Extension */
	if (data[0] & 0x10)
		skip += *(unsigned short *)&data[skip + 2];

	if (adts_len >= 7) {
		unsigned int frame_len;

		/* Au header is 4 bytes */
		skip += 4;

		frame_len = len - skip + adts_len;
		adts[3] &= ~0x03;
		adts[3] |= ((frame_len >> 11) & 0x3);
		adts[4] = ((frame_len >> 3) & 0xff);
		adts[5] &= ~0xe0;
		adts[5] |= ((frame_len & 0x7) << 5);
		if (write(fd, adts, adts_len) != adts_len)
			return -1;
	}

	if (write(fd, data + skip, len - skip) != len - skip)
		return -1;

	return 0;
}

static int rtp_session_setup(struct rtp_session *rs)
{
	struct rtp_response *desc_resp = NULL;
	struct rtp_response *setup_resp = NULL;
	char setup_uri[256];
	char buf[128];
	char *p;
	const char *media_type;
	int ret = -1;

	if (rs->net_fd >= 0)
		close(rs->net_fd);

	rs->net_fd = network_client(rs->server, rs->port);
        if (rs->net_fd < 0)
		return -1;

	if (rs->media == RTP_MEDIA_VIDEO)
		media_type = "m=video ";
	else
		media_type = "m=audio ";

	/* Get the SDP, and check the type */
	desc_resp = rtp_request_describe(rs);
	if (desc_resp == NULL || desc_resp->status != HTTP_OK)
		goto setup_fail;

	if (!sdp_check_type(desc_resp->content, media_type))
		goto setup_fail;

	/* Get the setup URI if needed */
	strcpy(setup_uri, rs->uri);
	sdp_get_attr(buf, desc_resp->content, media_type, "control");
	if (strlen(buf) != 0) {
		strcat(setup_uri, "/");
		strcat(setup_uri, buf);
	}

	/* Setup transport parameters */
	setup_resp = rtp_request_setup(rs, setup_uri);

	if (setup_resp == NULL || setup_resp->status != HTTP_OK)
		goto setup_fail;

	rtp_response_get_header(setup_resp, "Session", buf);
	p = strstr(buf, " \t;");
	if (p)
		*p = '\0';
	strcpy(rs->sess_id, buf);

	rtp_response_get_header(setup_resp, "Transport", buf);
	if ((p = strstr(buf, "interleaved=")) == NULL ||
	    sscanf(p, "interleaved=%d-", &rs->tunnel_id) != 1)
		goto setup_fail;

	if (rs->media == RTP_MEDIA_AUDIO) {
		sdp_get_attr(buf, desc_resp->content, media_type,
			     "rtpmap");

		if (strlen(buf) && strstr(buf, "mpeg4-generic")) {
			char mode[32];

			sdp_get_attr(buf, desc_resp->content,
				     media_type, "fmtp");
			rtp_get_fmtp_param(mode, buf, "mode");

			if (!strcmp(mode, "AAC-hbr")) {
				char config[128];

				rs->is_mpeg_4aac = 1;
				rtp_get_fmtp_param(config, buf, "config");
				set_adts_header(rs->adts_header, config);
			}
		}
	}

	ret = 0;

setup_fail:
	free(setup_resp);
	free(desc_resp);

	return ret;
}

int rtp_session_record(struct rtp_session *rs, const char *outfile)
{
	if (rtp_session_setup(rs)) {
		rtp_session_stop(rs);
		return -1;
	}

	if (rs->out_fd >= 0)
		close(rs->out_fd);

	rs->out_fd = open(outfile, O_WRONLY | O_CREAT | O_EXCL, 0640);
	if (rs->out_fd < 0 || rtp_request_play(rs)) {
		rtp_session_stop(rs);
		return -1;
	}

	return 0;
}

int rtp_session_read(struct rtp_session *rs)
{
	char data[2048], header[4];
	unsigned int len, off;
	int ret;

	if (rs->net_fd < 0 || rs->out_fd < 0)
		return -1;

	/* Read RTSP embedded data header and then the RTP data. */
	ret = read(rs->net_fd, header, sizeof(header));
	if (ret <= 0)
		return ret;

	if (header[0] != '$')
		return -1;

	len = ntohs(*(u_int16_t*)&header[2]);
      
	if (len > sizeof(data))
		return -1;

	for (ret = off = 0; off < len; off += ret) {
		ret = read(rs->net_fd, data + off, len - off);
		if (ret < 0)
			return -1;
	}

	/* Only parse RTP packets */
	if ((unsigned char)header[1] != rs->tunnel_id)
		return 0;

	return rtp_putdata(rs->out_fd, data, len, rs->adts_header,
			   rs->is_mpeg_4aac ? ADTS_HEADER_LENGTH : 0);
}
