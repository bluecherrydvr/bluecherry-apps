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

#include <libbluecherry.h>

#include "rtp-session.h"
#include "rtp-request.h"

typedef enum {
	MAIN = 1,
	LC   = 2,
	SSR  = 3,
	LTP  = 4
} mpeg_obj_type_t;

int rtp_session_init(struct rtp_session *rs, void *dbres)
{
	const char *val;
	char *device;
	char *p, *t;

	memset(rs, 0, sizeof(*rs));

	val = bc_db_get_val(dbres, "rtsp_username");
	if (!val)
		return -1;
	strcpy(rs->userinfo, val);

	strcat(rs->userinfo, ":");

	val = bc_db_get_val(dbres, "rtsp_password");
	if (!val)
		return -1;
	strcat(rs->userinfo, val);

	val = bc_db_get_val(dbres, "device");
	if (!val)
		return -1;

	device = strdup(val);
	if (!device)
		return -1;

	p = t = device;
	while (*t != '|' && *t != '\0')
		t++;
	if (*t == '\0') {
		free(device);
		return -1;
	}

	*(t++) = '\0';

	strcpy(rs->server, p);

	p = t;
	while (*t != '|' && *t != '\0')
		t++;
	if (*t == '\0') {
		free(device);
		return -1;
	}

	*(t++) = '\0';

	rs->port = atoi(p);
	strcpy(rs->uri, t);

	free(device);

	rs->net_fd = -1;
	rs->media = RTP_MEDIA_VIDEO;

	return 0;
}

void rtp_session_stop(struct rtp_session *rs)
{
	if (rs->net_fd >= 0) {
		rtp_request_teardown(rs);
		close(rs->net_fd);
		rs->net_fd = -1;
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

static void rtp_putdata(struct rtp_session *rs, const char *data, int len)
{
	unsigned char *out = rs->outbuf;
	int skip = 12;

	/* CSRCs */
	skip += 4 * (data[0] & 0x0f);

	/* Extension */
	if (data[0] & 0x10)
		skip += *(unsigned short *)&data[skip + 2];

	if (rs->is_mpeg_4aac) {
		unsigned char *adts = rs->adts_header;
		unsigned int frame_len;

		/* Au header is 4 bytes */
		skip += 4;

		frame_len = len - skip + ADTS_HEADER_LENGTH;
		adts[3] &= ~0x03;
		adts[3] |= ((frame_len >> 11) & 0x3);
		adts[4] = ((frame_len >> 3) & 0xff);
		adts[5] &= ~0xe0;
		adts[5] |= ((frame_len & 0x7) << 5);

		memcpy(out, adts, ADTS_HEADER_LENGTH);
		out += ADTS_HEADER_LENGTH;
		rs->outbuf_len += ADTS_HEADER_LENGTH;
	}

	memcpy(out, data + skip, len - skip);
	rs->outbuf_len += len - skip;
}

static int rtp_session_setup(struct rtp_session *rs)
{
	struct rtp_response desc_resp;
	struct rtp_response setup_resp;
	char setup_uri[256];
	char buf[128];
	char *p;
	const char *media_type;

	if (rs->net_fd >= 0)
		close(rs->net_fd);

	rs->net_fd = network_client(rs->server, rs->port);
        if (rs->net_fd < 0)
		return -1;

	rs->sess_id[0] = '\0';
	rs->tunnel_id = 0;

	if (rs->media == RTP_MEDIA_VIDEO)
		media_type = "m=video ";
	else
		media_type = "m=audio ";

	/* Get the SDP, and check the type */
	if (rtp_request_describe(rs, &desc_resp) || desc_resp.status != HTTP_OK)
		return -1;

	if (!sdp_check_type(desc_resp.content, media_type))
		return -1;

	/* Get the setup URI if needed */
	strcpy(setup_uri, rs->uri);
	sdp_get_attr(buf, desc_resp.content, media_type, "control");
	if (strlen(buf) != 0) {
		strcat(setup_uri, "/");
		strcat(setup_uri, buf);
	}

	sdp_get_attr(buf, desc_resp.content, media_type, "framerate");
	if (strlen(buf) != 0)
		rs->framerate = atoi(buf);
	else
		rs->framerate = 30; // Guessing

	/* Setup transport parameters */
	if (rtp_request_setup(rs, setup_uri, &setup_resp) ||
	    setup_resp.status != HTTP_OK)
		return -1;

	rtp_response_get_header(&setup_resp, "Session", buf);
	p = strstr(buf, " \t;");
	if (p)
		*p = '\0';
	strcpy(rs->sess_id, buf);

	rtp_response_get_header(&setup_resp, "Transport", buf);
	if ((p = strstr(buf, "interleaved=")) == NULL ||
	    sscanf(p, "interleaved=%d-", &rs->tunnel_id) != 1)
		return -1;

	if (rs->media == RTP_MEDIA_AUDIO) {
		sdp_get_attr(buf, desc_resp.content, media_type,
			     "rtpmap");

		if (strlen(buf) && strstr(buf, "mpeg4-generic")) {
			char mode[32];

			sdp_get_attr(buf, desc_resp.content,
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

	return 0;
}

int rtp_session_start(struct rtp_session *rs)
{
	if (rtp_session_setup(rs)) {
		rtp_session_stop(rs);
		return -1;
	}

	if (rtp_request_play(rs)) {
		rtp_session_stop(rs);
		return -1;
	}

	return 0;
}

static void rtp_check_frame(struct rtp_session *rs)
{
	const unsigned char *p = rs->outbuf;

	if (rs->outbuf_len == 0)
		return;

	/* If we had a valid frame or no length, we just copy it */
	if (rs->frame_valid || rs->frame_len == 0) {
		memcpy(rs->frame_buf, p, rs->outbuf_len);
		rs->frame_len = rs->outbuf_len;
		rs->frame_valid = rs->outbuf_len = 0;
		return;
	}

	/* Check if this is the start of a new frame. If so, we mark
	 * this frame as valid, and will start the next frame with
	 * outbuf on the next call. */
	if (p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x01 &&
	    (p[3] >= 0xb0 && p[3] <= 0xbf)) {
		rs->frame_valid = 1;
		return;
	}

	/* If it's too big, just ditch the rest */
	if (rs->frame_len + rs->outbuf_len > sizeof(rs->frame_buf))
		return;

	/* Else, append it */
	memcpy(rs->frame_buf + rs->frame_len, p, rs->outbuf_len);
	rs->frame_len += rs->outbuf_len;

	rs->outbuf_len = 0;
}

int rtp_session_read(struct rtp_session *rs)
{
	char data[2048], header[4];
	unsigned int len, off;
	int ret;

	if (rs->net_fd < 0)
		return EIO;

	/* Catch residual data from last call */
	rtp_check_frame(rs);

	/* Read RTSP embedded data header and then the RTP data. */
	ret = read(rs->net_fd, header, sizeof(header));
	if (ret <= 0)
		return ret;

	if (header[0] != '$')
		return EINVAL;

	len = ntohs(*(u_int16_t*)&header[2]);
      
	if (len > sizeof(data))
		return EINVAL;

	for (ret = off = 0; off < len; off += ret) {
		ret = read(rs->net_fd, data + off, len - off);
		if (ret < 0)
			return EIO;
	}

	/* Only parse RTP packets */
	if ((unsigned char)header[1] != rs->tunnel_id)
		return EAGAIN;

	rtp_putdata(rs, data, len);
	rtp_check_frame(rs);

	return rs->frame_valid ? 0 : EAGAIN;
}
