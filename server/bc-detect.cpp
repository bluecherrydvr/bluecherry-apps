/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <unistd.h>
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

struct card_list {
	int valid:1, dirty:1;

	int card_id;
	int n_ports;
	const char *uid_type;
	char name[256];
	char driver[64];
	char video_type[8];
};

static struct udev *udev_instance;

static int check_solo(struct udev_device *device, struct card_list *cards)
{
	char path[PATH_MAX];
	char card_name[32];
	char bcuid[37];
	const char *uid_type;
	char eeprom[128], driver[64], video_type[8];
	int id, ports;
	const char *syspath;
	DIR *dir;
	struct dirent *de;

	*card_name = 0;
	syspath = udev_device_get_syspath(device);
	dir = opendir(syspath);

	bc_log(Debug, "Checking driver on %s", syspath);

	if (!dir)
		return -errno;

	while ((de = readdir(dir))) {
		if (!strncmp(de->d_name, "solo6", 5)) {
			strlcpy(card_name, de->d_name, sizeof(card_name));
			break;
		}
	}
	closedir(dir);

	if (!*card_name) {
		bc_log(Debug, "%s: Not ready or incompatible driver loaded",
			syspath);
		return -EAGAIN;
	}

	sprintf(path, "%s/%s/eeprom", syspath, card_name);
	{
		int ret = 0;

		int fd = open(path, 0, O_RDONLY);
		if (fd >= 0) {
			ret = read(fd, eeprom, sizeof(eeprom));
			close(fd);
		}

		if (ret == sizeof(eeprom)
		    && strncmp(eeprom + BCUID_EEPROM_OFF, BCUID_EEPROM_PRE,
			       strlen(BCUID_EEPROM_PRE))) {
			memcpy(bcuid, eeprom + BCUID_EEPROM_OFF +
			       strlen(BCUID_EEPROM_PRE), 36);
			bcuid[36] = '\0';
			uid_type = BC_UID_TYPE_BC;
		} else {
			strcpy(bcuid, udev_device_get_property_value(device, "PCI_SLOT_NAME"));
			uid_type = BC_UID_TYPE_PCI;
		}
	}

	if (sscanf(card_name, "%999[^-]-%d-%d", driver, &id, &ports) != 3)
		return -EINVAL;

	sprintf(path, "%s/%s/video_type", syspath, card_name);
	{
		int ret = 0;

		int fd = open(path, 0, O_RDONLY);
		if (fd >= 0) {
			memset(video_type, 0, sizeof(video_type));
			ret = read(fd, video_type, sizeof(video_type));
			close(fd);
		}

		if (ret < 3)
			strcpy(video_type, "NTSC");
	}

	/* Check to see if we've scanned this one before */
	for (int i = 0; i < MAX_CARDS; i++) {
		/* Already scanned? */
		if (cards[i].valid && !strcasecmp(cards[i].name, bcuid)) {
			cards[i].dirty = 0;
			return 0;
		}
	}

	bc_log(Info, "solo6x10: Found %s[%s] id %d, %d ports", bcuid, driver,
		id, ports);

	for (int i = 0; i < MAX_CARDS; i++) {
		if (!cards[i].valid) {
			cards[i].card_id  = id;
			cards[i].n_ports  = ports;
			cards[i].uid_type = uid_type;
			strcpy(cards[i].driver, driver);
			strcpy(cards[i].name, bcuid);
			strcpy(cards[i].video_type, video_type);

			cards[i].valid = 1;
			cards[i].dirty = 0;
			break;
		}
	}

	return 0;
}

static int tw5864_add(struct udev_device *device, struct card_list *cards)
{
	const char *syspath = udev_device_get_syspath(device);
	const char *devpath = udev_device_get_devnode(device);

	bc_log(Debug, "Checking driver on devnode %s, syspath %s", devpath, syspath);

	for (int i = 0; i < MAX_CARDS; i++) {
		/* Check to see if we've scanned this one before */
		if (cards[i].valid) {
			/* Already scanned? */
			if (!strcmp(cards[i].name, syspath)) {
				cards[i].dirty = 0;
				break;
			}
		} else {
			cards[i].card_id  = 0;  /* Not used */
			cards[i].n_ports  = 4;  /* Let web interface merge entries with matching PCI addresses */
			cards[i].uid_type = "TW5864";
			strcpy(cards[i].driver, "tw5864");
			strcpy(cards[i].name, syspath);

			cards[i].valid = 1;
			cards[i].dirty = 0;
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
static int __bc_check_avail(struct card_list *cards)
{
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	int ret = 0;

	if (!udev_instance) {
		udev_instance = udev_new();
		if (!udev_instance) {
			bc_status_component_error("Cannot initialize udev");
			return -EIO;
		}
	}

	for (int i = 0; vendors[i].vendor; i++) {
		enumerate = udev_enumerate_new(udev_instance);
		udev_enumerate_add_match_sysattr(enumerate, "vendor", vendors[i].vendor);
		udev_enumerate_scan_devices(enumerate);
		devices = udev_enumerate_get_list_entry(enumerate);
		udev_list_entry_foreach(dev_list_entry, devices) {
			struct udev_device *dev;
			const char *path, *device_id;

			path = udev_list_entry_get_name(dev_list_entry);
			dev = udev_device_new_from_syspath(udev_instance, path);

			device_id = udev_device_get_sysattr_value(dev, "device");

			if (device_id) {
				bc_log(Debug, "Scanning device %s (%s)", device_id, path);
				for (const char **x = vendors[i].devices; *x; ++x) {
					if (!strcmp(device_id, *x)) {
						bc_log(Debug, "Found card from vendor %s, checking driver...", vendors[i]);
						/* If there is no driver, this device isn't initialized yet */
						ret = udev_device_get_driver(dev)
							? check_solo(dev, cards) : -EAGAIN;
						break;
					}
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

	{  /* Add TW5864-based cards */
		enumerate = udev_enumerate_new(udev_instance);
		udev_enumerate_add_match_sysattr(enumerate, "vendor", "0x1797" /* Techwell */);
		udev_enumerate_scan_devices(enumerate);
		devices = udev_enumerate_get_list_entry(enumerate);
		udev_list_entry_foreach(dev_list_entry, devices) {
			const char *path = udev_list_entry_get_name(dev_list_entry);
			struct udev_device *dev = udev_device_new_from_syspath(udev_instance, path);
			const char *device_id  = udev_device_get_sysattr_value(dev, "device");
			if (device_id) {
				bc_log(Debug, "Scanning device %s (%s)", device_id, path);
				if (!strcmp(device_id, "0x5864")) {
					bc_log(Debug, "Found TW5864 card");
					/* If there is no driver, this device isn't initialized yet */
					if (udev_device_get_driver(dev))
						ret = tw5864_add(dev, cards);
					else
						ret = -EAGAIN;
				}
			}
			udev_device_unref(dev);
			if (ret)
				break;
		}

		udev_enumerate_unref(enumerate);
	}
	/* TODO Add generic V4L2 devices, filter them by udev_enumerate_add_nomatch_sysattr(), or design this whole routine */

	return ret;
}

int bc_check_avail(void)
{
	static struct card_list cards[MAX_CARDS];

	for (int i = 0; i < MAX_CARDS; i++)
		cards[i].dirty = cards[i].valid;

	int ret = __bc_check_avail(cards);
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
		bc_log(Error, "Database error!");
		return -EIO;
	}

	/* Check for cards gone missing */
	for (int i = 0; i < MAX_CARDS; i++) {
		if (!(cards[i].valid && cards[i].dirty))
			continue;
		bc_log(Warning, "Card with name %s no longer found", cards[i].name);
		memset(&cards[i], 0, sizeof(cards[i]));
	}

	return ret;
}

