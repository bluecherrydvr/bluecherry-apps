/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include "bc-server.h"
#include "rtsp.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>
#include "libbluecherry.h"

extern "C" {
#include <libavutil/avstring.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/base64.h>
}

rtsp_server *rtsp_server::instance = 0;

rtsp_server::rtsp_server()
	: serverfd(-1), n_fds(0)
{
	if (!instance)
		instance = this;

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
	serverfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (serverfd < 0)
		return -1;

	int yes = 1;
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		goto error;

	struct sockaddr_in6 addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin6_port   = htons(port);
	addr.sin6_family = AF_INET6;
	addr.sin6_addr   = in6addr_any;
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

	int errors = 0;
	for (;;) {
		pthread_mutex_lock(&poll_mutex);
		int n = poll(fds, n_fds, -1);
                pthread_mutex_unlock(&poll_mutex);

		if (n < 0) {
			if (errno == EINTR)
				continue;
			bc_log("E(RTSP): poll() error: %s", strerror(errno));
			/* This is nowhere near ideal. We should reset everything instead, but
			 * this is at least better than silent failure, because the process will
			 * restart. */
			if (++errors == 10)
				exit(1);
			else
				continue;
		}

		for (int i = 0; i < n_fds; ++i) {
			if (!fds[i].revents)
				continue;

			rtsp_connection *c = connections[i];

			/* It is not safe to write fds[i], or do much of anything, after calling into
			 * other code, because FDs may have been added or removed in that time.
			 *
			 * We don't have to worry about missing events; if they aren't handled, they'll
			 * poll again immediately. */

			int revents = fds[i].revents;
			fds[i].revents = 0;

			if (fds[i].fd == serverfd) {
				acceptConnection();
			} else if (c) {
				int re = -1;
				if (revents & POLLOUT)
					re = c->writable();
				else if (revents & POLLIN)
					re = c->readable();

				if (re < 0) {
					/* Close the connection and remove it from polling */
					delete c;
					--i;
					continue;
				}
			} else if (fds[i].fd == wakeupfd[0]) {
				char buf[128];
				int rd = read(wakeupfd[0], buf, sizeof(buf));
				int reason = 0;
				if (rd < 0)
					bc_log("W(RTSP): Error on wakeup fd: %s", strerror(errno));
				for (int k = 0; k < rd; ++k)
					reason |= buf[k];

				if (reason & WAKE_GC)
					rtsp_stream::collectGarbage();
			} else {
				bc_log("W(RTSP): BUG: Unknown FD");
				fds[i].events = 0;
			}
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
	pthread_mutex_lock(&poll_mutex);
	for (int i = 0; i < n_fds; ++i) {
		if (fds[i].fd != fd)
			continue;

		memmove(&fds[i], &fds[i+1], sizeof(fds) - (sizeof(pollfd) * (i+1)));
		memmove(&connections[i], &connections[i+1], sizeof(connections) - (sizeof(*connections) * (i+1)));
		n_fds--;
		break;
	}
	pthread_mutex_unlock(&poll_mutex);
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

void rtsp_server::wake(int reason)
{
	char v = reason;
	if (write(wakeupfd[1], &v, 1) < 0)
		bc_log("W(RTSP): Error writing to wakeup fd: %s", strerror(errno));
}

void rtsp_server::acceptConnection()
{
	int fd = accept(serverfd, NULL, NULL);
	if (fd < 0) {
		bc_log("E(RTSP): accept() failed: %s", strerror(errno));
		switch (errno) {
		case EMFILE:
		case ENFILE:
			bc_log("E(RTSP): out of fds, killing process!");
			exit(1);
		}
		return;
	}

	new rtsp_connection(this, fd);
}

/* To survive low bandwidth connections, we need to be smart about
 * buffering; it's important to never drop RTSP commands, and we
 * shouldn't drop partially sent frames either. When a new keyframe
 * is buffered, we can skip sending any unsent inter-frames to avoid
 * falling too far behind realtime.
 *
 * Each rtsp_write_buffer is one unit to send on an rtsp_connection,
 * which will be sent in whole or (if applicable) dropped entirely.
 *
 */
struct rtsp_write_buffer
{
	enum Type
	{
		Control, /* Control data, not droppable */
		Reference, /* Reference frame */
		Inter, /* Inter frame, droppable if followed by a reference */
	};

	enum FrameFlag
	{
		Only, /* Only buffer in the frame */
		First, /* First buffer in the frame */
		Partial /* Following buffers in the same frame */
	};

	rtsp_write_buffer *next;
	char *data;
	int size, pos;
	Type type;
	FrameFlag flag;

	rtsp_write_buffer(const char *d, int s, int t, int f)
		: next(0), size(s), pos(0), type((Type)t), flag((FrameFlag)f)
	{
		data = new char[size];
		memcpy(data, d, size);
	}

	~rtsp_write_buffer()
	{
		delete[] data;
	}

	void print_buffer(const char *prefix)
	{
		char msg[1024];
		strcpy(msg, prefix);
		int i = strlen(msg);

		for (rtsp_write_buffer *p = this; p; p = p->next) {
			char c = '?';
			switch (p->type) {
			case Control:   c = 'c'; break;
			case Reference: c = 'r'; break;
			case Inter:     c = 'i'; break;
			}
			if (p->flag == Only || p->flag == First)
				c = toupper(c);
			else if (p->flag != Partial)
				c = '!';
			msg[i++] = c;
		}
		msg[i] = 0;
		bc_log("%s", msg);
	}

	static int trim(rtsp_write_buffer *&head)
	{
		int size = 0;
		for (rtsp_write_buffer *p = head; p; p = p->next) {
			size += p->size - p->pos;
			if (p->type != Reference || p->flag == Partial)
				continue;

			/* On each reference frame, drop all previous unsent reference
			 * or inter frames */
			rtsp_write_buffer *last = 0;
			bool hasFirst = false;
			for (rtsp_write_buffer *c = head; c != p; ) {
				rtsp_write_buffer *next = c->next;
				if (!c->pos && (c->type == Reference || c->type == Inter) && (c->flag != Partial || hasFirst)) {
					if (c->flag == First)
						hasFirst = true;
					size -= c->size - c->pos;
					delete c;
				} else if (last) {
					last->next = c;
					last = c;
				} else
					last = head = c;
				c = next;
			}

			if (last)
				last->next = p;
			else
				last = head = p;
		}

		return size;
	}
};

rtsp_connection::rtsp_connection(rtsp_server *server, int fd)
	: server(server), fd(fd), rdbuf_len(0), wrbuf(0), wrbuf_tail(0)
{
	pthread_mutex_init(&write_lock, 0);
	server->addFd(this, fd, POLLIN);

	/* Increase the send buffer to 128kB, which should be large enough to hold at least one
	 * frame. This is more efficient, because it doesn't require waking the poll thread twice. */
	int value = 128*1024;
	setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, sizeof(value));
}

rtsp_connection::~rtsp_connection()
{
	while (!sessions.empty())
		delete sessions.begin()->second;
	server->removeFd(fd);
	close(fd);
	for (rtsp_write_buffer *p = wrbuf, *n; p; p = n) {
		n = p->next;
		delete p;
	}
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
	if (rdbuf_len+1 >= sizeof(rdbuf)) {
		bc_log("I(RTSP): Connection read buffer exceeded; dropped connection");
		return -1;
	}

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
			sendResponse(rtsp_message(req, 400, "Bad request"));
			return -1;
		}
		*value = 0;
		value++;
		value += strspn(value, " \t");

		std::string key = ln;
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

		req.headers[key] = value;
	}

	std::string t = req.header("content-length");
	if (!t.empty() && t != "0") {
		bc_log("W(RTSP): RTSP server does not support request bodies");
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
	else if (req.method == "PAUSE")
		re = handlePause(req);
	else {
		rtsp_message response(req, 405, "Method not allowed");
		response.setHeader("Allow", "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE");
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

	/* XXX error handling */
	send(buf, strlen(buf));
}

int rtsp_connection::send(const char *buf, int size, int type, int flag)
{
	int re = 0;
	int wr;
	pthread_mutex_lock(&write_lock);

	if (wrbuf) {
		/* rtsp_write_buffer makes a copy of the data */
		rtsp_write_buffer *wbuf = new rtsp_write_buffer(buf, size, type, flag);
		wrbuf_tail->next = wbuf;
		wrbuf_tail = wbuf;
		goto end;
	}

	wr = write(fd, buf, size);
	if (wr < 0) {
		re = -1;
		goto end;
	}

	if (wr < size) {
		/* Copy the entire buffer and set pos to mark it as partially sent */
		wrbuf = new rtsp_write_buffer(buf, size, type, flag);
		wrbuf->pos = wr;
		wrbuf_tail = wrbuf;

		server->setFdEvents(fd, POLLIN | POLLOUT);
		server->wake(WAKE_REPOLL);
	}

end:
	if (wrbuf && rtsp_write_buffer::trim(wrbuf) >= 1024*1024*2) {
		/* If the buffer reaches 2MB and cannot be trimmed any further,
		 * drop the connection. There is little hope in this case anyway. */
		bc_log("I(RTSP): Buffer size exceeded on stream, dropping connection");
		shutdown(fd, SHUT_RDWR);
		re = -1;
	}

	pthread_mutex_unlock(&write_lock);
	return re;
}

int rtsp_connection::writable()
{
	int re = 0;
	pthread_mutex_lock(&write_lock);

	while (wrbuf) {
		int wr = write(fd, wrbuf->data + wrbuf->pos, wrbuf->size - wrbuf->pos);
		if (wr < 0) {
			re = -1;
			break;
		}
		wrbuf->pos += wr;
		if (wrbuf->pos >= wrbuf->size) {
			rtsp_write_buffer *next = wrbuf->next;
			delete wrbuf;
			wrbuf = next;
		} else
			break;
	}

	if (!wrbuf) {
		wrbuf_tail = 0;
		server->setFdEvents(fd, POLLIN);
	}

	pthread_mutex_unlock(&write_lock);
	return re;
}

bool rtsp_connection::authenticate(rtsp_message &req, int device_id)
{
	std::string auth = req.header("authorization");
	if (auth.size() > 6 && auth.substr(0, 6) == "Basic ") {
		auth = auth.substr(6);
		char *buf = new char[auth.size()];
		int r = av_base64_decode((uint8_t*)buf, auth.c_str(), auth.size());
		if (r > 0) {
			buf[r] = 0;
			char *password = buf;
			char *username = strsep(&password, ":");
			if (username && password &&
			    bc_user_auth(username, password, ACCESS_REMOTE, device_id) == 1)
			{
				delete[] buf;
				return true;
			}
		}
		delete[] buf;
	}

	rtsp_message re(req, 401, "Authorization Required");
	re.setHeader("WWW-Authenticate", "Basic realm=\"RTSP Server\"");
	sendResponse(re);
	return false;
}

int rtsp_connection::handleOptions(rtsp_message &req)
{
	rtsp_message re(req, 200, "OK");
	re.setHeader("Public", "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE");
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
	if (!authenticate(req, stream ? stream->id : -1))
		return 0;

	if (!stream) {
		sendResponse(rtsp_message(req, 404, "Not found"));
		return 0;
	}

	rtsp_message response(req, 200, "OK");
	response.setHeader("Content-Type", "application/sdp");
	response.setHeader("Content-Length", stream->sdp.size());
	sendResponse(response);
	if (send(stream->sdp.c_str(), stream->sdp.size()) < 0)
		return -1;

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

	if (!authenticate(req, stream ? stream->id : -1))
		return 0;

	if (!stream) {
		sendResponse(rtsp_message(req, 404, "Not found"));
		return 0;
	}

	rtsp_session *session = new rtsp_session(this, stream);

	if (!session->parseParameters(path) || session->stream_id < 0) {
		sendResponse(rtsp_message(req, 451, "Parameter not understood"));
		delete session;
		return 0;
	}

	if (streamid >= stream->nb_streams) {
		sendResponse(rtsp_message(req, 404, "Not found"));
		delete session;
		return 0;
	}

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

/* TEARDOWN is defined as operating on the URI, not the Session header;
 * I'm not sure if it's appropriate to destroy all sessions matching the URI
 * if no Session header is present, or what. Currently, we just destroy the
 * single session provided by the header and ignore the URI. */
int rtsp_connection::handleTeardown(rtsp_message &req)
{
	int session_id = strtol(req.header("session").c_str(), NULL, 10);
	std::map<int,rtsp_session*>::iterator it = sessions.find(session_id);
	if (it == sessions.end()) {
		sendResponse(rtsp_message(req, 454, "Session not found"));
		return 0;
	}

	rtsp_session *session = it->second;
	session->setActive(false);
	delete session;

	sendResponse(rtsp_message(req, 200, "OK"));
	return 0;
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

int rtsp_connection::handlePause(rtsp_message &req)
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

	session->setActive(false);

	sendResponse(rtsp_message(req, 200, "OK"));
	return 0;
}

std::map<std::string,rtsp_stream*> rtsp_stream::streams;
pthread_mutex_t rtsp_stream::streams_lock = PTHREAD_MUTEX_INITIALIZER;

rtsp_stream *rtsp_stream::create(struct bc_record *bc, AVFormatContext *ctx)
{
	rtsp_stream *st = new rtsp_stream;
	st->bc_rec = bc;
	st->id = bc->id;

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
	: _activeSessionCount(0)
{
	pthread_mutex_init(&sessions_lock, 0);
}

rtsp_stream::~rtsp_stream()
{
	while (!sessions.empty()) {
		/* There's no way to signal the end of a particular interleaved channel to clients,
		 * nor the end of a particular session. Since none of our use cases have more than
		 * one session per connection right now, the easiest way to handle this is to close
		 * the connection. If this is a problem in the future, RTCP BYE may help.
		 * The destructor for rtsp_connection will delete the session, which will remove
		 * itself from the sessions list on this stream. */
		delete (*sessions.begin())->connection;
	}
	pthread_mutex_destroy(&sessions_lock);
}

/* This is called from arbitrary threads (the recording thread), but
 * cleanup MUST be done on the RTSP thread, to properly resolve other relationships.
 * We use the rtsp_server's wake() and WAKE_GC flag to call into collectGarbage on
 * the RTSP thread. This pattern assumes that each rtsp_stream is only used from one
 * arbitrary (recording) thread and the RTSP thread. */
void rtsp_stream::remove(struct bc_record *bc)
{
	pthread_mutex_lock(&streams_lock);
	for (std::map<std::string,rtsp_stream*>::iterator it = streams.begin(); it != streams.end(); ++it) {
		if (it->second->bc_rec == bc) {
			/* Flag for garbage collection */
			it->second->bc_rec = 0;
			break;
		}
	}
	if (rtsp_server::instance)
		rtsp_server::instance->wake(WAKE_GC);
	pthread_mutex_unlock(&streams_lock);
}

void rtsp_stream::collectGarbage()
{
	pthread_mutex_lock(&streams_lock);
	for (std::map<std::string,rtsp_stream*>::iterator it = streams.begin(); it != streams.end(); ) {
		if (it->second->bc_rec) {
			it++;
			continue;
		}

		delete it->second;
		streams.erase(it);
		it = streams.begin();
	}
	pthread_mutex_unlock(&streams_lock);
}

rtsp_stream *rtsp_stream::findUri(std::string uri)
{
	rtsp_stream *st = NULL;

	/* for /live/ URLs, we ignore everything after the first two segments
	 * to allow streamid and other parameters */
	if (uri.size() > 6 && uri.compare(0, 6, "/live/") == 0) {
		int p = uri.find('/', 6);
		if (p != std::string::npos)
			uri.erase(p);
	}

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
	if (session->isActive())
		_activeSessionCount++;
	pthread_mutex_unlock(&sessions_lock);
}

void rtsp_stream::removeSession(rtsp_session *session)
{
	pthread_mutex_lock(&sessions_lock);

	int active = 0;
	std::vector<rtsp_session*>::iterator erase = sessions.end();
	for (std::vector<rtsp_session*>::iterator it = sessions.begin(); it != sessions.end(); ++it) {
		if (*it == session)
			erase = it;
		else if ((*it)->isActive())
			active++;
	}

	if (erase != sessions.end())
		sessions.erase(erase);
	_activeSessionCount = active;

	pthread_mutex_unlock(&sessions_lock);
}

void rtsp_stream::sessionActiveChanged(rtsp_session *session)
{
	if (session->isActive())
		_activeSessionCount++;
	else
		_activeSessionCount--;
}

void rtsp_stream::sendPackets(uint8_t *buf, int size, int flags)
{
	pthread_mutex_lock(&sessions_lock);
	bool first = true;
	for (int p = 0; p+4 <= size; ) {
		uint8_t *pkt    = buf + p;
		uint32_t pkt_sz = AV_RB32(pkt);

		if (pkt_sz > size - p - 4)
			pkt_sz = size - p - 4;

		/* The packet must be at least 8 bytes to hold the RTP header */
		if (pkt_sz < 12) {
			p += 4 + pkt_sz;
			continue;
		}

		/* The packet buffer is prefixed with a 4-byte length by the output
		 * code; this is NOT part of the actual RTP stream. We can replace
		 * this with the four-byte header used by RTSP's interleaved TCP mode.
	 	 */
		/* If the RTP packet_type is 200 or 201, this is a RTCP packet.
		 * This data is totally wrong for clients; we need to rewrite these for
		 * each stream. Drop them for now. */
		if (pkt[5] == 200 || pkt[5] == 201) {
			p += 4 + pkt_sz;
			continue;
		}

		pkt[0] = '$';
		AV_WB16(pkt + 2, (uint16_t)pkt_sz);

		for (std::vector<rtsp_session*>::iterator it = sessions.begin(); it != sessions.end(); ++it) {
			if (!(*it)->isActive() || ((*it)->needKeyframe && !(flags & AV_PKT_FLAG_KEY)))
				continue;
			/* TCP interleaving channel */
			pkt[1] = (uint8_t) (*it)->channel_rtp;
			/* Rewrite RTP sequence number */
			AV_WB16(pkt + 6, (*it)->rtp_seq++);
			/* Send packet, including the interleaving header */
			(*it)->connection->send((char*)pkt, 4 + pkt_sz, (flags & AV_PKT_FLAG_KEY) ?
			                                                 rtsp_write_buffer::Reference :
									 rtsp_write_buffer::Inter,
									(first) ?
									 rtsp_write_buffer::First :
									 rtsp_write_buffer::Partial);
			/* If we're not on keyframe-only mode, unset the needKeyframe flag.
			 * Otherwise, it always stays true. */
			(*it)->needKeyframe = (*it)->keyframeOnly;
		}

		p += 4 + pkt_sz;
		first = false;
	}
	pthread_mutex_unlock(&sessions_lock);
}

rtsp_session::rtsp_session(rtsp_connection *c, rtsp_stream *s, int sid)
	: connection(c), stream(s), session_id(generate_id()), stream_id(sid),
	  channel_rtp(-1), channel_rtcp(-1), needKeyframe(true), keyframeOnly(false),
	  active(false)
{
	/* Sequence IDs start from a random value */
	rtp_seq = (uint16_t)rand();
	connection->addSession(this);
	stream->addSession(this);
}

rtsp_session::~rtsp_session()
{
	connection->removeSession(this);
	stream->removeSession(this);
	bc_log("I(RTSP): Stream ended for device %d", stream->id);
}

int rtsp_session::generate_id()
{
	/* XXX this is supposed to be 8-octets of randomness */
	return rand();
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

bool rtsp_session::parseParameters(std::string str)
{
	char *p = const_cast<char*>(str.data());
	char *param;

	while ((param = strsep(&p, "/"))) {
		char *value = param;
		strsep(&value, "=");
		/* value may be null, or empty */

		if (strcasecmp(param, "streamid") == 0 && value) {
			char *e = 0;
			stream_id = strtol(value, &e, 10);
			if (!e || *e)
				stream_id = -1;
		} else if (strcasecmp(param, "mode") == 0 && value) {
			if (strcasecmp(value, "keyframe") == 0)
				keyframeOnly = true;
			else if (strcasecmp(value, "full") == 0)
				keyframeOnly = false;
		}
	}

	return true;
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
	needKeyframe = on;
	stream->sessionActiveChanged(this);

	bc_log("I(RTSP): Stream %s for device %d", on ? "started" : "stopped", stream->id); 
}

