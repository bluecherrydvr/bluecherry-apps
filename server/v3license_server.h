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

#ifndef __V3LICENSE_SERVER_H
#define __V3LICENSE_SERVER_H

#ifdef V3_LICENSING
#include <string>
#include <map>
#include <vector>
#include <queue>

#include <pthread.h>
#include <poll.h>
#include <inttypes.h>

#define V3LICENSE_FD_MAX 256
#define BUF_MAX 256
#define QUEUE_MAX_V3LICENSE_SOCK 10
#define MIN_ARG_CNT_V3LICENSE 1
#define MAX_ARG_CNT_V3LICENSE 4

typedef struct {
    int socket = -1;
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
} license_thread_context_t;

class v3license_server
{
public:
    static v3license_server *instance;

    v3license_server();
    ~v3license_server();

    int setup(int port);
    void run();

    static license_thread_context_t *thread_context;
    static void* runThread(void* p);
    size_t splitArgument(const std::string &txt, std::vector<std::string> &strs, char ch);
    static void stopThread();

private:
    void acceptConnection();

public:
    static int initedLex;
    static int running;

private:
    int serverfd;
};

#endif /* V3_LICENSING */
#endif /* __V3LICENSE_SERVER_H */
