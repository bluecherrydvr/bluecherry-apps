#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <poll.h>
#include <arpa/inet.h>
#include <errno.h>

#include <map>
#include "bc-syslog.h"
#include "logc.h"
#include "trigger_server.h"
#include "trigger_processor.h"

trigger_server::trigger_server() :
    _bindFd(-1),
    _hangupThread(false),
    _threadIsUp(false)
{
    pthread_mutex_init(&processor_registry_lock, NULL);
}

trigger_server::~trigger_server()
{
    pthread_mutex_destroy(&processor_registry_lock);
}

void trigger_server::destroy()
{
    bc_log(Info, "TriggerServer: destroy() called");
    bc_log(Info, "TriggerServer: destroy() called - setting _hangupThread = true");
    _hangupThread = true;

    if (_threadIsUp) {
        pthread_join(_tid, NULL);
        _threadIsUp = false;
    }

    if (_bindFd != -1) {
        close(_bindFd);
        unlink(_socketPath.c_str());
        _bindFd = -1;
    }
}

int trigger_server::openBindListenUnixSocket(const std::string& socketPath)
{
    struct sockaddr_un addr;
    if (socketPath.length() >= sizeof(addr.sun_path)) {
        bc_log(Error, "Socket path too long: %s", socketPath.c_str());
        return -1;
    }

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        bc_log(Error, "Socket creation failed: %s", strerror(errno));
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);
    unlink(addr.sun_path);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        bc_log(Error, "Socket bind failed: %s", strerror(errno));
        close(fd);
        return -1;
    }

    if (listen(fd, 5) == -1) {
        bc_log(Error, "Socket listen failed: %s", strerror(errno));
        close(fd);
        return -1;
    }

    _bindFd = fd;
    _socketPath = socketPath;
    return 0;
}

int trigger_server::reconfigure(const std::string& socketPath)
{
    bc_log(Info, "TriggerServer: reconfigure() called");

    // Prevent double initialization
    if (_threadIsUp) {
        bc_log(Warning, "TriggerServer: reconfigure() skipped — thread is already running");
        return 0;
    }

    destroy();  // Clean up any previous socket or thread

    _hangupThread = false;  // Reset exit flag

    if (openBindListenUnixSocket(socketPath) != 0) {
        bc_log(Error, "Failed to bind trigger socket to %s", socketPath.c_str());
        return -1;
    }

    _threadIsUp = true;

    int ret = pthread_create(&_tid, NULL, [](void* arg) -> void* {
        bc_log(Info, "TriggerServer: Thread started");
        static_cast<trigger_server*>(arg)->servingLoop();
        return nullptr;
    }, this);

    if (ret != 0) {
        bc_log(Error, "Failed to create trigger thread: %s", strerror(ret));
        return -1;
    }

    bc_log(Info, "Trigger server reconfigured and bound to socket %s", socketPath.c_str());
    return 0;
}


void trigger_server::servingLoop()
{
    bc_log(Info, "Trigger server loop is running");
    bc_log(Info, "TriggerServer: servingLoop() entered, _hangupThread = %d", _hangupThread);


    while (!_hangupThread) {
        struct pollfd pfd;
        pfd.fd = _bindFd;
        pfd.events = POLLIN;

        int ret = poll(&pfd, 1, 1000);
        if (ret > 0 && (pfd.revents & POLLIN)) {
            int clientFd = accept(_bindFd, NULL, NULL);
            if (clientFd >= 0) {
                serveClient(clientFd);
            }
        }
    }

    bc_log(Info, "Trigger server loop exiting");
    bc_log(Info, "TriggerServer: servingLoop() exiting, _hangupThread = %d", _hangupThread);
}

void trigger_server::serveClient(int fd)
{
// Debug:
//    bc_log(Info, "TriggerServer: serveClient() started");

    char input[1024] = {0};
    ssize_t input_len = read(fd, input, sizeof(input) - 1);
    bc_log(Info, "TriggerServer: read ret %ld, input: '%s'", input_len, input);

    if (input_len <= 0) {
        bc_log(Warning, "TriggerServer: read failed or empty, closing");
        close(fd);
        return;
    }

    int camera_id = 0;
    char description[1024] = {0};
    sscanf(input, "%d %[^\n]", &camera_id, description);
    bc_log(Info, "TriggerServer: parsed camera_id=%d, description='%s'", camera_id, description);

    trigger_processor* proc = find_processor(camera_id);
    if (proc) {
// Debug:
//        bc_log(Info, "TriggerServer: found processor, calling trigger()");
        proc->trigger(description);
        dprintf(fd, "200 Ok\n");
    } else {
        bc_log(Warning, "TriggerServer: no processor for camera %d", camera_id);
        dprintf(fd, "404 Processor Not Found\n");
    }

    close(fd);
// Debug:
//    bc_log(Info, "TriggerServer: serveClient() complete");
}


void trigger_server::register_processor(int camera_id, trigger_processor* processor)
{
    pthread_mutex_lock(&processor_registry_lock);
    processor_registry[camera_id] = processor;
    pthread_mutex_unlock(&processor_registry_lock);

    bc_log(Info, "Registered trigger_processor for camera ID %d", camera_id);
}

void trigger_server::unregister_processor(int camera_id, trigger_processor* processor)
{
    pthread_mutex_lock(&processor_registry_lock);
    processor_registry.erase(camera_id);
    pthread_mutex_unlock(&processor_registry_lock);

    bc_log(Info, "Unregistered trigger_processor for camera ID %d", camera_id);
}

trigger_processor* trigger_server::find_processor(int camera_id)
{
    pthread_mutex_lock(&processor_registry_lock);
    auto it = processor_registry.find(camera_id);
    trigger_processor* processor = (it != processor_registry.end()) ? it->second : nullptr;
    pthread_mutex_unlock(&processor_registry_lock);

    if (!processor)
        bc_log(Warning, "Trigger requested for camera ID %d but no processor is registered", camera_id);

    return processor;
}


