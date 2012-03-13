/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __RTSP_H
#define __RTSP_H

#include <string>
#include <map>
#include <vector>
#include <pthread.h>

#define FD_MAX 256

enum WakeReason
{
	WAKE_REPOLL = 1 << 0, /* Restart the poll() call, probably due to setFdEvents */
	WAKE_GC     = 1 << 1  /* Garbage collect, such as deleting dead streams and sessions */
};

class rtsp_server
{
public:
	static rtsp_server *instance;

	rtsp_server();
	~rtsp_server();

	int setup(int port);
	void run();

	void wake(int reason);

	static void *runThread(void *p);

private:
	friend class rtsp_connection;

	int serverfd;
	int wakeupfd[2];
	pthread_mutex_t poll_mutex;
	struct pollfd fds[FD_MAX];
	class rtsp_connection *connections[FD_MAX];
	int n_fds;

	void acceptConnection();

	/* valid ONLY on the RTSP thread */
	int addFd(rtsp_connection *instance, int fd, int events);
	void removeFd(int fd);
	/* Threadsafe (as long as you can guarantee fd is open) */
	void setFdEvents(int fd, int events);
};

struct rtsp_message;
class rtsp_session;

class rtsp_connection
{
public:
	rtsp_connection(rtsp_server *server, int fd);
	~rtsp_connection();

	int readable();
	int writable();

	void addSession(rtsp_session *session);
	void removeSession(rtsp_session *session);

private:
	rtsp_server * const server;
	const int fd;
	char rdbuf[10240];
	int rdbuf_len;
	pthread_mutex_t write_lock;
	std::string wrbuf;

	std::map<int,rtsp_session*> sessions;

	int parse();
	void sendResponse(const rtsp_message &response);
	int send(const char *buf, int size);

	/* TMP */
	friend class rtsp_stream;

	int handleOptions(rtsp_message &request);
	int handleDescribe(rtsp_message &request);
	int handleSetup(rtsp_message &request);
	int handleTeardown(rtsp_message &request);
	int handlePlay(rtsp_message &request);
	int handlePause(rtsp_message &request);
};

class rtsp_stream
{
public:
	std::string sdp;
	std::string uri;
	int nb_streams;

	/* Thread-safe; should be called from recording threads */
	static rtsp_stream *create(struct bc_record *rec, AVFormatContext *rtpctx);
	static void remove(struct bc_record *rec);

	static rtsp_stream *findUri(const std::string &uri);

	void addSession(rtsp_session *session);
	void removeSession(rtsp_session *session);
	void sessionActiveChanged(rtsp_session *session);

	int activeSessionCount() { return _activeSessionCount; } 
	void sendPackets(uint8_t *buf, int bufsz, int flags);

	/* Invoked to delete dead streams on the RTSP thread after a (thread-safe) remove() */
	static void collectGarbage();

private:
	friend class rtsp_connection;

	static pthread_mutex_t streams_lock;
	static std::map<std::string,rtsp_stream*> streams;

	rtsp_stream();
	~rtsp_stream();

	/* Use with extreme caution; we cannot guarantee that it still exists
	 * when running from the RTSP thread */
	struct bc_record *bc_rec;
	pthread_mutex_t sessions_lock;
	std::vector<rtsp_session*> sessions;
	int _activeSessionCount;
};

class rtsp_session
{
public:
	rtsp_connection * const connection;
	rtsp_stream * const stream;
	const int stream_id;
	const int session_id;

	/* For TCP interleaving */
	int channel_rtp, channel_rtcp;
	bool needKeyframe;

	rtsp_session(rtsp_connection *connection, rtsp_stream *stream, int streamid);
	~rtsp_session();

	bool parseTransport(std::string str);
	std::string transport() const;

	void setActive(bool active);
	bool isActive() const { return active; }

private:
	bool active;

	static int generate_id();
};

#endif

