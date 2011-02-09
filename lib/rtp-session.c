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

#include <pthread.h>
#include <curl/curl.h>

#include "rtp-session.h"

typedef enum {
	MAIN = 1,
	LC   = 2,
	SSR  = 3,
	LTP  = 4
} mpeg_obj_type_t;

size_t Curl_base64_encode(void *data, const char *inputbuff,
			  size_t insize, char **outptr);

/* cURL is not thread safe */
static pthread_mutex_t curl_lock = PTHREAD_MUTEX_INITIALIZER;

void rtp_session_init(struct rtp_session *rs, const char *userinfo,
		     const char *uri, const char *server,
		     unsigned int port)
{
	memset(rs, 0, sizeof(*rs));
 
	strcpy(rs->userinfo, userinfo);
	strcpy(rs->server, server);
	strcpy(rs->uri, uri);
	rs->port = port;
}

static size_t null_write(void *ptr, size_t size, size_t nmemb,
			 void *userdata)
{
	return size * nmemb;
}

void rtp_session_stop(struct rtp_session *rs)
{
	pthread_mutex_lock(&curl_lock);
	if (rs->curl) {
		curl_easy_setopt(rs->curl, CURLOPT_WRITEFUNCTION,
				 null_write);
		curl_easy_setopt(rs->curl, CURLOPT_INTERLEAVEFUNCTION,
				 null_write);
		curl_easy_setopt(rs->curl, CURLOPT_HEADERFUNCTION,
				 null_write);
		curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST,
				 CURL_RTSPREQ_TEARDOWN);
		curl_easy_perform(rs->curl);
		curl_easy_cleanup(rs->curl);
		rs->curl = NULL;
	}
	if (rs->slist) {
		curl_slist_free_all(rs->slist);
		rs->slist = NULL;
	}
	pthread_mutex_unlock(&curl_lock);
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
		/* XXX Not supported!! */
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

static size_t handle_setup(void *ptr, size_t size, size_t nmemb,
			   void *userdata)
{
	struct rtp_session *rs = userdata;
	char *header = (char *)ptr;
	int len = size * nmemb;
	int *id;
	char *p;

	header[len] = '\0';
	if (strncasecmp(header, "Transport: ", 11))
		return len;

	if (rs->setup_vid)
		id = &rs->tid_v;
	else
		id = &rs->tid_a;

	if ((p = strstr(header, "interleaved=")) == NULL ||
	    sscanf(p, "interleaved=%d-", id) != 1)
		*id = -1;

	return len;
}

static void handle_vid(struct rtp_session *rs, unsigned char *data,
		       int len)
{
	int skip = 12;

	if (rs->vid_valid)
		rs->vid_valid = rs->vid_len = 0;

	/* CSRCs */
	skip += 4 * (data[0] & 0x0f);

	if (skip >= len) {
		rs->vid_valid = rs->vid_len = 0;
		return;
	}

	memcpy(rs->vid_buf + rs->vid_len, data + skip, len - skip);
	rs->vid_len += len - skip;

	if (data[1] & 0x80)
		rs->vid_valid = 1;
}

static void handle_aud(struct rtp_session *rs, unsigned char *data,
		       int len)
{
	int skip = 12;

	if (rs->aud_valid)
		rs->aud_valid = rs->aud_len = 0;

	/* CSRCs */
	skip += 4 * (data[0] & 0x0f);

	if (skip >= len) {
		rs->aud_valid = rs->aud_len = 0;
		return;
	}

	if (rs->is_aac) {
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

		memcpy(rs->aud_buf + rs->aud_len, adts, ADTS_HEADER_LENGTH);
		rs->aud_len += ADTS_HEADER_LENGTH;
	}

	memcpy(rs->aud_buf + rs->aud_len, data + skip, len - skip);
	rs->aud_len += len - skip;

	if (data[1] & 0x80)
		rs->aud_valid = 1;
}

static size_t handle_rtp(void *ptr, size_t size, size_t nmemb,
			 void *userdata)
{
	struct rtp_session *rs = userdata;
	size_t ret_len = size * nmemb;
	unsigned char *data = ptr;
	int tid, len;

	tid = data[1];
	len = ((int)data[2] << 8) | (int)data[3];

	if (rs->tid_v == tid)
		handle_vid(rs, data + 4, len);
	else if (rs->tid_a == tid)
		handle_aud(rs, data + 4, len);

	return ret_len;
}

static int check_curl(void)
{
	static int initialized;
	int ret = 0;

	pthread_mutex_lock(&curl_lock);
	if (!initialized) {
		if (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK)
			initialized = 1;
		else
			ret = -1;
	}
	pthread_mutex_unlock(&curl_lock);

	return ret;
}

static void rtp_add_auth(struct rtp_session *rs)
{
	char coded[200];
	char *encoding = NULL;

	if (!strlen(rs->userinfo))
		return;

	memset(coded, 0, sizeof(coded));
	strcpy(coded, "Authorization: Basic ");

	Curl_base64_encode(NULL, rs->userinfo, strlen(rs->userinfo),
			   &encoding);
	if (encoding != NULL) {
		if (strlen(coded) + strlen(encoding) < sizeof(coded) - 1)
			strcat(coded, encoding);
		curl_free(encoding);
	}

	rs->slist = curl_slist_append(rs->slist, coded);
	curl_easy_setopt(rs->curl, CURLOPT_HTTPHEADER, rs->slist);
}

static size_t handle_sdp(void *ptr, size_t size, size_t nmemb,
			 void *userdata);

int rtp_session_start(struct rtp_session *rs)
{
	char uri[1024];
	int ret = -1;
	long http_code;

	if (check_curl())
		return -1;

	rs->tid_v = rs->tid_a = -1;
	rs->vid_len = rs->vid_valid = rs->aud_len = rs->aud_valid = 0;
	rs->is_mpeg4 = rs->is_h264 = rs->is_aac = rs->is_mp3;

	rs->vid_uri[0] = '\0';
	rs->aud_uri[0] = '\0';

	pthread_mutex_lock(&curl_lock);

	rs->curl = curl_easy_init();
	if (rs->curl == NULL)
		goto setup_fail;

	/* Build our base URI */
	sprintf(uri, "rtsp://%s:%d%s", rs->server, rs->port, rs->uri);

	/* Setup the base cURL session */
	curl_easy_setopt(rs->curl, CURLOPT_PRIVATE, (void *)rs);
	curl_easy_setopt(rs->curl, CURLOPT_URL, uri);
	/* cURL doesn't support RTSP auth yet */
        rtp_add_auth(rs);
	/* Defaults for data handlers do nothing */
	curl_easy_setopt(rs->curl, CURLOPT_INTERLEAVEDATA, rs);
	curl_easy_setopt(rs->curl, CURLOPT_WRITEDATA, rs);
	curl_easy_setopt(rs->curl, CURLOPT_HEADERDATA, rs);
	curl_easy_setopt(rs->curl, CURLOPT_HEADERFUNCTION, null_write);
	curl_easy_setopt(rs->curl, CURLOPT_WRITEFUNCTION, null_write);
	curl_easy_setopt(rs->curl, CURLOPT_INTERLEAVEFUNCTION, null_write);
	curl_easy_setopt(rs->curl, CURLOPT_TIMEOUT, 3);
	curl_easy_setopt(rs->curl, CURLOPT_VERBOSE, 0);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_TRANSPORT,
			 "RTP/AVP/TCP;unicast");

	/* First, get the SDP and parse it */
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_STREAM_URI, uri);
	curl_easy_setopt(rs->curl, CURLOPT_WRITEFUNCTION, handle_sdp);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST,
			 CURL_RTSPREQ_DESCRIBE);
	if (curl_easy_perform(rs->curl))
		goto setup_fail;
	curl_easy_getinfo(rs->curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200 || rs->vid_uri[0] == '\0')
		goto setup_fail;

	/* No longer needed */
	curl_easy_setopt(rs->curl, CURLOPT_WRITEFUNCTION, null_write);

	/* Now setup the audio/video streams and PLAY them */
	rs->setup_vid = 1;
	curl_easy_setopt(rs->curl, CURLOPT_HEADERFUNCTION, handle_setup);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_STREAM_URI, rs->vid_uri);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_SETUP);
	if (curl_easy_perform(rs->curl))
		goto setup_fail;
	curl_easy_getinfo(rs->curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200 || rs->tid_v < 0)
		goto setup_fail;

	if (rs->aud_uri[0]) {
		rs->setup_vid = 0;
		curl_easy_setopt(rs->curl, CURLOPT_HEADERFUNCTION, handle_setup);
		curl_easy_setopt(rs->curl, CURLOPT_RTSP_STREAM_URI, rs->aud_uri);
		curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_SETUP);
		if (curl_easy_perform(rs->curl))
			goto setup_fail;
		curl_easy_getinfo(rs->curl, CURLINFO_RESPONSE_CODE, &http_code);
		if (http_code != 200 || rs->tid_a < 0)
			goto setup_fail;
	}

	curl_easy_setopt(rs->curl, CURLOPT_HEADERFUNCTION, null_write);

	/* Kick 'er in da head */
	curl_easy_setopt(rs->curl, CURLOPT_INTERLEAVEFUNCTION, handle_rtp);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_STREAM_URI, rs->uri);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_PLAY);
	if (curl_easy_perform(rs->curl))
		goto setup_fail;
	curl_easy_getinfo(rs->curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200)
		goto setup_fail;

	/* All is well on the northern front */
	ret = 0;

setup_fail:
	pthread_mutex_unlock(&curl_lock);
	if (ret)
		rtp_session_stop(rs);

	return ret;
}

static void get_uri(char *uri, char *sdp, const char *type,
		    struct rtp_session *rs)
{
	char buf[1024];

	/* Get the setup URI if needed */
	sdp_get_attr(buf, sdp, type, "control");
	if (strlen(buf) == 0)
		return;

	if (buf[0] == '/') {
		/* Absolute path */
		sprintf(uri, "rtsp://%s:%d%s", rs->server,
			rs->port, buf);
	} else if (!strncasecmp(buf, "rtsp://", 7)) {
		/* Full URI */
		strcpy(uri, buf);
	} else {
		/* Relative path */
		sprintf(uri, "%s%s", rs->uri, buf);
	}
}

static size_t handle_sdp(void *ptr, size_t size, size_t nmemb,
			 void *userdata)
{
	struct rtp_session *rs = userdata;
	size_t ret = size * nmemb;
	const char *media_type;
	char buf[1024];

	((char *)ptr)[size * nmemb] = '\0';

	media_type = "m=video ";
	if (!sdp_check_type(ptr, media_type))
		return ret;

	get_uri(rs->vid_uri, ptr, media_type, rs);
	if (rs->vid_uri[0] == '\0')
		return ret;

	sdp_get_attr(buf, ptr, media_type, "framerate");
	if (strlen(buf) != 0)
		rs->framerate = atoi(buf);
	else
		rs->framerate = 30; // Guessing

	sdp_get_attr(buf, ptr, media_type, "rtpmap");
	if (strlen(buf)) {
		if (strstr(buf, "H264"))
			rs->is_h264 = 1;
		else if (strstr(buf, "MP4V-ES"))
			rs->is_mpeg4 = 1;
	}

	/* Now check for audio */
	media_type = "m=audio ";
	if (!sdp_check_type(ptr, media_type))
		return ret;

	get_uri(rs->aud_uri, ptr, media_type, rs);
	if (rs->aud_uri[0] == '\0')
		return ret;

	sdp_get_attr(buf, ptr, media_type, "rtpmap");

	if (strlen(buf) && strstr(buf, "mpeg4-generic")) {
		char mode[32];

		sdp_get_attr(buf, ptr, media_type, "fmtp");
		rtp_get_fmtp_param(mode, buf, "mode");

		if (!strcmp(mode, "AAC-hbr")) {
			char config[128];

			rs->is_aac = 1;
			rtp_get_fmtp_param(config, buf, "config");
			set_adts_header(rs->adts_header, config);
		}
	}

	return ret;
}

int rtp_session_read(struct rtp_session *rs)
{
	int ret = 0;

	pthread_mutex_lock(&curl_lock);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_RECEIVE);
	if (curl_easy_perform(rs->curl))
		ret = EIO;
	pthread_mutex_unlock(&curl_lock);

	return ret;
}
