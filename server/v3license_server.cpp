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
#include "bc-server.h"
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

#ifdef V3_LICENSING
#include "v3license_server.h"
#include "v3license_processor.h"

v3license_server *v3license_server::instance = 0;
license_thread_context_t *v3license_server::thread_context = 0;

v3license_server::v3license_server()
	: serverfd(-1)
{
	if (!instance)
	{
		instance = this;
	}
	thread_context = new license_thread_context_t();
}

v3license_server::~v3license_server()
{
	close(serverfd);
	if (thread_context)
	{
		free(thread_context);
	}
}

int v3license_server::setup(int port)
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
	{
		bc_log(Error, "Failed to create license server socket: %s", strerror(errno));
		return -1;
	}

	const int yes = 1, no = 0;
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		bc_log(Error, "Failed to set SO_REUSEADDR option on the license server socket: %s", strerror(errno));
		goto error;
	}

	if (domain == AF_INET6) {
		if (setsockopt(serverfd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0)
		{
			bc_log(Error, "Failed to set IPV6_V6ONLY option on the license server socket: %s", strerror(errno));
			goto error;
		}

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
	{
		bc_log(Error, "Failed to bind license server socket: %s", strerror(errno));
		goto error;
	}

	if (listen(serverfd, QUEUE_MAX_V3LICENSE_SOCK) < 0)
	{
		bc_log(Error, "Failed to listen license server socket: %s", strerror(errno));
		goto error;
	}

	return 0;

error:
	close(serverfd);
	serverfd = -1;
	return -1;
}

void * socketThread(void *arg)
{
	license_thread_context_t *context = (license_thread_context_t*)arg;
	pthread_mutex_t *lock = &context->lock;
	int newSocket = context->socket;
	char client_message[BUF_MAX] = { 0 };
	char message[BUF_MAX] = { 0 };

	// Receive the message from the client socket
	int size = recv(newSocket , client_message , BUF_MAX, 0);
	if (size <= 0)
	{
		bc_log(Error, "Failed to receive message from client socket: %s", strerror(errno));
		return NULL;
	}

	// Send message to the client socket 
	pthread_mutex_lock(lock);
	int status = bc_license_v3_check();
	snprintf(message, sizeof(message), "%d", status);
	pthread_mutex_unlock(lock);

	if (send(newSocket, message, strlen(message), 0) < 0)
	{
		bc_log(Error, "Failed to send message from client socket: %s", strerror(errno));
	}

	// Exit the thread
	bc_log(Info, "Exit socketThread %d.", newSocket);
	close(newSocket);
	pthread_exit(NULL);
}

void *v3license_server::runThread(void *p)
{
	int server = static_cast<v3license_server*>(p)->serverfd;

	if (server < 0)
	{
		bc_log(Error, "v3license server cast failed: %s", strerror(errno));
		return NULL;
	}

	while (true)
	{
		int client = accept(server, NULL, NULL);
		if (client < 0) {
			bc_log(Error, "accept() failed: %s", strerror(errno));
			return NULL;
		}

		thread_context->socket = client;

		pthread_t thread;
		if( pthread_create(&thread, NULL, socketThread, thread_context) != 0 )
		{
			bc_log(Error, "Failed to create v3license thread: %s", strerror(errno));
		}
	}

	return NULL;
}

#endif /* V3_LICENSING */
