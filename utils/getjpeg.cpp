/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <errno.h>

#include "libbluecherry.h"

static void print_error(char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fprintf(stderr, "\n");

	exit(1);
}

static void print_image(struct bc_handle *bc)
{
	void *data;
	int size;

	if (bc_buf_get(bc))
		print_error("Failed to get a buffer");

	data = bc_buf_data(bc);
	size = bc_buf_size(bc);

	if (data == NULL || size <= 0)
		print_error("Invalid data or size for buffer");

	if (fwrite(bc_buf_data(bc), bc_buf_size(bc), 1, stdout) != 1)
		print_error("Error writing jpeg: %m");
}

extern char *__progname;

static void usage(void)
{
	fprintf(stderr, "Usage: %s <args> > outfile\n", __progname);
	fprintf(stderr, "  -d\tDevice id (default 1)\n");
	exit(1);
}

int main(int argc, char **argv)
{
	const char *err_msg;
	BC_DB_RES dbres;
	struct bc_handle *bc;
	long devid = 1;
	int opt;

	while ((opt = getopt(argc, argv, "d:h")) != -1) {
		switch (opt) {
		case 'd': devid = atol(optarg); break;
		case 'h': default: usage();
		}
	}

	if (devid < 0)
		usage();

	if (bc_db_open())
		print_error("Failed to open database");

	dbres = bc_db_get_table("SELECT * FROM Devices LEFT OUTER JOIN "
				"AvailableSources USING (device) WHERE "
				"Devices.id=%ld AND disabled=0", devid);

	if (dbres == NULL)
		print_error("Failed to find device in database");

	if (bc_db_fetch_row(dbres))
		print_error("Failed to get device from database");

	/* Setup the device */
	if ((bc = bc_handle_get(dbres)) == NULL)
		print_error("Error opening device: %m");

	if (bc->type != BC_DEVICE_V4L2)
		print_error("Not a v4l2 device");

	bc_db_free_table(dbres);
	bc_db_close();

	/* Setup for MJPEG, leave everything else as default */
	bc->v4l2.vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	if (ioctl(bc->v4l2.dev_fd, VIDIOC_S_FMT, &bc->v4l2.vfmt) < 0)
		print_error("Error setting mjpeg: %m");

	if (bc_handle_start(bc, &err_msg))
		print_error("Error starting stream: %s", err_msg);

	print_image(bc);

	bc_handle_free(bc);

	exit(0);
}
