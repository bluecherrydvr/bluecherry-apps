/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <bsd/string.h>
#include <unistd.h>
#include "bc-server.h"
#include "rtsp.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <assert.h>

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
	awakening = false;
	pthread_mutex_init(&wake_lock, NULL);
}

rtsp_server::~rtsp_server()
{
	for (int i = 0; i < n_fds; ++i)
		delete connections[i];
	close(serverfd);
	close(wakeupfd[0]);
	close(wakeupfd[1]);
	pthread_mutex_destroy(&wake_lock);
}

int rtsp_server::setup(int port)
{
	int domain = AF_INET6;
	struct sockaddr_storage addr_stor = {0, };
	const struct sockaddr *addr_ptr = (const sockaddr *) &addr_stor;
	size_t addr_len = sizeof(addr_stor);

	serverfd = socket(domain, SOCK_STREAM, 0);
	if (serverfd < 0) {
		domain = AF_INET;
		serverfd = socket(domain, SOCK_STREAM, 0);
	}
	if (serverfd < 0)
		return -1;

	const int yes = 1, no = 0;
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		goto error;

	if (domain == AF_INET6) {
		if (setsockopt(serverfd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0)
			goto error;

		struct sockaddr_in6 *addr = (sockaddr_in6 *) &addr_stor;
		addr->sin6_port   = htons(port);
		addr->sin6_family = AF_INET6;
		addr->sin6_addr   = in6addr_any;
	} else {
		struct sockaddr_in *addr = (sockaddr_in *) &addr_stor;
		addr->sin_port   = htons(port);
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = htonl(INADDR_ANY);
	}
	if (bind(serverfd, addr_ptr, addr_len) < 0)
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
		int n = poll(fds, n_fds, 100/*ms*/);

		if (n <= 0) {
			if (n == 0 || errno == EINTR)
				continue;
			bc_log(Error, "poll error: %s", strerror(errno));
			continue;
		}

		for (int i = 0; i < n_fds; ++i) {
			if (!fds[i].revents)
				continue;

			rtsp_connection *c = connections[i];

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
				pthread_mutex_lock(&wake_lock);
				awakening = false;
				uint8_t buf[1];
				int rd = read(wakeupfd[0], buf, sizeof(buf));
				if (rd < 0) {
					bc_log(Error, "Error on RTSP wakeup fd: %s", strerror(errno));
					assert(0);
				}
				pthread_mutex_unlock(&wake_lock);
				rtsp_stream::collectGarbage();

				// Send queued packets on each stream
				rtsp_stream::allSendQueued();

			} else {
				bc_log(Bug, "Unknown FD in RTSP poll");
				fds[i].events = 0;
				assert(0);
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
	for (int i = 0; i < n_fds; ++i) {
		if (fds[i].fd != fd)
			continue;

		memmove(&fds[i], &fds[i+1], sizeof(fds) - (sizeof(pollfd) * (i+1)));
		memmove(&connections[i], &connections[i+1], sizeof(connections) - (sizeof(*connections) * (i+1)));
		n_fds--;
		break;
	}
}

void rtsp_server::setFdEvents(int fd, int events)
{
	// O(n) time, likely frequently-called procedure. TODO improve performance.
	for (int i = 0; i < n_fds; ++i) {
		if (fds[i].fd != fd)
			continue;
		fds[i].events = events;
		break;
	}
}

void rtsp_server::wake()
{
	pthread_mutex_lock(&wake_lock);
	if (!awakening) {
		uint8_t v;
		if (write(wakeupfd[1], &v, 1) < 0)
			bc_log(Error, "Error writing to wakeup fd: %s", strerror(errno));
		awakening = true;
	}
	pthread_mutex_unlock(&wake_lock);
}

void rtsp_server::acceptConnection()
{
	int fd = accept(serverfd, NULL, NULL);
	if (fd < 0) {
		bc_log(Error, "accept() failed: %s", strerror(errno));
		return;
	}

	new rtsp_connection(this, fd);
}

void rtsp_server::getStatusXml(pugi::xml_node &node)
{
       node.append_attribute("n_connections") = n_fds - 2;
       // TODO Much more info. Requires extending rtsp_* classes to hold more info.
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
		bc_log(Debug, "%s", msg);
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
	// The called destructor goes back to rtsp_connection object and erases itself from `sessions`. Nasty loop. TODO Simplify objects relations
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
		bc_log(Debug, "Connection read buffer exceeded; dropped connection");
		return -1;
	}

	ssize_t rd = read(fd, rdbuf + rdbuf_len, sizeof(rdbuf) - rdbuf_len - 1);
	if (rd < 1)
		return -1;

	rdbuf_len += rd;

	unsigned int parsed = parse();
	if (parsed < 0)
		return -1;

	if (parsed) {
		if (parsed < rdbuf_len)
			memmove(rdbuf, rdbuf + parsed, rdbuf_len - parsed);
		if (parsed > rdbuf_len)
			parsed = rdbuf_len;
		rdbuf_len -= parsed;
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

static char *memsep(char **ptr, char sep, char *limit)
{
	if (!**ptr)
		return NULL;

	char *ret = *ptr;
	char *p = *ptr;

	for (; p < limit && *p; ++p) {
		if (*p == sep) {
			*ptr = p + 1;
			*p = 0;
			break;
		}
	}

	return ret;
}

static char *split_line(char **ptr, char *limit)
{
	if (!**ptr)
		return NULL;

	char *ret = *ptr;
	char *p = *ptr;

	for (char v; p < limit && *p; ++p) {
		v = *p;
		if (v == '\r' || v == '\n') {
			*ptr = p + 1 + (v == '\r' && *(p + 1) == '\n');
			*p = 0;
			break;
		}
	}
	return ret;
}

ssize_t rtsp_connection::parse()
{
	const unsigned rdbuf_lim = sizeof(rdbuf) / sizeof(rdbuf[0]);

	if (rdbuf_len >= rdbuf_lim) {
		bc_log(Bug, "rdbuf_len >= rdbuf_lim");
		rdbuf_len = rdbuf_lim - 1;
	}

	rdbuf[rdbuf_len] = 0;

	/* XXX Can't be replaced by strnstr because Ubuntu Precise's libbsd
	 * doesn't implement it :( */
	char *end = strstr(rdbuf, "\r\n\r\n");
	if (!end)
		return 0;

	/* We have an entire request */
	rtsp_message req;
	char *p = rdbuf;
	char *ln;
	while ((ln = split_line(&p, rdbuf + rdbuf_lim))) {
		if (!*ln)
			break;

		if (req.method.empty()) {
			/* Request line */
			char *lp = ln;
			req.method = memsep(&lp, ' ', rdbuf + rdbuf_lim);
			req.uri = memsep(&lp, ' ', rdbuf + rdbuf_lim);
			req.version = memsep(&lp, ' ', rdbuf + rdbuf_lim);
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

		char *value = (char *)memchr(ln, ':', rdbuf_lim - (ln - rdbuf));
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
		bc_log(Warning, "RTSP server does not support request bodies");
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
	// Yeah, where's the fuckin error handling, dudes? TODO FIXME
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
		if (EAGAIN == errno || EWOULDBLOCK == errno) {
			wr = 0;
		} else {
			re = -1;
			goto end;
		}
	}

	if (wr < size) {
		/* Copy the entire buffer and set pos to mark it as partially sent */
		wrbuf = new rtsp_write_buffer(buf, size, type, flag);
		wrbuf->pos = wr;
		wrbuf_tail = wrbuf;

		// deadlock avoidance, preserving locking order
		pthread_mutex_unlock(&write_lock);
		server->setFdEvents(fd, POLLIN | POLLOUT);
		pthread_mutex_lock(&write_lock);
		server->wake();
	}

end:
	if (wrbuf && rtsp_write_buffer::trim(wrbuf) >= 1024*1024*2) {
		/* If the buffer reaches 2MB and cannot be trimmed any further,
		 * drop the connection. There is little hope in this case anyway. */
		bc_log(Debug, "Buffer size exceeded on stream, dropping connection (client too slow?)");
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
		int wr = write(fd, wrbuf->data + wrbuf->pos,
			       wrbuf->size - wrbuf->pos);
		if (wr < 0) {
			if (EAGAIN == errno || EWOULDBLOCK == errno) {
				wr = 0;
			} else {
				re = -1;
				break;
			}
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
		// deadlock avoidance, preserving locking order
		pthread_mutex_unlock(&write_lock);
		server->setFdEvents(fd, POLLIN);
		pthread_mutex_lock(&write_lock);
	}

	pthread_mutex_unlock(&write_lock);
	return re;
}

bool rtsp_connection::authenticate(rtsp_message &req, int device_id)
{
	if (req.uri.find("authtoken=") != std::string::npos) {
		std::string token = req.uri.substr(req.uri.find("authtoken=") + 10);
		token = token.substr(0, token.find('/'));  // Drop possible trailing '/streamid=0' etc.
		BC_DB_RES dbres = bc_db_get_table("SELECT * FROM RtspAuthTokens JOIN Users ON RtspAuthTokens.user_id = Users.id WHERE RtspAuthTokens.token='%s'", token.c_str());
		if (dbres) {
			if (bc_db_fetch_row(dbres)) {
				bc_db_free_table(dbres);
			} else {
				bc_db_free_table(dbres);
				return true;
			}
		}
		// Fallthrough to usual auth
	}

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
		sendResponse(rtsp_message(req, 400, "Bad request"));
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
		unsigned int sep = path.find('/', 6);
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

rtsp_stream *rtsp_stream::create(struct bc_record *bc, AVFormatContext *ctx[])
{
	rtsp_stream *st = new rtsp_stream;
	st->bc_rec = bc;
	st->id = bc->id;

	std::ostringstream uris;
	uris << "/live/" << bc->id;
	st->uri = uris.str();

	char sdp[2048];
	int re;

	st->nb_streams = ctx[1] ? 2 : 1;
	if ((re = av_sdp_create(ctx, st->nb_streams, sdp, sizeof(sdp))) < 0) {
		char error[512];
		av_strerror(re, error, sizeof(error));
		bc->log.log(Error, "Cannot create SDP for streaming: %s", error);
	}
	st->sdp = sdp;

	pthread_mutex_lock(&streams_lock);
	std::map<std::string,rtsp_stream*>::iterator it = streams.find(st->uri);
	if (it != streams.end()) {
		/* XXX XXX XXX safe? */
		// TODO Check correctness
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
	pthread_mutex_init(&queue_lock, 0);
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

	// Free all queued packets
	while (!queue.empty()) {
		rtsp_packet *rtsp_packet = queue.front();
		queue.pop();
		delete[] rtsp_packet->data;
		delete rtsp_packet;
	}

	pthread_mutex_destroy(&queue_lock);
}

/* This is called from arbitrary threads (the recording thread), but
 * cleanup MUST be done on the RTSP thread, to properly resolve other relationships.
 * We use the rtsp_server's wake() to call into collectGarbage on
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
		rtsp_server::instance->wake();
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
		it = streams.begin();  // TODO FIXME go to next, not to begin
	}
	pthread_mutex_unlock(&streams_lock);
}

void rtsp_stream::allSendQueued()
{
	pthread_mutex_lock(&streams_lock);
	for (std::map<std::string,rtsp_stream*>::iterator it = streams.begin(); it != streams.end(); ++it) {
		(it->second)->sendQueuedPackets();
	}
	pthread_mutex_unlock(&streams_lock);
}

rtsp_stream *rtsp_stream::findUri(std::string uri)
{
	rtsp_stream *st = NULL;

	/* for /live/ URLs, we ignore everything after the first two segments
	 * to allow streamid and other parameters */
	if (uri.size() > 6 && uri.compare(0, 6, "/live/") == 0) {
		size_t p = uri.find('/', 6);
		if (p != std::string::npos)
			uri.erase(p);
	}

	uri = uri.substr(0, uri.find("?"));

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

void rtsp_stream::sendPackets(uint8_t *buf, unsigned int size, int flags, int stream_index)
{
	// Queue the data to execute sending logics from rtsp_server::run() thread.

	// Form the packet
	// TODO (a far one) incorporate server/streaming.cpp into RTSP serving stuff
	uint8_t *pkt = new uint8_t[size + 4];
	if (!pkt) {
		bc_log(Fatal, "Failed to allocate memory for data streaming");
		assert(0);
		// TODO retcode
	}

	pkt[0] = '$';
	pkt[1] = stream_index * 2;
	AV_WB16(pkt + 2, size);
	memcpy(pkt + 4, buf, size);
	// AV_WB16(pkt + 6, rtp_seq++);  // Overwritten on sending to individual session basing on its counter

	// TODO Move `first` determination up to the calling function, outside of there
	bool first = bc_rec->pkt_first_chunk;
	bc_rec->pkt_first_chunk = false;

	// Fill the structure
	rtsp_packet *rtsp_pkt = new rtsp_packet;
	rtsp_pkt->data = pkt;
	rtsp_pkt->size = size + 4;
	rtsp_pkt->is_key = flags & AV_PKT_FLAG_KEY;
	rtsp_pkt->is_first_chunk = first;

	// Lock stream queue
	pthread_mutex_lock(&queue_lock);
	// Append new packet to the queue
	queue.push(rtsp_pkt);
	// Unlock stream queue
	pthread_mutex_unlock(&queue_lock);

	// wake server
	rtsp_server::instance->wake();
}

void rtsp_stream::sendQueuedPackets()
{
	// Lock stream queue
	pthread_mutex_lock(&queue_lock);

	while (!queue.empty()) {
		rtsp_packet *rtsp_packet = queue.front();
		queue.pop();

		pthread_mutex_lock(&sessions_lock);
		for (std::vector<rtsp_session*>::iterator it = sessions.begin(); it != sessions.end(); ++it) {
			if (!(*it)->isActive() || ((*it)->needKeyframe && !rtsp_packet->is_key))
				continue;
			AV_WB16(rtsp_packet->data + 6, (*it)->rtp_seq++);  // TODO Drop per-session seq counter?
			(*it)->connection->send((char*)rtsp_packet->data, rtsp_packet->size, rtsp_packet->is_key ?
					rtsp_write_buffer::Reference :
					rtsp_write_buffer::Inter,
					rtsp_packet->is_first_chunk ?
					rtsp_write_buffer::First :
					rtsp_write_buffer::Partial);
			/* If we're not on keyframe-only mode, unset the needKeyframe flag.
			 * Otherwise, it always stays true. */
			(*it)->needKeyframe = (*it)->keyframeOnly;
		}
		pthread_mutex_unlock(&sessions_lock);

		delete[] rtsp_packet->data;
		delete rtsp_packet;
	}
	// Unlock stream queue
	pthread_mutex_unlock(&queue_lock);
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
	bc_log(Debug, "Stream ended for device %d", stream->id);
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

	bc_log(Debug, "Stream %s for device %d", on ? "started" : "stopped", stream->id);
}

