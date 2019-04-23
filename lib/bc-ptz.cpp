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
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <linux/serial.h>

#include "libbluecherry.h"

/* PTZ Control protocols */
enum {
	BC_PTZ_PROTO_PELCO = 0,
};

static void pelco_csum(uint8_t data[7])
{
	int i;

	data[6] = 0;
	for (i = 1; i < 6; ++i)
		data[6] += data[i];
}

static int itobaud(int input, speed_t *out)
{
        switch (input) {
	case 50: *out = B50; break;
	case 75: *out = B75; break;
	case 110: *out = B110; break;
	case 134: *out = B134; break;
	case 150: *out = B150; break;
	case 200: *out = B200; break;
	case 300: *out = B300; break;
	case 600: *out = B600; break;
	case 1200: *out = B1200; break;
	case 1800: *out = B1800; break;
	case 2400: *out = B2400; break;
	case 9600: *out = B9600; break;
	case 19200: *out = B19200; break;
	case 38400: *out = B38400; break;
	default: return -1;
	}

	return 0;
}

static int itobits(int input, int *out)
{
	switch (input) {
	case 8: *out = CS8; break;
	case 7: *out = CS7; break;
	case 6: *out = CS6; break;
	case 5: *out = CS5; break;
	default: return -1;
	}

	return 0;
}

static int ctopar(char input, int *out)
{
	switch (input) {
	case 'n': *out = IGNPAR; break;
	case 'e': *out = PARENB; break;
	case 'o': *out = PARENB | PARODD; break;
	default: return -1;
	}

	return 0;
}

void bc_ptz_check(struct bc_handle *bc, BC_DB_RES dbres)
{
	const char *ptz_path, *ptz_proto, *ptz_params;
	int baud, bits, stopbits, par, csbits, addr;
	speed_t speed;
	char parity;

	ptz_path = bc_db_get_val(dbres, "ptz_control_path", NULL);

	if (!ptz_path || strncasecmp(ptz_path, "/dev/", 5))
		return;

	ptz_proto = bc_db_get_val(dbres, "ptz_control_protocol", NULL);

	if (!ptz_proto || strncasecmp(ptz_proto, "PELCO", 5))
		return;

	ptz_params = bc_db_get_val(dbres, "ptz_serial_values", NULL);
	if (!ptz_params)
		return;

	if (sscanf(ptz_params, "%d,%d,%d,%c,%d", &addr, &baud, &bits, &parity,
		   &stopbits) != 5)
		return;
	parity = tolower(parity);

	if (itobaud(baud, &speed))
		return;

	if (itobits(bits, &csbits))
		return;

	if (ctopar(parity, &par))
		return;

	if (stopbits != 1 && stopbits != 2)
		return;

	if (addr < 0 || addr > 255)
		return;

	strcpy(bc->ptz_path, ptz_path);
	bc->ptz_addr = addr;
	bc->ptz_tio.c_cflag = csbits | CLOCAL | CREAD;
	if (stopbits == 2)
		bc->ptz_tio.c_cflag |= CSTOPB;
	cfsetospeed(&bc->ptz_tio, speed);
	cfsetispeed(&bc->ptz_tio, speed);
	bc->ptz_tio.c_iflag = par;
}

static int bc_ptz_cmd_pelco(struct bc_handle *bc, unsigned int cmd, int delay,
			    int pan_speed, int tilt_speed, int pset_id)
{
	struct serial_struct serinfo;
	uint8_t data[7] = { 0xff, 0x00, 0x00, 0x00, 0x20, 0x20, 0x00 };
	int fd, real_delay = -1;
	int ret = 0;

	/* STOP is only useful by itself */
	if (cmd & BC_PTZ_CMD_STOP && cmd != BC_PTZ_CMD_STOP)
		return -EINVAL;

	/* Cannot use move and preset commands together */
	if (cmd & BC_PTZ_CMDS_PSET_MASK && cmd & BC_PTZ_CMDS_MOVE_MASK)
		return -EINVAL;

	/* Directional commands cannot conflict  (e.g. up and down) */
	if ((cmd & 0x3) == 0x3 || (cmd & 0x30) == 0x30 ||
	    (cmd & 0x300) == 0x300 || (cmd & 0x3000) == 0x3000)
		return -EINVAL;

	/* Preset commands are exclusive */
	if (cmd & BC_PTZ_CMDS_PSET_MASK) {
		if ((cmd & BC_PTZ_CMDS_PSET_MASK) != BC_PTZ_CMD_SAVE &&
		    (cmd & BC_PTZ_CMDS_PSET_MASK) != BC_PTZ_CMD_GO &&
		    (cmd & BC_PTZ_CMDS_PSET_MASK) != BC_PTZ_CMD_CLEAR)
			return -EINVAL;
	}

	data[1] = bc->ptz_addr;

	if (pan_speed >= 0 && pan_speed <= 255)
		data[4] = pan_speed;
	if (tilt_speed >= 0 && tilt_speed <= 255)
		data[5] = tilt_speed;

	if (cmd & BC_PTZ_CMD_STOP) {
		data[3] = 0;
	} else if (cmd & BC_PTZ_CMDS_MOVE_MASK) {
	        if (cmd & BC_PTZ_CMD_RIGHT)
			data[3] |= 1 << 1, real_delay = delay;
		if (cmd & BC_PTZ_CMD_LEFT)
			data[3] |= 1 << 2, real_delay = delay;
		if (cmd & BC_PTZ_CMD_UP)
			data[3] |= 1 << 3, real_delay = delay;
		if (cmd & BC_PTZ_CMD_DOWN)
			data[3] |= 1 << 4, real_delay = delay;
		if (cmd & BC_PTZ_CMD_IN)
			data[3] |= 1 << 5, real_delay = delay;
		if (cmd & BC_PTZ_CMD_OUT)
			data[3] |= 1 << 6, real_delay = delay;
	} else { /* Presets */
		data[4] = 0;
		data[5] = pset_id;

		switch (cmd) {
		case BC_PTZ_CMD_SAVE: data[3] = 0x03; break;
		case BC_PTZ_CMD_GO: data[3] = 0x07; break;
		case BC_PTZ_CMD_CLEAR: data[3] = 0x05; break;
		default: return -EINVAL;
		}
	}

	pelco_csum(data);

	fd = open(bc->ptz_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
		return -EIO;

	/* Set close_wait to 0 */
	ioctl(fd, TIOCGSERIAL, &serinfo);
	serinfo.closing_wait = 65535; /* Magic for "none" */
	ioctl(fd, TIOCSSERIAL, &serinfo);

	tcflush(fd, TCIFLUSH);
	if (tcsetattr(fd, TCSANOW, &bc->ptz_tio) == 0 &&
	    write(fd, data, sizeof(data)) == sizeof(data)) {
		if (real_delay >= 0) {
			usleep(real_delay * 1000);
			data[2] = data[3] = data[6] = 0;
			pelco_csum(data);
			if (write(fd, data, sizeof(data)) != sizeof(data))
				ret = -EIO;
		}
	} else {
		ret = -EIO;		
	}

	close(fd);

	return ret;
}

int bc_ptz_cmd(struct bc_handle *bc, unsigned int cmd, int delay,
 	       int pan_speed, int tilt_speed, int pset_id)
{
	if (!strlen(bc->ptz_path))
		return -ENODEV;

	if (bc->ptz_proto == BC_PTZ_PROTO_PELCO)
		return bc_ptz_cmd_pelco(bc, cmd, delay, pan_speed,
					tilt_speed, pset_id);

	return -EINVAL;
}
