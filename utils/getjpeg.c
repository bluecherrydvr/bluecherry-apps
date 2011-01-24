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

#include <libbluecherry.h>

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
	bc_buf_get(bc);

	if (fwrite(bc_buf_data(bc), bc_buf_size(bc), 1, stdout) != 1)
		print_error("Error writing jpeg: %m");
}

extern char *__progname;

static void usage(void)
{
        fprintf(stderr, "Usage: %s <args> > outfile\n", __progname);
	fprintf(stderr, "  -d\tDevice name\n");
	fprintf(stderr, "  -D\tDriver name\n");
	exit(1);
}

int main(int argc, char **argv)
{
	struct bc_handle *bc;
	char dev[256];
	char driver[256];
	int opt;

	dev[0] = driver[0] = '\0';
	dev[sizeof(dev) - 1] = '\0';
	driver[sizeof(driver) - 1] = '\0';

	while ((opt = getopt(argc, argv, "d:D:h")) != -1) {
		switch (opt) {
		case 'd': strncpy(dev, optarg, sizeof(dev) - 1); break;
		case 'D': strncpy(driver, optarg, sizeof(driver) - 1); break;
		case 'h': default: usage();
		}
	}

	if (dev[0] == '\0' || driver[0] == '\0')
		usage();

	/* Setup the device */
	if ((bc = bc_handle_get(NULL)) == NULL)
		print_error("%s: error opening device: %m", dev);

	/* Setup for MJPEG, leave everything else as default */
	bc->vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	if (ioctl(bc->dev_fd, VIDIOC_S_FMT, &bc->vfmt) < 0)
		print_error("%s: error setting mjpeg: %m", dev);

	if (bc_handle_start(bc))
		print_error("%s: error starting stream: %m", dev);

	print_image(bc);

	bc_handle_free(bc);

	exit(0);
}
