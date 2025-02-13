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
#include "trigger_server.h"
#include "trigger_processor.h"

trigger_server::trigger_server() :
	_bindFd(-1)
	, _hangupThread(false)
	, _threadIsUp(false)
{
	pthread_mutex_init(&processor_registry_lock, NULL);
}

trigger_server::~trigger_server()
{
	pthread_mutex_destroy(&processor_registry_lock);
}

void trigger_server::destroy() {
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

int trigger_server::openBindListenUnixSocket(const std::string& socketPath) {
	// TODO Deduplicate with status_server
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

bool trigger_server::socketWaitReadable(int fd, int timeout_ms) {
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
	pthread_setname_np(pthread_self(), "TRIGGER");
	trigger_server *self = reinterpret_cast<trigger_server*>(arg);
	self->servingLoop();
	return NULL;
}

int trigger_server::reconfigure(const std::string& socketPath) {
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

void trigger_server::servingLoop() {
	while (!_hangupThread) {
		bool newData = socketWaitReadable(_bindFd, 100);
		if (newData) {
			sockaddr_storage clientAddr;
			socklen_t addrlen = sizeof(clientAddr);
			int clientFd = accept(_bindFd, (struct sockaddr*)&clientAddr, &addrlen);
			if (clientFd == -1) {
				bc_log(Error, "Got connection, but failed to accept");
				continue;
			}
			serveClient(clientFd);
		}
	}
}

void trigger_server::serveClient(int fd) {
	// Read from socket
	char input[1024];
	int input_len;
	int camera_id;
	char *description = NULL;

	// In spite of simplification, there's no loop, just assume that sent data arrives at once
	input_len = read(fd, input, sizeof(input)-1);
	bc_log(Error, "trigger_server: read ret %d", input_len);
	if (input_len < 0) {
		bc_log(Error, "trigger_server: errno %d", errno);
		close(fd);
		return;
	}

	// Stringize the input
	input[input_len] = NULL;

	// Parse input
	errno = 0;
	camera_id = strtol(input, &description, 10);
	if (errno) {
		dprintf(fd, "400 Bad Request; syntax: <camera_id> <description string>");
		close(fd);
		return;
	}

	if (description) {
		description++;
		if (description >= input + input_len)
			description = NULL;
	}

	// Find trigger_processor and execute special method
	trigger_processor *proc = find_processor(camera_id);
	if (!proc) {
		dprintf(fd, "404 Processor Not Found");
		close(fd);
		return;
	}

	proc->trigger(description);
	// Write request's status information to socket
	dprintf(fd, "200 Ok");
	// Close socket
	close(fd);
}

void trigger_server::register_processor(int camera_id, trigger_processor *processor)
{
	pthread_mutex_lock(&processor_registry_lock);
	processor_registry.insert(std::pair<int, trigger_processor*>(camera_id, processor));
	pthread_mutex_unlock(&processor_registry_lock);
}

void trigger_server::unregister_processor(int camera_id, trigger_processor *processor)
{
	pthread_mutex_lock(&processor_registry_lock);
	processor_registry.erase(camera_id);
	pthread_mutex_unlock(&processor_registry_lock);
}

trigger_processor *trigger_server::find_processor(int camera_id)
{
	trigger_processor *ret = NULL;
	std::map<int, trigger_processor*>::iterator iter;

	pthread_mutex_lock(&processor_registry_lock);
	iter = processor_registry.find(camera_id);
	if (iter != processor_registry.end())
		ret = iter->second;
	pthread_mutex_unlock(&processor_registry_lock);

	return ret;
}
