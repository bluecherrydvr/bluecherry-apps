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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "libbluecherry.h"
#include <bsd/string.h>
#include "logging.h"

extern "C" {
#include <libavutil/md5.h>
}

time_t bc_gettime_monotonic()
{
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0) {
		bc_log(Bug, "clock_gettime failed");
		return 0;
	}

	return ts.tv_sec;
}

int bc_user_auth(const char *username, const char *password, int access_type, int device_id)
{
	int re = 0;
	char md5_pwd[16], s_password[256];
	const char *tmp, *access_field = 0;
	char *s_username = bc_db_escape_string(username, strlen(username));
	BC_DB_RES dbres = bc_db_get_table("SELECT * FROM Users WHERE username='%s'", s_username);
	free(s_username);
	if (!dbres)
		return -1;

	if (bc_db_fetch_row(dbres))
		goto end;

	strlcpy(s_password, password, sizeof(s_password));
	strlcat(s_password, bc_db_get_val(dbres, "salt", NULL), sizeof(s_password));
	av_md5_sum((uint8_t*)md5_pwd, (const uint8_t*)s_password, (int)strlen(s_password));
	hex_encode(s_password, sizeof(s_password), md5_pwd, sizeof(md5_pwd));

	tmp = bc_db_get_val(dbres, "password", NULL);
	if (strcmp(tmp, s_password) != 0)
		goto end;

	switch (access_type) {
		case ACCESS_SETUP:  access_field = "access_setup";
		case ACCESS_REMOTE: access_field = "access_remote";
		case ACCESS_WEB:    access_field = "access_web";
		case ACCESS_BACKUP: access_field = "access_backup";
		case ACCESS_RELAY:  access_field = "access_relay";
		default:            access_field = 0;
	}

	if (access_field && bc_db_get_val_int(dbres, access_field) != 1)
		goto end;

	if (device_id >= 0) {
		const char *reject_devices = bc_db_get_val(dbres, "access_device_list", NULL);
		char *p = (char*)reject_devices, *token;
		while ((token = strsep(&p, ","))) {
			if (atoi(token) == device_id)
				goto end;
		}
	}

	re = 1;
end:
	bc_db_free_table(dbres);
	return re;
}

int hex_encode(char *out, int out_sz, const char *in, int in_sz)
{
	int i;
	static const char table[] = "0123456789abcdef";

	if (out_sz < in_sz * 2 + 1) {
		*out = 0;
		return 0;
	}

	for (i = 0; i < in_sz; ++i) {
		*(out++) = table[(in[i] >> 4) & 0xf];
		*(out++) = table[in[i] & 0xf];
	}

	*out = 0;
	return in_sz*2;
}

