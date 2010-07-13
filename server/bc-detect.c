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

		if (strncmp(dev, "video", 5))
			continue;

		if (device_exists(dev))
			continue;

		bc_log("Inserting /dev/%s into AvailableSources", dev);

		res = bc_db_query(bc_db, "INSERT INTO AvailableSources "
				  "(devicepath, driver) "
				  "VALUES('/dev/%s', 'solo6010');", dev);
	}

	closedir(dir);

	bc_log("Detection of available sources completed");

	bc_db_free_table(bc_db, enabled_rows);
	bc_db_free_table(bc_db, avail_rows);
	bc_db_close(bc_db);

	exit(0);
}
