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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <curl/curl.h>

#include "rtp-session.h"

typedef enum {
	MAIN = 1,
	LC   = 2,
	SSR  = 3,
	LTP  = 4
} mpeg_obj_type_t;

#define RETERR(__msg) ({		\
	if (err_msg)			\
		*err_msg = __msg;	\
	return -1;			\
})

static const char * const trans_fmt =
	"RTP/AVP/TCP/UDP;unicast;interleaved=0-1;client_port=%d-%d";

/* cURL is not thread safe */
static pthread_mutex_t curl_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t port_lock = PTHREAD_MUTEX_INITIALIZER;

static int get_next_port(void)
{
	static int next_port = 4000;
	int port;

	pthread_mutex_lock(&port_lock);
	port = next_port;
	next_port += 2;
	if (next_port > 65535)
		next_port = 4000;
	pthread_mutex_unlock(&port_lock);

	return port;
}

size_t Curl_base64_encode(void *data, const char *inputbuff,
			  size_t insize, char **outptr);

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
	/* Close down our session */
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
		curl_easy_setopt(rs->curl, CURLOPT_HTTPHEADER, NULL);
		curl_easy_cleanup(rs->curl);
		rs->curl = NULL;
	}
	/* Headers cleared after they may be used above */
	if (rs->slist) {
		curl_slist_free_all(rs->slist);
		rs->slist = NULL;
	}
	/* Close any UDP listeners */
	if (rs->vid_fd >= 0) {
		close(rs->vid_fd);
		rs->vid_fd = -1;
	}
	if (rs->aud_fd >= 0) {
		close(rs->aud_fd);
		rs->aud_fd = -1;
	}
	pthread_mutex_unlock(&curl_lock);
}

static void rtp_get_fmtp_param(char *res, const char *fmtp, const char *param)
{
	const char *p = fmtp, *t;

	res[0] = '\0';

	while ((p = strcasestr(p, param)) != NULL) {
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
	if (strcasestr(sdp, type) == NULL)
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

	begin_pos = strcasestr(sdp, type);
	if (begin_pos == NULL)
		return;

	begin_pos = strcasestr(begin_pos, fa);
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

	if ((p = strcasestr(header, "interleaved="))) {
		if (rs->setup_vid)
			id = &rs->tid_v;
		else
			id = &rs->tid_a;

		if (sscanf(p, "interleaved=%d-", id) != 1)
			*id = -1;
	} else if ((p = strcasestr(header, "client_port="))) {
		int *serv_port;

		if (rs->setup_vid) {
			id = &rs->vid_port;
			serv_port = &rs->vid_serv_port;
		} else {
			id = &rs->aud_port;
			serv_port = &rs->aud_serv_port;
		}

		if (sscanf(p, "client_port=%d-", id) != 1)
			*id = -1;

		p = strcasestr(header, "server_port=");
		if (!p || sscanf(p, "server_port=%d-", serv_port) != 1)
			*id = -1;
	}

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

	if (rs->aud_codec == CODEC_ID_AAC) {
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

static int open_listener(int port)
{
	struct sockaddr_in sa_in;
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	const int flag = 1;

	if (fd < 0)
		return -1;

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag,
		       sizeof(flag)) < 0) {
		close(fd);
		return -1;
	}
#ifdef SO_REUSEPORT
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag,
		       sizeof(flag)) < 0) {
		close(fd);
		return -1;
	}
#endif

	memset(&sa_in, 0, sizeof(sa_in));
	sa_in.sin_port = htons(port);
	sa_in.sin_family = AF_INET;
	sa_in.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd, (struct sockaddr *)&sa_in, sizeof(sa_in)) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}

static size_t handle_sdp(void *ptr, size_t size, size_t nmemb,
			 void *userdata);

#define GOTOERR(__msg) ({		\
	if (err_msg)			\
		*err_msg = __msg;	\
	goto setup_fail;		\
})


int rtp_session_start(struct rtp_session *rs, const char **err_msg)
{
	char uri[1024];
	char trans[256];
	int ret = -1;
	long http_code;
	int port;

	if (check_curl())
		RETERR("Failed to initialize cURL");

	rs->tid_v = rs->tid_a = rs->aud_port = rs->vid_port = -1;
	rs->vid_len = rs->vid_valid = rs->aud_len = rs->aud_valid = 0;
	rs->aud_codec = CODEC_ID_NONE;
	rs->vid_codec = CODEC_ID_MPEG4;

	rs->vid_uri[0] = '\0';
	rs->aud_uri[0] = '\0';

	pthread_mutex_lock(&curl_lock);

	rs->curl = curl_easy_init();
	if (rs->curl == NULL)
		GOTOERR("Failed to initialize cURL session");

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

	/* First, get the SDP and parse it */
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_STREAM_URI, uri);
	curl_easy_setopt(rs->curl, CURLOPT_WRITEFUNCTION, handle_sdp);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST,
			 CURL_RTSPREQ_DESCRIBE);
	if (curl_easy_perform(rs->curl))
		GOTOERR("Failed DESCRIBE request");
	curl_easy_getinfo(rs->curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200 || rs->vid_uri[0] == '\0')
		GOTOERR("Bad response from DESCRIBE request");

	/* No longer needed */
	curl_easy_setopt(rs->curl, CURLOPT_WRITEFUNCTION, null_write);

	/* Now setup the audio/video streams */
	port = get_next_port();
	sprintf(trans, trans_fmt, port, port + 1);
	rs->setup_vid = 1;
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_TRANSPORT, trans);
	curl_easy_setopt(rs->curl, CURLOPT_HEADERFUNCTION, handle_setup);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_STREAM_URI, rs->vid_uri);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_SETUP);
	if (curl_easy_perform(rs->curl))
		GOTOERR("Failed video SETUP request");
	curl_easy_getinfo(rs->curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200 || (rs->tid_v < 0 && rs->vid_port <= 0))
		GOTOERR("Bad response from video SETUP request");
	if (rs->vid_port > 0) {
		rs->vid_fd = open_listener(rs->vid_port);
		if (rs->vid_fd < 0)
			GOTOERR("Failure to open UDP listener for video");
	}

	/* SETUP the audio too, if we have a URI for it */
	if (rs->aud_uri[0]) {
		port = get_next_port();
		sprintf(trans, trans_fmt, port, port + 1);
		rs->setup_vid = 0;
		curl_easy_setopt(rs->curl, CURLOPT_RTSP_TRANSPORT, trans);
		curl_easy_setopt(rs->curl, CURLOPT_HEADERFUNCTION, handle_setup);
		curl_easy_setopt(rs->curl, CURLOPT_RTSP_STREAM_URI, rs->aud_uri);
		curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_SETUP);
		if (curl_easy_perform(rs->curl))
			GOTOERR("Failed audio SETUP request");
		curl_easy_getinfo(rs->curl, CURLINFO_RESPONSE_CODE, &http_code);
		if (http_code != 200 || (rs->tid_a < 0 && rs->aud_port <= 0))
			GOTOERR("Bad response from audio SETUP request");
		if (rs->aud_port > 0) {
			rs->aud_fd = open_listener(rs->aud_port);
			if (rs->aud_fd < 0)
				GOTOERR("Failure to open UDP listener for audio");
		}
	}

	curl_easy_setopt(rs->curl, CURLOPT_HEADERFUNCTION, null_write);

	/* Kick 'er in da head (aka PLAY) */
	curl_easy_setopt(rs->curl, CURLOPT_INTERLEAVEFUNCTION, handle_rtp);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_STREAM_URI, uri);
	curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_PLAY);
	if (curl_easy_perform(rs->curl))
		GOTOERR("Failed PLAY request");
	curl_easy_getinfo(rs->curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200)
		GOTOERR("Bad response from PLAY request");

	/* All is well on the northern front */
	ret = 0;

setup_fail:
	pthread_mutex_unlock(&curl_lock);
	if (ret) {
		rtp_session_stop(rs);
		return -1;
	}

	return 0;
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
		sprintf(uri, "rtsp://%s:%d%s", rs->server, rs->port, buf);
	} else if (!strncasecmp(buf, "rtsp://", 7)) {
		/* Full URI */
		strcpy(uri, buf);
	} else {
		/* Relative path */
		sprintf(uri, "rtsp://%s:%d%s%s%s", rs->server, rs->port,
			rs->uri, strlen(rs->uri) == 1 ? "" : "/", buf);
	}
}

static size_t handle_sdp(void *ptr, size_t size, size_t nmemb,
			 void *userdata)
{
	struct rtp_session *rs = userdata;
	size_t ret = size * nmemb;
	const char *media_type;
	char buf[1024];
	char *p;

	((char *)ptr)[size * nmemb] = '\0';

	media_type = "m=video ";
	if (!sdp_check_type(ptr, media_type))
		return ret;

	get_uri(rs->vid_uri, ptr, media_type, rs);
	if (!strlen(rs->vid_uri))
		return ret;

	sdp_get_attr(buf, ptr, media_type, "framerate");
	rs->framerate = strlen(buf) ? atoi(buf) : 30;

	sdp_get_attr(buf, ptr, media_type, "rtpmap");
	if (strlen(buf)) {
		if (strcasestr(buf, "H264"))
			rs->vid_codec = CODEC_ID_H264;
		else if (strcasestr(buf, "MP4V-ES"))
			rs->vid_codec = CODEC_ID_MPEG4;
	}

	/* Now check for audio */
	media_type = "m=audio ";
	if (!sdp_check_type(ptr, media_type))
		return ret;

	get_uri(rs->aud_uri, ptr, media_type, rs);
	if (!strlen(rs->aud_uri))
		return ret;

	/* Any error in parsing after here just leaves the
	 * aud_uri empty so we get no audio. */
	sdp_get_attr(buf, ptr, media_type, "rtpmap");

	if (!strlen(buf)) {
		rs->aud_uri[0] = '\0';
		return ret;
	}

	if ((p = strcasestr(buf, "mpeg4-generic"))) {
		char mode[32];
		char bitrate[32];

		if (sscanf(p, "mpeg4-generic/%d/%d", &rs->samplerate,
			   &rs->channels) != 2) {
			if (sscanf(p, "mpeg4-generic/%d", &rs->samplerate)
			    != 1) {
				rs->aud_uri[0] = '\0';
				return ret;
			}
			rs->channels = 1;
		}

		sdp_get_attr(buf, ptr, media_type, "fmtp");
		rtp_get_fmtp_param(mode, buf, "mode");
		rtp_get_fmtp_param(bitrate, buf, "bitrate");
		if (!strlen(bitrate))
			rs->bitrate = 24000;
		else
			rs->bitrate = atoi(bitrate);

		if (!strcasecmp(mode, "AAC-hbr")) {
			char config[128];

			rs->aud_codec = CODEC_ID_AAC;
			rtp_get_fmtp_param(config, buf, "config");
			set_adts_header(rs->adts_header, config);
		} else
			rs->aud_uri[0] = '\0';
	} else {
		rs->aud_uri[0] = '\0';
	}

	return ret;
}

static int read_listener(struct rtp_session *rs)
{
	unsigned char data[2048];
	int ret;
	fd_set fds;
	struct timeval tv;
	int hi_fd = -1;

	tv.tv_sec = 0;
	tv.tv_usec = 1000;

	FD_ZERO(&fds);
	if (rs->vid_fd >= 0) {
		FD_SET(rs->vid_fd, &fds);
		hi_fd = rs->vid_fd;
	}
	if (rs->aud_fd >= 0) {
		FD_SET(rs->aud_fd, &fds);
		if (rs->aud_fd > hi_fd)
			hi_fd = rs->aud_fd;
	}

	if (hi_fd < 0)
		return -1;

	ret = select(hi_fd + 1, &fds, NULL, NULL, &tv);
	if (ret <= 0)
		return ret;

	if (rs->vid_fd >= 0 && FD_ISSET(rs->vid_fd, &fds)) {
		ret = read(rs->vid_fd, data, sizeof(data));
		if (ret < 0)
			return -1;
		handle_vid(rs, data, ret);
	}

	if (rs->aud_fd >= 0 && FD_ISSET(rs->aud_fd, &fds)) {
		ret = read(rs->aud_fd, data, sizeof(data));
		if (ret < 0)
			return -1;
		handle_aud(rs, data, ret);
	}

	return 0;
}

int rtp_session_read(struct rtp_session *rs)
{
	int ret = 0;

	pthread_mutex_lock(&curl_lock);
	if (rs->tid_v >= 0 || rs->tid_a >= 0) {
		/* Receive interleaved TCP data */
		curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST,
				 CURL_RTSPREQ_RECEIVE);
		if (curl_easy_perform(rs->curl))
			ret = EIO;
	} else if (rs->vid_fd >= 0 || rs->aud_fd >= 0) {
		/* Read stuff from UDP ports */
		if (read_listener(rs)) {
			ret = EIO;
		} else if (0) { //!(rs->heart_beat++ & 0x000003ff)) {
			/* Send a heart beat every so often */
			curl_easy_setopt(rs->curl, CURLOPT_RTSP_REQUEST,
					 CURL_RTSPREQ_OPTIONS);
			/* Some ACTi cameras return an error on OPTIONS after PLAY.
			 * Newer firmware is supposed to fix this problem. So we
			 * simply check for connection timeout and ignore the
			 * actual response code here. */
			if (curl_easy_perform(rs->curl) == CURLE_OPERATION_TIMEDOUT)
				ret = EIO;
		}
	} else {
		ret = EIO;
	}
	pthread_mutex_unlock(&curl_lock);

	return ret;
}
