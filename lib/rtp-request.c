/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <httpd.h>

#include "rtp-request.h"

#define READ_SIZE 1500


static void rtp_response_init(struct rtp_response *rp)
{
	memset(rp, 0, sizeof(*rp));
	rp->status = HTTP_INTERNAL_SERVER_ERROR;
}

static void rtp_response_parse(struct rtp_response *rp)
{
	char *p, *t;

	p = strstr(rp->data, "RTSP/");
	if (p == NULL)
		return;

	/* The response status line is RTSP/1.0 Code Reason */
	p = strstr(p, " ");
	if (p == NULL)
		return;
	p++;

	t = strstr(p, " ");
	if (t == NULL)
		return;

	t[0] = '\0';
	rp->status = atoi(p);
	t[0] = ' ';

	/* Find start of content */
	rp->content = strstr(t, CRLF CRLF);
}


static int rtp_response_add_data(struct rtp_response *rp, void *__data,
				 int len)
{
	unsigned char *data = __data;
	char *p;
	int ret = 0;

	if (rp->skip) {
		if (len <= rp->skip) {
			rp->skip -= len;
			return 0;
		} else {
			data += rp->skip;
			len -= rp->skip;
			rp->skip = 0;
		}
	}

	while (len && *data == '$') {
		int a_len = 4 + data[2] * 256 + data[3];

		if (a_len <= len) {
			data += a_len;
			len -= a_len;
		} else {
			rp->skip = a_len - len;
			return 0;
		}
	}

	if (!len)
		return 0;

	memcpy(rp->data + rp->ptr, data, len);
	rp->ptr += len;

	/* Find CRLF CRLF that indicates end of header */
	p = strstr(rp->data, CRLF CRLF);

	if (p) {
		char val[128];

		p += strlen(CRLF CRLF);

		/* Check if request has content */
		rtp_response_get_header(rp, "Content-Length", val);

		if (strlen(val) > 0) {
			if (rp->ptr == (p - rp->data) + atoi(val))
				ret = 1;
		} else {
			ret = 1;
		}
	}

	if (ret)
		rtp_response_parse(rp);

	return ret;
}

void rtp_response_get_header(struct rtp_response *rp, const char *field, char *val)
{
	char *p = rp->data;
	size_t len = strlen(field);

	val[0] = '\0';

	while ((p = strstr(p, field)) != NULL) {
		char *t;

		if (strlen(p) < len)
			break;
		p += len;

		if (p[0] != ':')
			continue;
		p++;

		while (p[0] == ' ' && p[0] != '\0')
			p++;

		t = strstr(p, CRLF);
		if (t == NULL)
			break;

		strncpy(val, p, t - p);
		val[t - p] = '\0';
		break;
	}
}

static void rtp_request_add_header(struct rtp_session *rs,
				   const char *field,
				   const char *val)
{
	strcat(rs->req_buf, field);
	strcat(rs->req_buf, ": ");
	strcat(rs->req_buf, val);
	strcat(rs->req_buf, CRLF);
}

static void rtp_request_init(struct rtp_session *rs,
			     const char *cmd, const char *uri)
{
	char seq[10];

	memset(rs->req_buf, 0, sizeof(rs->req_buf));

	sprintf(rs->req_buf, "%s rtsp://%s:%d%s RTSP/1.0%s", cmd, rs->server,
		rs->port, uri, CRLF);
	sprintf(seq, "%d", rs->seq_num++);

	/* Add common header fields */
	rtp_request_add_header(rs, "CSeq", seq);
}

static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmno"
			   "pqrstuvwxyz0123456789+/";

static void encodeblock(unsigned char in[3], unsigned char out[4], int len)
{
	out[0] = cb64[in[0] >> 2];
	out[1] = cb64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
	out[2] = (unsigned char)(len > 1 ? cb64[((in[1] & 0x0f) << 2) |
		 ((in[2] & 0xc0) >> 6)] : '=');
	out[3] = (unsigned char)(len > 2 ? cb64[in[2] & 0x3f] : '=');
}

static void base64_encode_ascii(char *out_buf, const char *in_buf)
{
	const char *in_p = in_buf;
	unsigned char *out_p = (unsigned char *)out_buf;
	unsigned char in[3];

	while (*in_p != '\0' ) {
		int len = 0;
		int i;

		for (i = 0; i < 3; i++) {
			in[i] = *(unsigned char *)in_p;

			if (in[i] == '\0') {
				in[i] = 0;
				break;
			}
			len++;
			in_p++;
		}

		if (len) {
			encodeblock(in, out_p, len);
			out_p += 4;
		}
	}
}

static void rtp_request_add_auth(struct rtp_session *rs)
{
	char coded[100];

	if (!rs->userinfo || !strlen(rs->userinfo))
		return;

	memset(coded, 0, sizeof(coded));
	strcpy(coded, "Basic ");
	base64_encode_ascii(coded + strlen(coded), rs->userinfo);

	rtp_request_add_header(rs, "Authorization", coded);
}

int rtp_request_send(struct rtp_session *rs,
		     struct rtp_response *rp)
{
	char read_buf[READ_SIZE];
	int len;

	rtp_response_init(rp);

	if (strlen(rs->sess_id))
		rtp_request_add_header(rs, "Session", rs->sess_id);
	rtp_request_add_auth(rs);
	rtp_request_add_header(rs, "User-Agent", "Bluecherry Server");

	/* Finalize request with CRLF */
	strcat(rs->req_buf, CRLF);

	if (write(rs->net_fd, rs->req_buf, strlen(rs->req_buf)) < 0)
		return -1;

	/* Add data to response until a complete response is received */
	do {
		len = read(rs->net_fd, read_buf, sizeof(read_buf));
		if (len <= 0)
			return -1;
	} while (!rtp_response_add_data(rp, read_buf, len));

	return rp->status == HTTP_OK ? 0 : -1;
}

int rtp_request_describe(struct rtp_session *rs, struct rtp_response *rp)
{
	rtp_request_init(rs, "DESCRIBE", rs->uri);
	rtp_request_add_header(rs, "Accept", "application/sdp");

	return rtp_request_send(rs, rp);
}

/* This doesn't use the base URI we are requesting */
int rtp_request_setup(struct rtp_session *rs, const char *uri, struct rtp_response *rp)
{
	rtp_request_init(rs, "SETUP", uri);
	rtp_request_add_header(rs, "Transport", "RTP/AVP/TCP;unicast");

	return rtp_request_send(rs, rp);
}

int rtp_request_play(struct rtp_session *rs)
{
	struct rtp_response rp;

	rtp_request_init(rs, "PLAY", rs->uri);

	return rtp_request_send(rs, &rp);
}

void rtp_request_teardown(struct rtp_session *rs)
{
	struct rtp_response rp;

	rtp_request_init(rs, "TEARDOWN", rs->uri);

	rtp_request_send(rs, &rp);
}
