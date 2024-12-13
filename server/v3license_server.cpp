/*
 * Copyright 2010-2021 Bluecherry, LLC
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
#include "bc-server.h"

#ifdef V3_LICENSING
#include "v3license_server.h"
#include "v3license_processor.h"

v3license_server* v3license_server::instance = NULL;
license_thread_context_t* v3license_server::thread_context = 0;
int v3license_server::initedLex = LA_FAIL;
int v3license_server::running = 0;

v3license_server::v3license_server()
    : serverfd(-1)
{
    if (!instance) {
        instance = this;
    }

    initedLex = bc_license_v3_Init();
    thread_context = new license_thread_context_t();
    running = 0;
}

v3license_server::~v3license_server()
{
    close(serverfd);
    free(thread_context);
}

int v3license_server::setup(int port)
{
    int ret;

    if (initedLex != LA_OK) {
        bc_log(Error, "Failed to initialize v3license feature (Error code: %d)", initedLex);
        return -1;
    }

    int domain = AF_INET6;
    struct sockaddr_storage addr_stor = {0, };
    const struct sockaddr *addr_ptr = (const sockaddr *) &addr_stor;
    size_t addr_len = sizeof(addr_stor);

    serverfd = socket(domain, SOCK_STREAM, 0);
    if (serverfd < 0) {
        domain = AF_INET;
        serverfd = socket(domain, SOCK_STREAM, 0);
    }

    if (serverfd < 0) {
        bc_log(Error, "Failed to create license server socket: %s", strerror(errno));
        return -1;
    }

    const int yes = 1, no = 0;
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        bc_log(Error, "Failed to set SO_REUSEADDR option on the license server socket: %s",
               strerror(errno));
        goto error;
    }

    if (domain == AF_INET6) {
        if (setsockopt(serverfd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0) {
            bc_log(Error, "Failed to set IPV6_V6ONLY option on the license server socket: %s",
                   strerror(errno));
            goto error;
        }

        struct sockaddr_in6 *addr = (sockaddr_in6 *) &addr_stor;
        addr->sin6_port   = htons(port);
        addr->sin6_family = AF_INET6;
        addr->sin6_addr   = in6addr_any;
    }
    else {
        struct sockaddr_in *addr = (sockaddr_in *) &addr_stor;
        addr->sin_port   = htons(port);
        addr->sin_family = AF_INET;
        addr->sin_addr.s_addr = htonl(INADDR_ANY);
    }

    if (bind(serverfd, addr_ptr, addr_len) < 0) {
        bc_log(Error, "Failed to bind license server socket: %s", strerror(errno));
        goto error;
    }

    if (listen(serverfd, QUEUE_MAX_V3LICENSE_SOCK) < 0) {
        bc_log(Error, "Failed to listen license server socket: %s", strerror(errno));
        goto error;
    }

    ret = fcntl(serverfd, F_SETFL, O_NONBLOCK);
    assert(!ret);
    return ret;

error:
    close(serverfd);
    serverfd = -1;
    return -1;
}

size_t v3license_server::splitArgument(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

    return strs.size();
}

void* socketThread(void *arg)
{
    license_thread_context_t* context = (license_thread_context_t*)arg;
    pthread_mutex_t* lock = &context->lock;
    int newSocket = context->socket;
    char client_message[BUF_MAX] = {0};
    char command[BUF_MAX] = {0};
    char message[BUF_MAX] = {0};
    std::vector<std::string> vec;
    size_t argCount = 0;
    int status = LA_FAIL;

    char *param[MAX_ARG_CNT_V3LICENSE];

    int size = recv(newSocket , client_message , BUF_MAX, 0);
    if (size <= 0) {
        bc_log(Error, "Failed to receive message from client socket: %s",
               strerror(errno));
        close(newSocket);
        return NULL;
    }

    // Send message to the client socket 
    if (pthread_mutex_lock(lock) != 0) {
        bc_log(Error, "Failed to mutex lock for thread: %s", strerror(errno));
        pthread_mutex_unlock(lock);
        close(newSocket);
        return NULL;
    }

    if (!v3license_server::instance) {
        bc_log(Error, "Failed to get the v3license server instance: %s",
               strerror(errno));
        pthread_mutex_unlock(lock);
        close(newSocket);
        return NULL;
    }

    argCount = v3license_server::instance->splitArgument(client_message, vec, ' ');
    if (argCount > MAX_ARG_CNT_V3LICENSE || argCount < MIN_ARG_CNT_V3LICENSE) {
        bc_log(Error, "Failed to get the v3license command argument: %s",
               strerror(errno));
        pthread_mutex_unlock(lock);
        close(newSocket);
        return NULL;
    }

    for (uint32_t i = 0; i < vec.size(); i++) {
        param[i] = (char*) malloc(BUF_MAX);
        snprintf(param[i], BUF_MAX, "%s", vec.at(i).c_str());
    }

    snprintf(command, sizeof(command), "%s", param[0]);

    if (strcmp(command, "bc_v3_license_isActivated") == 0) {
        status = bc_license_v3_IsActivated();
        snprintf(message, sizeof(message), "%s %d\n", command, status);
    }
    else if (strcmp(command, "bc_v3_license_isLicenseGenuine") == 0) {
        status = bc_license_v3_IsLicenseGenuine();
        snprintf(message, sizeof(message), "%s %d\n", command, status);
    }
    else if (strcmp(command, "bc_v3_license_IsTrialGenuine") == 0) {
        status = bc_license_v3_IsTrialGenuine();
        snprintf(message, sizeof(message), "%s %d\n", command, status);
    }
    else if (strcmp(command, "bc_v3_license_GetLicenseMetadata") == 0) {
        char licenseMeta[BUF_MAX] = {0};
        status = bc_license_v3_GetLicenseMetadata(licenseMeta, BUF_MAX);
        snprintf(message, sizeof(message), "%s %d %s\n",
                 command, status, licenseMeta);
    }
    else if (strcmp(command, "bc_v3_license_GetLicenseExpiryDate") == 0) {
        int32_t isUnlimited = 0;
        int32_t date = 0;
        status = bc_license_v3_GetLicenseExpiryDate(&isUnlimited, &date);
        snprintf(message, sizeof(message), "%s %d %d %d\n",
                 command, status, isUnlimited, date);
    }
    else if (strcmp(command, "bc_v3_license_GetTrialExpiryDate") == 0) {
        uint32_t trialDate = 0;
        status = bc_license_v3_GetTrialExpiryDate(&trialDate);
        snprintf(message, sizeof(message), "%s %d %d\n",
                 command, status, trialDate);
    }
    else if (strcmp(command, "bc_v3_license_ActivateLicense") == 0) {
        if (!param[1]) {
            bc_log(Error, "License key is null or empty");
            snprintf(message, sizeof(message), "%s %d\n", command, LA_FAIL);
        }
        else {
            status = bc_license_v3_ActivateLicense(param[1]);
            snprintf(message, sizeof(message), "%s %d\n", command, status);

            if (status == LA_OK) {
                char licenseMeta[BUF_MAX] = {0};
                bc_license_v3_GetLicenseMetadata(licenseMeta, BUF_MAX);
            }
        }
    }
    else if (strcmp(command, "bc_v3_license_ActivateTrial") == 0) {
        status = bc_license_v3_ActivateTrial();
        snprintf(message, sizeof(message), "%s %d\n", command, status);
    }
    else if (strcmp(command, "bc_v3_license_DeactivateLicense") == 0) {
        status = bc_license_v3_DeactivateLicense();
        snprintf(message, sizeof(message), "%s %d\n", command, status);
    }
    else {
        bc_log(Error, "The %s license API is unspported", command);
    }

    for (uint32_t i = 0; i < vec.size(); i++) {
        if (param[i]) {
            free(param[i]);
            param[i] = NULL;
        }
    }

    pthread_mutex_unlock(lock);

    if (send(newSocket, message, strlen(message), 0) < 0) {
        bc_log(Error, "Failed to send message from client socket: %s",
               strerror(errno));
    }

    // Exit the thread
    close(newSocket);

    return NULL;
}

void* v3license_server::runThread(void* p)
{
    int server = static_cast<v3license_server*>(p)->serverfd;

    if (server < 0) {
        bc_log(Error, "v3license server cast failed: %s", strerror(errno));
        return NULL;
    }

    running = 1;
    bc_log(Info, "v3license server started");

    while (running) {
        // Accept the new client request
        int client = accept(server, NULL, NULL);
        if (client < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                bc_log(Error, "accept() failed: %s", strerror(errno));
            }
            sleep(1);
            continue;
        }

        thread_context->socket = client;
        bc_log(Info, "A new license request is accepted.");

        // Create a new thread to process the client request
        pthread_t threadId;
        int err = pthread_create(&threadId, NULL, socketThread, thread_context);
        if (err != 0) {
            bc_log(Error, "Failed to create a new v3license thread: %s",
                   strerror(errno));
            sleep(1);
            continue;
        }

        bc_log(Debug, "Created a new v3license thread.");

        // Detach the new thread
        err = pthread_detach(threadId);
        if (err != 0) {
            bc_log(Error, "Failed to detach v3license thread: %s",
                   strerror(errno));
            sleep(1);
            continue;
        }

    }

    bc_log(Info, "v3license server stopped");

    return NULL;
}

void v3license_server::stopThread()
{
    running = 0;
}

#endif /* V3_LICENSING */
