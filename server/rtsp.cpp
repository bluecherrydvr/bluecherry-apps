/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include "bc-server.h"
#include "rtsp.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>

extern "C" {
#include "libavutil/avstring.h"
#include "libavutil/intreadwrite.h"
}

rtsp_server::rtsp_server()
	: serverfd(-1), n_fds(0)
{
	memset(fds, 0, sizeof(fds));
	memset(connections, 0, sizeof(connections));
	wakeupfd[0] = wakeupfd[1] = -1;
	pthread_mutex_init(&poll_mutex, 0);
}

rtsp_server::~rtsp_server()
{
	for (int i = 0; i < n_fds; ++i)
		delete connections[i];
	close(serverfd);
	close(wakeupfd[0]);
	close(wakeupfd[1]);
	pthread_mutex_destroy(&poll_mutex);
}

int rtsp_server::setup(int port)
{
	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd < 0)
		return -1;

	int yes = 1;
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		goto error;

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	if (bind(serverfd, (sockaddr*)&addr, sizeof(addr)) < 0)
		goto error;

	if (listen(serverfd, 10) < 0)
		goto error;

	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, wakeupfd) < 0)
		goto error;

	addFd(NULL, serverfd, POLLIN);
	addFd(NULL, wakeupfd[0], POLLIN);

	return 0;

error:
	close(serverfd);
	serverfd = -1;
	return -1;
}

void rtsp_server::run()
{
	if (serverfd < 0)
		return;

	for (;;) {
		int n = poll(fds, n_fds, -1);
		if (n < 0) {
			bc_log("rtsp_server: poll: %s", strerror(errno));
			break;
		}

		for (int i = 0; i < n_fds; ++i) {
			if (!fds[i].revents)
				continue;

			bc_log("rtsp_server: poll on socket %d (fd %d) for %d", i, fds[i].fd, fds[i].revents);
			rtsp_connection *c = connections[i];

			if (fds[i].fd == serverfd) {
				acceptConnection();
			} else if (c) {
				int re = -1;
				if (fds[i].revents & POLLOUT)
					re = c->writable();
				else if (fds[i].revents & POLLIN)
					re = c->readable();

				if (re < 0) {
					/* Close the connection and remove it from polling */
					delete c;
					--i;
					continue;
				}
			} else if (fds[i].fd == wakeupfd[0]) {
				char buf[128];
				if (read(wakeupfd[0], buf, sizeof(buf)) < 0)
					;
			} else {
				bc_log("W: Unknown FD in rtsp_server");
				fds[i].events = 0;
			}

			fds[i].revents = 0;
		}
	}
}

void *rtsp_server::runThread(void *p)
{
	static_cast<rtsp_server*>(p)->run();
	return NULL;
}

int rtsp_server::addFd(rtsp_connection *instance, int fd, int events)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
		return -1;

	if (n_fds >= FD_MAX)
		return -1;

	fds[n_fds].fd      = fd;
	fds[n_fds].events  = events;
	connections[n_fds] = instance;
	return n_fds++;
}

void rtsp_server::removeFd(int fd)
{
	for (int i = 0; i < n_fds; ++i) {
		if (fds[i].fd != fd)
			continue;
		pthread_mutex_lock(&poll_mutex);
		memmove(&fds[i], &fds[i+1], sizeof(fds) - (sizeof(pollfd) * (i+1)));
		memmove(&connections[i], &connections[i+1], sizeof(connections) - (sizeof(*connections) * (i+1)));
		pthread_mutex_unlock(&poll_mutex);
		break;
	}
}

void rtsp_server::setFdEvents(int fd, int events)
{
	pthread_mutex_lock(&poll_mutex);
	for (int i = 0; i < n_fds; ++i) {
		if (fds[i].fd != fd)
			continue;
		fds[i].events = events;
		break;
	}
	pthread_mutex_unlock(&poll_mutex);
}

void rtsp_server::wake()
{
	char v = 0;
	if (write(wakeupfd[1], &v, 1) < 0)
		;
}

void rtsp_server::acceptConnection()
{
	int fd = accept(serverfd, NULL, NULL);
	if (fd < 0)
		return;

	new rtsp_connection(this, fd);
}

rtsp_connection::rtsp_connection(rtsp_server *server, int fd)
	: server(server), fd(fd), rdbuf_len(0)
{
	pthread_mutex_init(&write_lock, 0);
	server->addFd(this, fd, POLLIN);
}

rtsp_connection::~rtsp_connection()
{
	while (!sessions.empty())
		delete sessions.begin()->second;
	server->removeFd(fd);
	close(fd);
	pthread_mutex_destroy(&write_lock);
}

void rtsp_connection::addSession(rtsp_session *session)
{
	sessions[session->session_id] = session;
}

void rtsp_connection::removeSession(rtsp_session *session)
{
	sessions.erase(session->session_id);
}

int rtsp_connection::readable()
{
	int rd = read(fd, rdbuf + rdbuf_len, sizeof(rdbuf) - rdbuf_len - 1);
	if (rd < 1)
		return -1;

	rdbuf_len += rd;

	rd = parse();
	if (rd < 0)
		return -1;

	if (rd) {
		if (rd < rdbuf_len)
			memmove(rdbuf, rdbuf + rd, rdbuf_len - rd);
		if (rd > rdbuf_len)
			rd = rdbuf_len;
		rdbuf_len -= rd;
	}

	return 0;
}

struct rtsp_message
{
	std::string version;
	std::map<std::string,std::string> headers;

	/* Request */
	std::string method, uri;

	/* Response */
	int response;
	std::string responseStr;

	rtsp_message()
		: response(0)
	{
	}

	rtsp_message(const rtsp_message &request, int code, const std::string &message)
	{
		version     = request.version;
		response    = code;
		responseStr = message;

		std::string cseq = request.header("cseq");
		if (!cseq.empty())
			setHeader("CSeq", cseq);

		setHeader("Content-Length", "0");
	}

	/* Case sensitive; request headers are inserted all lowercase */
	std::string header(const std::string &key) const
	{
		std::map<std::string,std::string>::const_iterator it = headers.find(key);
		if (it == headers.end())
			return std::string();
		return it->second;
	}

	void setHeader(const std::string &key, const std::string &value)
	{
		headers[key] = value;
	}

	void setHeader(const std::string &key, int value)
	{
		std::ostringstream s;
		s << value;
		setHeader(key, s.str());
	}
};

static char *split_line(char **save)
{
	char *re = *save;
	char *p = *save;
	if (!*p)
		return NULL;
	for (; *p; ++p) {
		if (*p == '\r') {
			*p = 0;
			*save = p+1;
			if (**save == '\n')
				(*save)++;
			break;
		} else if (*p == '\n') {
			*p = 0;
			*save = p+1;
			break;
		}
	}
	return re;
}

int rtsp_connection::parse()
{
	rdbuf[rdbuf_len] = 0;

	char *end = strstr(rdbuf, "\r\n\r\n");
	if (!end)
		return 0;

	/* We have an entire request */
	rtsp_message req;
	char *p = rdbuf;
	char *ln;
	while ((ln = split_line(&p))) {
		if (!*ln)
			break;

		if (req.method.empty()) {
			/* Request line */
			char *lp = ln;
			req.method = strsep(&lp, " ");
			req.uri = strsep(&lp, " ");
			req.version = strsep(&lp, " ");
			if (req.method.empty() || req.uri.empty() || req.version.empty()) {
				sendResponse(rtsp_message(req, 400, "Bad request"));
				return -1;
			}
			if (req.version != "RTSP/1.0") {
				sendResponse(rtsp_message(req, 505, "RTSP version not supported"));
				return -1;
			}
			continue;
		}

		char *value = strchr(ln, ':');
		if (!value) {
			/* XXX bad request */
			return -1;
		}
		*value = 0;
		value++;
		value += strspn(value, " \t");

		std::string key = ln;
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

		req.headers[key] = value;
	}

	bc_log("request: '%s' '%s' '%s'", req.method.c_str(), req.uri.c_str(), req.version.c_str());
	for (std::map<std::string,std::string>::iterator it = req.headers.begin(); it != req.headers.end(); ++it)
		bc_log("header: '%s' = '%s'", it->first.c_str(), it->second.c_str());

	std::string t = req.header("content-length");
	if (!t.empty() && t != "0") {
		bc_log("W: RTSP server does not support request bodies");
		sendResponse(rtsp_message(req, 500, "Request bodies not supported"));
		return -1;
	}

	if (req.header("cseq").empty()) {
		sendResponse(rtsp_message(req, 400, "CSeq required"));
		return -1;
	}

	t = req.header("require");
	if (!t.empty()) {
		rtsp_message response(req, 551, "Option not supported");
		response.setHeader("Unsupported", t);
		sendResponse(response);
		return -1;
	}

	int re = 0;
	if (req.method == "OPTIONS")
		re = handleOptions(req);
	else if (req.method == "DESCRIBE")
		re = handleDescribe(req);
	else if (req.method == "SETUP")
		re = handleSetup(req);
	else if (req.method == "TEARDOWN")
		re = handleTeardown(req);
	else if (req.method == "PLAY")
		re = handlePlay(req);
	else {
		rtsp_message response(req, 405, "Method not allowed");
		response.setHeader("Allow", "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY");
		sendResponse(response);
	}

	return (re < 0) ? re : (ptrdiff_t(end - rdbuf) + 4);
}

void rtsp_connection::sendResponse(const rtsp_message &response)
{
	char buf[2048] = { 0 };

	av_strlcatf(buf, sizeof(buf), "RTSP/1.0 %d %s\r\n", response.response, response.responseStr.c_str());
	for (std::map<std::string,std::string>::const_iterator it = response.headers.begin(); it != response.headers.end(); ++it)
		av_strlcatf(buf, sizeof(buf), "%s: %s\r\n", it->first.c_str(), it->second.c_str());
	av_strlcat(buf, "\r\n", sizeof(buf));

	bc_log("response: %s", buf);
	send(buf, strlen(buf));
}

int rtsp_connection::send(const char *buf, int size)
{
	int re = 0;
	int wr;
	pthread_mutex_lock(&write_lock);

	if (!wrbuf.empty()) {
		wrbuf.append(buf, size);
		goto end;
	}

	wr = write(fd, buf, size);
	if (wr < 0) {
		re = -1;
		goto end;
	}

	if (wr < size) {
		wrbuf.append(buf + wr, size - wr);
		server->setFdEvents(fd, POLLIN | POLLOUT);
		server->wake();
	}

end:
	pthread_mutex_unlock(&write_lock);
	return re;
}

int rtsp_connection::writable()
{
	int re = 0;
	pthread_mutex_lock(&write_lock);
	if (!wrbuf.empty()) {
		int wr = write(fd, wrbuf.c_str(), wrbuf.size());
		if (wr < 0) {
			re = -1;
		} else if ((unsigned)wr < wrbuf.size()) {
			wrbuf.erase(0, wr);
		} else
			wrbuf.clear();
	}

	if (wrbuf.empty())
		server->setFdEvents(fd, POLLIN);

	pthread_mutex_unlock(&write_lock);
	return re;
}

int rtsp_connection::handleOptions(rtsp_message &req)
{
	rtsp_message re(req, 200, "OK");
	re.setHeader("Public", "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY");
	sendResponse(re);
	return 0;
}

int rtsp_connection::handleDescribe(rtsp_message &req)
{
	char pathbuf[128];
	av_url_split(0, 0, 0, 0, 0, 0, 0, pathbuf, sizeof(pathbuf), req.uri.c_str());

	std::string path = pathbuf;
	if (path.empty()) {
		sendResponse(rtsp_message(req, 404, "Not found"));
		return 0;
	}

	if (path[path.size()-1] == '/')
		path.erase(path.size()-1);

	rtsp_stream *stream = rtsp_stream::findUri(path);
	if (!stream) {
		sendResponse(rtsp_message(req, 404, "Not found"));
		return 0;
	}

	rtsp_message response(req, 200, "OK");
	response.setHeader("Content-Type", "application/sdp");
	response.setHeader("Content-Length", stream->sdp.size());
	sendResponse(response);
	send(stream->sdp.c_str(), stream->sdp.size());

	return 0;
}

int rtsp_connection::handleSetup(rtsp_message &req)
{
	if (!req.header("session").empty()) {
		sendResponse(rtsp_message(req, 455, "Method not valid in this state"));
		return 0;
	}

	if (req.header("transport").empty()) {
		sendResponse(rtsp_message(req, 400, "Invalid request"));
		return 0;
	}

	char pathbuf[128];
	av_url_split(0, 0, 0, 0, 0, 0, 0, pathbuf, sizeof(pathbuf), req.uri.c_str());

	std::string path = pathbuf;
	rtsp_stream *stream = NULL;
	int streamid = -1;
	if (path.size() > 6 && path.compare(0, 6, "/live/") == 0) {
		int sep = path.find('/', 6);
		if (sep > 6) {
			stream = rtsp_stream::findUri(path.substr(0, sep));
			path = (sep+1 < path.size()) ? path.substr(sep+1) : std::string();
		}
	}

	if (path.size() > 9 && path.compare(0, 9, "streamid=") == 0) {
		char *e = 0;
		streamid = strtol(path.substr(9).c_str(), &e, 10);
		if (!e || *e)
			streamid = -1;
	}

	if (!stream || streamid >= stream->nb_streams) {
		sendResponse(rtsp_message(req, 404, "Not found"));
		return 0;
	} else if (streamid < 0) {
		sendResponse(rtsp_message(req, 451, "Parameter not understood"));
		return 0;
	}

	rtsp_session *session = new rtsp_session(this, stream, streamid);

	if (!session->parseTransport(req.header("transport"))) {
		sendResponse(rtsp_message(req, 461, "Unsupported transport"));
		delete session;
		return 0;
	}

	rtsp_message response(req, 200, "OK");
	response.setHeader("Session", session->session_id);
	response.setHeader("Transport", session->transport());
	sendResponse(response);

	return 0;
}

int rtsp_connection::handleTeardown(rtsp_message &req)
{
	return -1;
}

int rtsp_connection::handlePlay(rtsp_message &req)
{
	int session_id = strtol(req.header("session").c_str(), NULL, 10);
	std::map<int,rtsp_session*>::iterator it = sessions.find(session_id);
	if (it == sessions.end()) {
		sendResponse(rtsp_message(req, 454, "Session not found"));
		return 0;
	}

	rtsp_session *session = it->second;
	if (session->channel_rtp < 0 || session->channel_rtcp < 0) {
		sendResponse(rtsp_message(req, 455, "Method not valid in this state"));
		return 0;
	}

	session->setActive(true);

	sendResponse(rtsp_message(req, 200, "OK"));
	return 0;
}

std::map<std::string,rtsp_stream*> rtsp_stream::streams;
pthread_mutex_t rtsp_stream::streams_lock = PTHREAD_MUTEX_INITIALIZER;

rtsp_stream *rtsp_stream::create(struct bc_record *bc, AVFormatContext *ctx)
{
	rtsp_stream *st = new rtsp_stream;
	st->bc_rec = bc;

	std::ostringstream uris;
	uris << "/live/" << bc->id;
	st->uri = uris.str();

	char sdp[2048];
	int re;
	if ((re = av_sdp_create(&ctx, 1, sdp, sizeof(sdp))) < 0) {
		char error[512];
		av_strerror(re, error, sizeof(error));
		bc_dev_err(bc, "Cannot create SDP: %s", error);
	}
	st->sdp = sdp;
	st->nb_streams = ctx->nb_streams;

	pthread_mutex_lock(&streams_lock);
	std::map<std::string,rtsp_stream*>::iterator it = streams.find(st->uri);
	if (it != streams.end()) {
		/* XXX XXX XXX safe? */
		streams.erase(it);
	}

	streams[st->uri] = st;
	pthread_mutex_unlock(&streams_lock);

	return st;
}

rtsp_stream::rtsp_stream()
{
	pthread_mutex_init(&sessions_lock, 0);
}

void rtsp_stream::remove(struct bc_record *bc)
{
	pthread_mutex_lock(&streams_lock);
	for (std::map<std::string,rtsp_stream*>::iterator it = streams.begin(); it != streams.end(); ++it) {
		if (it->second->bc_rec == bc) {
			/* XXX massive terrible leak XXX XXX XXX XXX XXX XXX */
			streams.erase(it);
			break;
		}
	}
	pthread_mutex_unlock(&streams_lock);
}

rtsp_stream *rtsp_stream::findUri(const std::string &uri)
{
	rtsp_stream *st = NULL;

	pthread_mutex_lock(&streams_lock);
	std::map<std::string,rtsp_stream*>::iterator it = streams.find(uri);
	if (it != streams.end())
		st = it->second;
	pthread_mutex_unlock(&streams_lock);

	return st;
}

void rtsp_stream::addSession(rtsp_session *session)
{
	pthread_mutex_lock(&sessions_lock);
	sessions.push_back(session);
	pthread_mutex_unlock(&sessions_lock);
}

void rtsp_stream::removeSession(rtsp_session *session)
{
	pthread_mutex_lock(&sessions_lock);
	for (std::vector<rtsp_session*>::iterator it = sessions.begin(); it != sessions.end(); ++it) {
		if (*it == session) {
			sessions.erase(it);
			break;
		}
	}
	pthread_mutex_unlock(&sessions_lock);
}

void rtsp_stream::sendPackets(uint8_t *buf, int size)
{
	pthread_mutex_lock(&sessions_lock);
	for (int p = 0; p+4 <= size; ) {
		uint8_t *pkt    = buf + p;
		uint32_t pkt_sz = AV_RB32(pkt);

		if (pkt_sz > size - p - 4)
			pkt_sz = size - p - 4;

		pkt[0] = '$';
		AV_WB16(pkt + 2, (uint16_t)pkt_sz);

		for (std::vector<rtsp_session*>::iterator it = sessions.begin(); it != sessions.end(); ++it) {
			if (!(*it)->isActive())
				continue;
			pkt[1] = (uint8_t) (*it)->channel_rtp;
			(*it)->connection->send((char*)pkt, 4 + pkt_sz);
		}

		p += 4 + pkt_sz;
	}
	pthread_mutex_unlock(&sessions_lock);
}

rtsp_session::rtsp_session(rtsp_connection *c, rtsp_stream *s, int sid)
	: connection(c), stream(s), stream_id(sid), session_id(generate_id()),
	  channel_rtp(-1), channel_rtcp(-1), active(false)
{
	connection->addSession(this);
	stream->addSession(this);
}

rtsp_session::~rtsp_session()
{
	connection->removeSession(this);
	stream->removeSession(this);
}

int rtsp_session::generate_id()
{
	/* XXX */
	return 1;
}

bool rtsp_session::parseTransport(std::string str)
{
	char *p = const_cast<char*>(str.data());
	char *transport;

	while ((transport = strsep(&p, ","))) {
		char *x = transport;
		char *param = strsep(&x, ";");

		/* Only allow TCP for now */
		if (strcmp(param, "RTP/AVP/TCP") != 0)
			continue;

		bool acceptable = true;
		while ((param = strsep(&x, ";"))) {
			bc_log("transport param: %s", param);
			char *tmp;

			if (av_strstart(param, "mode=", NULL) && !strstr(param, "PLAY")) {
				acceptable = false;
				break;
			} else if (av_strstart(param, "interleaved=", (const char**)&tmp)) {
				sscanf(tmp, "%d-%d", &channel_rtp, &channel_rtcp);
				if (channel_rtp < 0 || channel_rtcp < 0) {
					acceptable = false;
					break;
				}
			}
		}

		if (!acceptable || channel_rtp < 0 || channel_rtcp < 0)
			continue;
		return true;
	}

	return false;
}

std::string rtsp_session::transport() const
{
	char tmp[64];
	snprintf(tmp, sizeof(tmp), "RTP/AVP/TCP;mode=\"PLAY\";interleaved=%d-%d",
	         channel_rtp, channel_rtcp);
	return tmp;
}

void rtsp_session::setActive(bool on)
{
	if (on == active)
		return;

	active = on;
}

