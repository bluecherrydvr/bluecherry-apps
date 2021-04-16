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

v3license_server::v3license_server()
	: serverfd(-1)
{
	if (!instance)
		instance = this;
}

v3license_server::~v3license_server()
{
	close(serverfd);
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

	return 0;

error:
	close(serverfd);
	serverfd = -1;
	return -1;
}
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * socketThread(void *arg)
{
	int newSocket = *((int *)arg);
	char client_message[BUF_MAX];
	char buffer[BUF_MAX];
	recv(newSocket , client_message , BUF_MAX, 0);

	// Send message to the client socket 
	pthread_mutex_lock(&lock);
	char *message = (char*)malloc(sizeof(client_message)+20);
	if (V3_LICENSE_OK == bc_license_v3_check())
	{
		strcpy(message,"license is OK: ");
	}
	else
	{
		strcpy(message,"license is FAIL : ");
	}
	strcat(message,client_message);
	strcat(message,"\n");
	strcpy(buffer,message);
	free(message);
	pthread_mutex_unlock(&lock);
	send(newSocket,buffer,13,0);
	printf("Exit socketThread \n");
	close(newSocket);
	pthread_exit(NULL);
}


void *v3license_server::runThread(void *p)
{
	while (true)
	{
		int server = static_cast<v3license_server*>(p)->serverfd;

		if (server < 0)
			return NULL;

		int client = accept(server, NULL, NULL);
		if (client < 0) {
			bc_log(Error, "accept() failed: %s", strerror(errno));
			return NULL;
		}
		pthread_t thread;
		if( pthread_create(&thread, NULL, socketThread, &client) != 0 )
			printf("Failed to create thread\n");
		usleep(10);
	}

	return NULL;
}

#endif /* V3_LICENSING */
