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

#include <libsysfs.h>

#include <libbluecherry.h>

#include "bc-server.h"

#define BCUID_EEPROM_OFF	8
#define BCUID_EEPROM_PRE	BC_UID_TYPE_BC ":"

#define MAX_CARDS		32
static struct card_list {
	int valid;
	int dirty;
	char name[128];
} cards[MAX_CARDS];

static void check_solo(struct sysfs_device *device, const char *dir)
{
	char eeprom_path[SYSFS_PATH_MAX];
	char bcuid[37];
	char *uid_type;
	char eeprom[128], driver[64];
	int id, ports;
	int fd;
	int i;

	sprintf(eeprom_path, "%s/%s/eeprom", device->path, dir);
	fd = open(eeprom_path, 0, O_RDONLY);
	if (fd >= 0) {
		int ret = read(fd, eeprom, sizeof(eeprom));
		close(fd);

		if (ret != sizeof(eeprom) ||
		    strncmp(eeprom + BCUID_EEPROM_OFF, BCUID_EEPROM_PRE,
			    strlen(BCUID_EEPROM_PRE))) {
			strcpy(bcuid, device->bus_id);
			uid_type = BC_UID_TYPE_PCI;
		} else {
			memcpy(bcuid, eeprom + BCUID_EEPROM_OFF +
			       strlen(BCUID_EEPROM_PRE), 36);
			bcuid[36] = '\0';
			uid_type = BC_UID_TYPE_BC;
		}
	} else {
		strcpy(bcuid, device->bus_id);
		uid_type = BC_UID_TYPE_PCI;
	}

	if (sscanf(dir, "%999[^-]-%d-%d", driver, &id, &ports) != 3)
		return;

	/* Check to see if we've scanned this one before */
	for (i = 0; i < MAX_CARDS; i++) {
		if (cards[i].valid && !strcasecmp(cards[i].name, bcuid)) {
			cards[i].dirty = 0;
			break;
		}
	}

	if (i == MAX_CARDS) {
		bc_log("I: Found %s[%s] id %d, %d ports", bcuid, driver,
		       id, ports);
		for (i = 0; i < MAX_CARDS; i++) {
			if (cards[i].valid)
				continue;
			cards[i].valid = 1;
			cards[i].dirty = 0;
			strcpy(cards[i].name, bcuid);
			break;
		}
	}

	for (i = 0; i < ports; i++) {
		__bc_db_query("INSERT INTO AvailableSources "
			    "(device, driver, card_id) "
			    "VALUES('%s|%s|%d', '%s', '%d')",
			    uid_type, bcuid, i, driver, id);
	}
}

static struct sysfs_driver *try_driver(const char *driver)
{
	char path[SYSFS_PATH_MAX];

	memset(path, 0, sizeof(path));
	if ((sysfs_get_mnt_path(path, SYSFS_PATH_MAX)) != 0)
		return NULL;

	strcat(path, "/" SYSFS_BUS_NAME "/pci/" SYSFS_DRIVERS_NAME "/");
	strcat(path, driver);

	return sysfs_open_driver_path(path);
}

/* Peruse sysfs for PCI devices associated with solo6010 driver */
static void __bc_check_avail(void)
{
	struct sysfs_driver *driver;
	struct sysfs_device *device;
	struct dlist *devlist;

        driver = try_driver("solo6x10");
	if (driver == NULL)
		driver = try_driver("solo6010");
	if (driver == NULL)
		return;

	devlist = sysfs_get_driver_devices(driver);
	if (devlist == NULL) {
		sysfs_close_driver(driver);
		return;
	}

	dlist_for_each_data(devlist, device, struct sysfs_device) {
		struct dlist *dirlist = sysfs_open_directory_list(device->path);
		char *dir;

		if (dirlist == NULL)
			continue;

		dlist_for_each_data(dirlist, dir, char) {
			if (!strncmp(dir, "solo6", 5)) {
				check_solo(device, dir);
				break;
			}
		}

		sysfs_close_list(dirlist);
	}

	sysfs_close_driver(driver);
}

void bc_check_avail(void)
{
	int i;

	if (bc_db_start_trans())
		return;

	__bc_db_query("DELETE FROM AvailableSources");
	for (i = 0; i < MAX_CARDS; i++) {
		if (!cards[i].valid)
			continue;
		cards[i].dirty = 1;
	}

	__bc_check_avail();

	bc_db_commit_trans();

	/* Check for cards gone missing */
	for (i = 0; i < MAX_CARDS; i++) {
		if (!(cards[i].valid && cards[i].dirty))
			continue;
		bc_log("W: Card with name %s no longer found", cards[i].name);
		memset(&cards[i], 0, sizeof(cards[i]));
	}
}
