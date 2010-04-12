/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>

#include "bc-server.h"

extern char *__progname;

static char *get_val(char **rows, int ncols, int row, const char *colname)
{
	int i;

	for (i = 0; i < ncols; i++) {
		if (strcmp(colname, rows[i]) == 0)
			break;
	}

	return (i == ncols) ? NULL : rows[((row + 1) * ncols) + i];
}

static void usage(void)
{
	fprintf(stderr, "Usage: %s\n", __progname);
	exit(1);
}

int main(int argc, char **argv)
{
	struct bc_rec *bc_recs = NULL;
	struct bc_db_handle *bc_db;
	int nrows, ncols;
	char **rows;
	int opt;
	int res;
	int i;

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
		case 'h': default: usage();
		}
	}

	bc_log("Started");

	bc_db = bc_db_open(BC_DB_SQLITE);
	if (bc_db == NULL) {
		bc_log("E: Could not open SQL database");
		exit(1);
	}

	res = bc_db_get_table(bc_db, &nrows, &ncols, &rows,
			      "SELECT * from Devices;");
	if (res != 0 || nrows == 0) {
		bc_log("E: No devices to record from");
		exit(1);
	}

	/* Now that we have something to do... */
	if (0) { //daemon(0, 0) == -1) {
		bc_log("E: Could not fork to background: %m");
		exit(1);
	}

	for (i = 0; i < nrows; i++) {
		char *dev = get_val(rows, ncols, i, "source_video");
		char *proto = get_val(rows, ncols, i, "protocol");
		char *id = get_val(rows, ncols, i, "id");
		char *name = get_val(rows, ncols, i, "device_name");
		struct bc_rec *bc_rec;

		if (!dev || !proto || !id || !name)
			continue;

		/* Only v4l2 for now */
		if (strcmp(proto, "V4L2") != 0)
			continue;

		bc_rec = malloc(sizeof(*bc_rec));
		if (bc_rec == NULL) {
			bc_log("E(%s): out of memory trying to start record",
			       id);
			continue;
		}

		memset(bc_rec, 0, sizeof(*bc_rec));
		bc_rec->next = bc_recs;
		bc_recs = bc_rec;

		sprintf(bc_rec->outfile, BC_FILE_REC_BASE "%s-%lu.mkv",
			id, time(NULL));
		bc_rec->id = strdup(id);
		bc_rec->dev = strdup(dev);
		bc_rec->name = strdup(name);
		bc_rec->width = atoi(get_val(rows, ncols, i, "resolutionX"));
		bc_rec->height = atoi(get_val(rows, ncols, i, "resolutionY"));

		if (bc_start_record(bc_rec)) {
			free(bc_rec);
			continue;
		}

		/* Add to the list */
		bc_rec->next = bc_recs;
		bc_recs = bc_rec;
	}

	sleep(30);

	exit(0);
}
