/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef BC_SERVER_SOCKETS_H
#define BC_SERVER_SOCKETS_H

#define BC_STATUS_SOCKET_PATH "/tmp/bluecherry_status"
#define BC_TRIGGER_SOCKET_PATH "/tmp/bluecherry_trigger"

#ifdef __cplusplus
extern "C" {
#endif

void bc_server_unlink_socket_paths(void);

#ifdef __cplusplus
}
#endif

#endif
