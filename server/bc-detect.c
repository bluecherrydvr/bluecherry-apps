/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <libbluecherry.h>

static struct bc_db_handle *bc_db;

/* Table for enabled devices */
static int enabled_nrows, enabled_ncols;
static char **enabled_rows;

/* Table for available devices */
static int avail_nrows, avail_ncols;
static char **avail_rows;

extern char *__progname;

static int device_exists(const char *dev)
{
        int i;

	for (i = 0; i < enabled_nrows; i++) {
		char *name = bc_db_get_val(enabled_rows, enabled_ncols, i,
					   "source_video");
		/* Skip '/dev/' part */
		if (name != NULL && !strcmp(name + 5, dev))
			return 1;
	}

	for (i = 0; i < avail_nrows; i++) {
                char *name = bc_db_get_val(avail_rows, avail_ncols, i,
					   "devicepath");
		/* Skip '/dev/' part */
		if (name != NULL && !strcmp(name + 5, dev))
			return 1;
	}

	return 0;
}

static const char *get_v4l2_card_name(const char *dev)
{
	char fulldev[128];
	static struct v4l2_capability vcap;
	int fd;
	int ret;

	sprintf(fulldev, "/dev/%s", dev);
	fd = open(fulldev, O_RDWR);
	if (fd == -1)
		return NULL;

	ret = ioctl(fd, VIDIOC_QUERYCAP, &vcap);

	close(fd);

	if (ret < 0)
		return NULL;

	return (const char *)vcap.card;
}

#define SOLO6010_CARD_DISP		"Softlogic 6010"
#define SOLO6010_CARD_PREFIX_ENC	SOLO6010_CARD_DISP " Enc "

#define SOLO6110_CARD_DISP		"Softlogic 6110"
#define SOLO6110_CARD_PREFIX_ENC	SOLO6110_CARD_DISP " Enc "

static void check_solo(const char *dev, const char **driver, const char **alsa)
{
	const char *card = get_v4l2_card_name(dev);
	int devnum = atoi(dev + 5); /* The X in videoX */
	static char alsadev[256];
	char *prefix;
	int i;

	if (card == NULL)
		return;

	*driver = *alsa = NULL;

	if (strlen(card) < strlen(SOLO6010_CARD_PREFIX_ENC))
		return;

	if (!strncmp(SOLO6010_CARD_PREFIX_ENC, card,
		    strlen(SOLO6010_CARD_PREFIX_ENC))) {
		*driver = "solo6010";
		prefix = SOLO6010_CARD_DISP;
	} else if (strncmp(SOLO6110_CARD_PREFIX_ENC, card,
			   strlen(SOLO6110_CARD_PREFIX_ENC))) {
                *driver = "solo6110";
		prefix = SOLO6110_CARD_DISP;
	}

	if (*driver == NULL)
		return;

	/* We know we have a card now, so find it's parent by counting
	 * backwards */
	for (i = devnum - 1; i >= 0 && *alsa == NULL; i--) {
		char parent[16];

		sprintf(parent, "video%d", i);
		card = get_v4l2_card_name(parent);

		if (strcmp(card, prefix))
			continue;

		sprintf(alsadev, "hw:CARD=Softlogic%d,DEV=0,SUBDEV=%d",
			i, (devnum - 1) - i);
		*alsa = alsadev;
	}

	bc_log("Got %s and %s", *driver, *alsa ?: "none");

	return;
}

static void usage(void)
{
	fprintf(stderr, "Usage: %s\n", __progname);
	exit(1);
}

int main(int argc, char **argv)
{
	int opt, res;
	DIR *dir;
	struct dirent *dent;

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
		case 'h': default: usage();
		}
	}

	bc_db = bc_db_open();
	if (bc_db == NULL) {
		bc_log("E: Could not open SQL database");
		exit(1);
	}

	/* Cache the db data we need */
	res = bc_db_get_table(bc_db, &enabled_nrows, &enabled_ncols,
			      &enabled_rows, "SELECT * from Devices;");
	res |= bc_db_get_table(bc_db, &avail_nrows, &avail_ncols,
			       &avail_rows, "SELECT * from AvailableSources;");

	if (res != 0) {
		bc_log("Error reading tables from database");
		exit(1);
	}

	bc_log("Starting detection of available sources...");

	dir = opendir("/dev");
	if (dir == NULL) {
		bc_log("Could not open /dev: %m");
		exit(1);
	}

	while ((dent = readdir(dir)) != NULL) {
		const char *dev = dent->d_name;
		const char *driver = NULL, *alsadev = NULL;

		if (strncmp(dev, "video", 5))
			continue;

		/* Already in the database? */
		if (device_exists(dev))
			continue;

		/* Find the driver */
		if (driver == NULL)
			check_solo(dev, &driver, &alsadev);

		if (driver == NULL)
			continue;

		bc_log("/dev/%s: Driver %s%s%s detected", dev, driver,
		       alsadev ? ", AlsaDevice " : "",
		       alsadev ?: "");

		res = bc_db_query(bc_db, "INSERT INTO AvailableSources "
				  "(devicepath, driver, alsasounddev) "
				  "VALUES('/dev/%s', '%s', '%s');", dev,
				  driver, alsadev ?: "");
		if (res)
			bc_log("/dev/%s: Error inserting into database", dev);
	}

	closedir(dir);

	bc_log("Detection of available sources completed");

	bc_db_free_table(bc_db, enabled_rows);
	bc_db_free_table(bc_db, avail_rows);
	bc_db_close(bc_db);

	exit(0);
}
