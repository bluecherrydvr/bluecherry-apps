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
#include <pthread.h>

#include "bc-server.h"

#include <libbluecherry.h>

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

static void check_solo(const char *dev, const char **driver, const char **alsa,
		       int *card_id)
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
	} else if (!strncmp(SOLO6110_CARD_PREFIX_ENC, card,
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
		*card_id = i;
	}

	return;
}

static void bc_add_debug_video(void)
{
	if (!debug_video)
		return;
	bc_db_query("INSERT INTO AvailableSources "
		    "(id, devicepath, driver, alsasounddev, card_id) "
		    "VALUES(999999, '%s', 'solo6010', '', 999999);",
		    FAKE_VIDEO_DEV);
}

void bc_check_avail(void)
{
	DIR *dir;
	struct dirent *dent;
	static int detection_run;

	if (detection_run)
		return;

	detection_run = 1;

	bc_db_lock();

	/* Truncate the table to clear all entries */
	bc_db_query("DELETE FROM AvailableSources");

	bc_add_debug_video();

	dir = opendir("/dev");
	if (dir == NULL) {
		bc_log("Could not open /dev: %m");
		bc_db_unlock();
		return;
	}

	while ((dent = readdir(dir)) != NULL) {
		const char *dev = dent->d_name;
		const char *driver = NULL, *alsadev = NULL;
		int card_id = 0;

		if (strncmp(dev, "video", 5))
			continue;

		/* Find the driver */
		if (driver == NULL)
			check_solo(dev, &driver, &alsadev, &card_id);

		if (driver == NULL)
			continue;

		bc_db_query("INSERT INTO AvailableSources "
			    "(devicepath, driver, alsasounddev, card_id) "
			    "VALUES('/dev/%s', '%s', '%s', %d);", dev,
			    driver, alsadev ?: "", card_id);
	}

	closedir(dir);

	bc_db_unlock();
}
