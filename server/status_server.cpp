#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <poll.h>
#include <arpa/inet.h>
#include <assert.h>

#include "bc-syslog.h"
#include "logc.h"
#include "status_server.h"
#include "xml_string_writer.h"

status_server::status_server(StatusCallback cb) :
	_callback(cb)
	, _bindFd(-1)
	, _hangupThread(false)
	, _threadIsUp(false)
{ }

void status_server::destroy() {
	_hangupThread = true;

	if (_threadIsUp) {
		pthread_join(_tid, NULL);
		_threadIsUp = false;
	}
	if (_bindFd != -1) {
		close(_bindFd);
		_bindFd = -1;
	}
}

int status_server::openBindListenUnixSocket(const std::string& socketPath) {
	struct sockaddr_un addr;
	if (socketPath.length() >= sizeof(addr.sun_path)) {
		bc_log(Error, "Socket path %s too long (max %zu)", socketPath.c_str(), sizeof(addr.sun_path) - 1);
		return -1;
	}
	int fd;
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) {
		bc_log(Error, "Creating UNIX socket failed");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", socketPath.c_str());
	addr.sun_family = AF_UNIX;
	unlink(addr.sun_path);  //!< Remove possibly previously existing file
	int ret;
	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		bc_log(Error, "Binding to %s failed", socketPath.c_str());
		close(fd);
		return -1;
	}

	ret = listen(fd, 100);
	if (ret == -1) {
		bc_log(Error, "Listening to %s failed", socketPath.c_str());
		close(fd);
		return -1;
	}
	return fd;
}

bool status_server::socketWaitReadable(int fd, int timeout_ms) {
	struct pollfd p;
	p.fd = fd;
	p.events = POLLIN;
	p.revents = 0;
	int ret;
	ret = poll(&p, 1, timeout_ms);
	if ((ret < 0) || (p.revents & (POLLERR | POLLHUP)))
		return true;
	return p.revents & p.events;
}

static void *servingLoopWrapper(void *arg) {
	pthread_setname_np(pthread_self(), "STATUS");
	status_server *self = reinterpret_cast<status_server*>(arg);
	self->servingLoop();
	return NULL;
}

int status_server::reconfigure(const std::string& socketPath) {
	int ret;
	_socketPath = socketPath;
	destroy();
	_hangupThread = false;
	_bindFd = openBindListenUnixSocket(_socketPath);
	if (_bindFd == -1) {
		bc_log(Error, "Failed to bind to socket %s", _socketPath.c_str());
		return 1;
	}
	bc_log(Info, "Status reports are served at %s", _socketPath.c_str());
	ret = pthread_create(&_tid, NULL, servingLoopWrapper, this);
	if (ret == -1) {
		bc_log(Error, "Failed to start thread");
		return 1;
	}
	_threadIsUp = true;

	return 0;
}

void status_server::servingLoop() {
	while (!_hangupThread) {
		bool newData = socketWaitReadable(_bindFd, 100);
		if (newData) {
			sockaddr_storage clientAddr;
			socklen_t addrlen = sizeof(clientAddr);
			int clientFd = accept4(_bindFd, (struct sockaddr*)&clientAddr, &addrlen, SOCK_NONBLOCK);
			if (clientFd == -1) {
				bc_log(Error, "Got connection, but failed to accept");
				continue;
			}
			serveClient(clientFd);
		}
	}
}

void status_server::serveClient(int fd) {
	// Take from server the full XML
	pugi::xml_document xml;
	_callback(xml);

	// Dump to text. Pity that pugixml output API is so poor in methods
	xml_string_writer writer;
	xml.save(writer);
	const char* unprocessed_data = writer.result.c_str();
	int unprocessed_data_size = writer.result.length();

	// Write it to client until completion or failure. Consider waiting >1s in EAGAINs failure.
	int total_sleep = 0;
	while (unprocessed_data_size
			&& fd != -1) {
		assert(unprocessed_data_size > 0);
		ssize_t written = write(fd, unprocessed_data, unprocessed_data_size);
		if (written == 0) {
			bc_log(Error, "Status client closed input connection while being served");
			close(fd);
			fd = -1;
			return;
		} else if (written == -1 && errno == EAGAIN) {
			bc_log(Error, "Caught EAGAIN, waiting a bit");
			usleep(10000);
			total_sleep += 10000;
			if (total_sleep > 100*1000) {
				bc_log(Error, "Too much totally waiting while pushing status report to client, disconnecting");
				close(fd);
				fd = -1;
				return;
			}
		} else if (written == -1) {
			bc_log(Error, "Error on status client connection, errno %d (%s)", errno, strerror(errno));
			close(fd);
			fd = -1;
			return;
		} else {
			unprocessed_data_size -= written;
			unprocessed_data += written;
		}
	}
	close(fd);
}
