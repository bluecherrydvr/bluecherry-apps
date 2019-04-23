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

#ifndef __RTSP_H
#define __RTSP_H

#include <string>
#include <map>
#include <vector>
#include <queue>

#include <pthread.h>
#include <poll.h>
#include <inttypes.h>

#include <pugixml.hpp>

#define FD_MAX 256

class rtsp_connection;

class rtsp_server
{
public:
	static rtsp_server *instance;

	rtsp_server();
	~rtsp_server();

	int setup(int port);
	void run();

	void wake();

	static void *runThread(void *p);

	/* valid ONLY on the RTSP thread */
	int addFd(rtsp_connection *instance, int fd, int events);
	void removeFd(int fd);
	/* Threadsafe (as long as you can guarantee fd is open) */
	void setFdEvents(int fd, int events);

	void getStatusXml(pugi::xml_node &node);
private:

	int serverfd;

	int wakeupfd[2];
	bool awakening;
	pthread_mutex_t wake_lock;

	struct pollfd fds[FD_MAX];
	class rtsp_connection *connections[FD_MAX];
	int n_fds;

	void acceptConnection();
};

struct rtsp_message;
class rtsp_session;
class rtsp_write_buffer;

class rtsp_connection
{
public:
	rtsp_connection(rtsp_server *server, int fd);
	~rtsp_connection();

	int readable();
	int writable();

	void addSession(rtsp_session *session);
	void removeSession(rtsp_session *session);

	int send(const char *buf, int size, int type = 0, int flags = 0);

private:
	rtsp_server * const server;
	const int fd;
	char rdbuf[10240];
	unsigned int rdbuf_len;
	pthread_mutex_t write_lock;
	rtsp_write_buffer *wrbuf, *wrbuf_tail;

	std::map<int,rtsp_session*> sessions;

	ssize_t parse();
	void sendResponse(const rtsp_message &response);

	bool authenticate(rtsp_message &request, int device_id = -1);

	int handleOptions(rtsp_message &request);
	int handleDescribe(rtsp_message &request);
	int handleSetup(rtsp_message &request);
	int handleTeardown(rtsp_message &request);
	int handlePlay(rtsp_message &request);
	int handlePause(rtsp_message &request);
};

struct rtsp_packet
{
	uint8_t *data;
	int size;
	bool is_key;
	bool is_first_chunk;
};

class rtsp_stream
{
public:
	std::string sdp;
	std::string uri;
	int id;
	int nb_streams;

	/* Thread-safe; should be called from recording threads */
	static rtsp_stream *create(struct bc_record *rec, AVFormatContext *rtpctx[]);
	static void remove(struct bc_record *rec);

	static rtsp_stream *findUri(std::string uri);

	int activeSessionCount() { return _activeSessionCount; } 
	void sendPackets(uint8_t *buf, unsigned int bufsz, int flags, int stream_index);

	/* Invoked to delete dead streams on the RTSP thread after a (thread-safe) remove() */
	static void collectGarbage();
	static void allSendQueued();

	void addSession(rtsp_session *session);
	void removeSession(rtsp_session *session);
	void sessionActiveChanged(rtsp_session *session);

private:

	static pthread_mutex_t streams_lock;
	static std::map<std::string,rtsp_stream*> streams;

	rtsp_stream();
	~rtsp_stream();
	void sendQueuedPackets();


	/* Use with extreme caution; we cannot guarantee that it still exists
	 * when running from the RTSP thread */
	struct bc_record *bc_rec;
	pthread_mutex_t sessions_lock;
	std::vector<rtsp_session*> sessions;
	int _activeSessionCount;
	pthread_mutex_t queue_lock;
	std::queue<rtsp_packet*> queue;
};

class rtsp_session
{
public:
	rtsp_connection * const connection;
	rtsp_stream * const stream;
	const int session_id;
	int stream_id;
	uint16_t rtp_seq;

	/* For TCP interleaving */
	int channel_rtp, channel_rtcp;
	bool needKeyframe, keyframeOnly;

	rtsp_session(rtsp_connection *connection, rtsp_stream *stream, int streamid = -1);
	~rtsp_session();

	bool parseTransport(std::string str);
	std::string transport() const;

	bool parseParameters(std::string str);

	void setActive(bool active);
	bool isActive() const { return active; }

private:
	bool active;

	static int generate_id();
};

#endif

