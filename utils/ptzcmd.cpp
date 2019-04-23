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

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>

#include <linux/serial.h>

#include "libbluecherry.h"
#include "utils.h"

extern char *__progname;

static void err(const char *fmt, ...)
{
	va_list va;

	fprintf(stderr, "ERROR: ");

	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);

	fprintf(stderr, "\n");

	exit(1);
}

static void usage(void)
{
	fprintf(stderr, "%s <id> <cmds> <delay> <panspeed> <tiltspeed> <psetid>\n",
		__progname);
	fprintf(stderr, "  (S)top\n");
	fprintf(stderr, "  Move: (u)p (d)own (l)eft (r)ight (w)ide (t)ight\n");
	fprintf(stderr, "  Presets: (s)ave (g)o (c)lear\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	struct bc_handle bch;
	long id;
	int delay, pan_speed, tilt_speed, pset_id;
	char *cmd_str;
	int ret;
	unsigned int cmd;
	BC_DB_RES dbres;

	if (argc != 7)
		usage();

	id = atol(argv[1]);
	cmd_str = argv[2];
	delay = atoi(argv[3]);
	pan_speed = atoi(argv[4]);
	tilt_speed = atoi(argv[5]);
	pset_id = atoi(argv[6]);

        if (id < 0)
		err("Invalid ID");

        if (bc_db_open(BC_CONFIG_DEFAULT))
		err("Could not open database");

	dbres = bc_db_get_table("SELECT * FROM Devices LEFT OUTER JOIN "
				"AvailableSources USING (device) WHERE "
				"Devices.id=%ld AND disabled=0", id);

	if (dbres == NULL || bc_db_fetch_row(dbres))
		err("Could not find device ID");

	memset(&bch, 0, sizeof(bch));
	bc_ptz_check(&bch, dbres);
	bc_db_free_table(dbres);

        for (unsigned int i = 0, cmd = 0; i < strlen(cmd_str); i++) {
		switch (cmd_str[i]) {
		case 'S': cmd |= BC_PTZ_CMD_STOP; break;
		case 'r': cmd |= BC_PTZ_CMD_RIGHT; break;
		case 'l': cmd |= BC_PTZ_CMD_LEFT; break;
		case 'u': cmd |= BC_PTZ_CMD_UP; break;
		case 'd': cmd |= BC_PTZ_CMD_DOWN; break;
		case 't': cmd |= BC_PTZ_CMD_IN; break;
		case 'w': cmd |= BC_PTZ_CMD_OUT; break;
		case 's': cmd |= BC_PTZ_CMD_SAVE; break;
		case 'g': cmd |= BC_PTZ_CMD_GO; break;
		case 'c': cmd |= BC_PTZ_CMD_CLEAR; break;
		default: err("Invalid command");
		}
	}

	ret = bc_ptz_cmd(&bch, cmd, delay, pan_speed, tilt_speed, pset_id);

	if (ret)
		err("Command failed");

        exit(0);
}
