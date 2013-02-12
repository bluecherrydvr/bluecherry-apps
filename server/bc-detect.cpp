/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <libudev.h>
#include <bsd/string.h>

#include "libbluecherry.h"

#include "bc-server.h"

#define BCUID_EEPROM_OFF	8
#define BCUID_EEPROM_PRE	BC_UID_TYPE_BC ":"

#define MAX_CARDS		32
static struct card_list {
	int valid;
	int dirty;
	int card_id;
	int n_ports;
	const char *uid_type;
	char name[37];
	char driver[64];
	char video_type[8];
} cards[MAX_CARDS];

static struct udev *udev_instance;

static int check_solo(struct udev_device *device)
{
	char path[PATH_MAX];
	char card_name[32];
	char bcuid[37];
	const char *uid_type;
	char eeprom[128], driver[64], video_type[8];
	int id, ports;
	int fd;
	int i;
	const char *syspath;
	DIR *dir;
	struct dirent *de;

	*card_name = 0;
	syspath = udev_device_get_syspath(device);
	dir = opendir(syspath);
	if (!dir)
		return -errno;
	while ((de = readdir(dir))) {
		if (!strncmp(de->d_name, "solo6", 5)) {
			strlcpy(card_name, de->d_name, sizeof(card_name));
			break;
		}
	}
	closedir(dir);

	if (!*card_name)
		return -EAGAIN;

	sprintf(path, "%s/%s/eeprom", syspath, card_name);
	fd = open(path, 0, O_RDONLY);
	if (fd >= 0) {
		int ret = read(fd, eeprom, sizeof(eeprom));
		close(fd);

		if (ret != sizeof(eeprom) ||
		    strncmp(eeprom + BCUID_EEPROM_OFF, BCUID_EEPROM_PRE,
			    strlen(BCUID_EEPROM_PRE))) {
			strcpy(bcuid, udev_device_get_property_value(device, "PCI_SLOT_NAME"));
			uid_type = BC_UID_TYPE_PCI;
		} else {
			memcpy(bcuid, eeprom + BCUID_EEPROM_OFF +
			       strlen(BCUID_EEPROM_PRE), 36);
			bcuid[36] = '\0';
			uid_type = BC_UID_TYPE_BC;
		}
	} else {
		strcpy(bcuid, udev_device_get_property_value(device, "PCI_SLOT_NAME"));
		uid_type = BC_UID_TYPE_PCI;
	}

	if (sscanf(card_name, "%999[^-]-%d-%d", driver, &id, &ports) != 3)
		return -EINVAL;

	sprintf(path, "%s/%s/video_type", syspath, card_name);
	fd = open(path, 0, O_RDONLY);
	if (fd >= 0) {
		int ret = read(fd, video_type, sizeof(video_type));
		close(fd);

		if (ret >= 3 && strncmp(video_type, "PAL", 3) == 0)
			video_type[3] = '\0';
		else if (ret >= 4 && strncmp(video_type, "NTSC", 4) == 0)
			video_type[4] = '\0';
		else
			strcpy(video_type, "NTSC");
	} else
		strcpy(video_type, "NTSC");

	/* Check to see if we've scanned this one before */
	for (i = 0; i < MAX_CARDS; i++) {
		if (cards[i].valid && !strcasecmp(cards[i].name, bcuid)) {
			cards[i].dirty = 0;
			break;
		}
	}

	if (i == MAX_CARDS) {
		bc_log(Info, "solo6x10: Found %s[%s] id %d, %d ports", bcuid, driver,
		       id, ports);
		for (i = 0; i < MAX_CARDS; i++) {
			if (cards[i].valid)
				continue;
			cards[i].valid    = 1;
			cards[i].dirty    = 0;
			cards[i].card_id  = id;
			cards[i].n_ports  = ports;
			cards[i].uid_type = uid_type;
			strcpy(cards[i].driver, driver);
			strcpy(cards[i].name, bcuid);
			strcpy(cards[i].video_type, video_type);
			break;
		}
	}

	return 0;
}

/* To detect devices prior to driver initialization, we
 * have to search by PCI IDs rather than the driver module.
 * This is used to make sure that all solo cards are initialized
 * before we update the database sources and begin using them. */
struct solo_vendor {
	const char *vendor;
	const char ** const devices;
};

const struct solo_vendor vendors[] = {
	{ "0x9413", (const char * []) /* Softlogic */
		{
			"0x6010", 0
		}
	},
	{ "0x1bb3", (const char * []) /* Bluecherry */
		{
			"0x4304", "0x4309", "0x4310", /* Neugent */
			"0x4e04", "0x4e09", "0x4e10", /* 6010 */
			"0x5304", "0x5308", "0x5310", /* 6110 */
			0
		}
	},
	{ 0, 0 }
};

/* Enumerate matching PCI devices and, if they've been initialied
 * by the driver, process them to update the database. */
static int __bc_check_avail(void)
{
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	int i, ret = 0;

	if (!udev_instance) {
		udev_instance = udev_new();
		if (!udev_instance) {
			bc_status_component_error("Cannot initialize udev");
			return -EIO;
		}
	}

	for (i = 0; vendors[i].vendor; i++) {
		enumerate = udev_enumerate_new(udev_instance);
		udev_enumerate_add_match_sysattr(enumerate, "vendor", vendors[i].vendor);
		udev_enumerate_scan_devices(enumerate);
		devices = udev_enumerate_get_list_entry(enumerate);
		udev_list_entry_foreach(dev_list_entry, devices) {
			struct udev_device *dev;
			const char *path, *device_id;
			const char **x;

			path = udev_list_entry_get_name(dev_list_entry);
			dev = udev_device_new_from_syspath(udev_instance, path);

			device_id = udev_device_get_sysattr_value(dev, "device");
			for (x = vendors[i].devices; device_id && *x; ++x) {
				if (!strcmp(device_id, *x)) {
					/* If there is no driver, this device isn't initialized yet */
					if (!udev_device_get_driver(dev))
						ret = -EAGAIN;
					else
						ret = check_solo(dev);
					break;
				}
			}

			udev_device_unref(dev);
			if (ret)
				break;
		}

		udev_enumerate_unref(enumerate);
		if (ret)
			break;
	}

	return ret;
}

int bc_check_avail(void)
{
	int ret = 0;

	for (int i = 0; i < MAX_CARDS; i++)
		cards[i].dirty = cards[i].valid;

	ret = __bc_check_avail();
	if (ret < 0)
		return ret;

	if (bc_db_start_trans())
		return -EIO;

	if (__bc_db_query("DELETE FROM AvailableSources"))
		goto rollback;

	for (int i = 0; i < MAX_CARDS; i++) {
		struct card_list *card = &cards[i];
		if (!card->valid)
			break;

		for (int j = 0; j < card->n_ports; j++) {
			ret |= __bc_db_query("INSERT INTO AvailableSources "
			                     "(device, driver, card_id, video_type) "
			                     "VALUES('%s|%s|%d', '%s', '%d', '%s')",
			                     card->uid_type, card->name, j, card->driver,
			                     card->card_id, card->video_type);
			if (ret)
				goto rollback;
		}
	}

	if (bc_db_commit_trans()) {
	rollback:
		bc_db_rollback_trans();
		return -EIO;
	}

	/* Check for cards gone missing */
	for (int i = 0; i < MAX_CARDS; i++) {
		if (!(cards[i].valid && cards[i].dirty))
			continue;
		bc_log(Warning, "solo6x10: card with name %s no longer found", cards[i].name);
		memset(&cards[i], 0, sizeof(cards[i]));
	}

	return ret;
}

