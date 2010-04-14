/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "bc-server.h"

static BC_DECLARE_LIST(bc_rec_list);
static struct bc_db_handle *bc_db;

extern char *__progname;

static void check_threads(void)
{
	struct bc_record *bc_rec;
	struct bc_list_struct *lh;
	int ret;
	char *errmsg = NULL;

	if (bc_list_empty(&bc_rec_list))
		return;

	bc_list_for_each(lh, &bc_rec_list) {
		bc_rec = bc_list_entry(lh, struct bc_record, list);
		ret = pthread_tryjoin_np(bc_rec->thread, (void **)&errmsg);
		if (!ret) {
			bc_log("I(%d): Record thread stopped: %s: %s",
			       bc_rec->id, bc_rec->name, errmsg);
			bc_list_del(&bc_rec->list);
			free(bc_rec->dev);
			free(bc_rec->name);
			free(bc_rec);
		}
	}
}

static int record_exists(const int id)
{
	struct bc_record *bc_rec;
	struct bc_list_struct *lh;

	if (bc_list_empty(&bc_rec_list))
		return 0;

	bc_list_for_each(lh, &bc_rec_list) {
		bc_rec = bc_list_entry(lh, struct bc_record, list);
		if (bc_rec->id == id)
			return 1;
	}

	return 0;
}

static void check_db(void)
{
	struct bc_record *bc_rec;
	int nrows, ncols;
	char **rows;
	int i;
	int res;

	res = bc_db_get_table(bc_db, &nrows, &ncols, &rows,
			      "SELECT * from Devices;");

	if (res != 0 || nrows == 0)
		return;

	for (i = 0; i < nrows; i++) {
		char *dev = bc_db_get_val(rows, ncols, i, "source_video");
		char *proto = bc_db_get_val(rows, ncols, i, "protocol");
		int id = bc_db_get_val_int(rows, ncols, i, "id");
		char *name = bc_db_get_val(rows, ncols, i, "device_name");

		if (record_exists(id))
			continue;

		if ((id < 0) || !dev || !proto || !name)
			continue;

		/* Only v4l2 for now */
		if (strcmp(proto, "V4L2") != 0)
			continue;

		bc_rec = malloc(sizeof(*bc_rec));
		if (bc_rec == NULL) {
			bc_log("E(%d): out of memory trying to start record",
			       id);
			continue;
		}

		memset(bc_rec, 0, sizeof(*bc_rec));

		bc_rec->id = id;
		bc_rec->dev = strdup(dev);
		bc_rec->name = strdup(name);

		if (bc_start_record(bc_rec, rows, ncols, i)) {
			free(bc_rec);
			continue;
		}

		bc_list_add(&bc_rec->list, &bc_rec_list);
	}

	bc_db_free_table(bc_db, rows);
}

static void usage(void)
{
	fprintf(stderr, "Usage: %s\n", __progname);
	exit(1);
}

int main(int argc, char **argv)
{
	int opt;
	int loops;

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

	if (daemon(0, 0) == -1) {
		bc_log("E: Could not fork to background: %m");
		exit(1);
	}

	bc_log("Started");

	/* Main loop */
	loops = 0;
	for (;;) {
		check_threads();

		/* Check the db every minute */
		if (loops-- > 0) {
			sleep(1);
			continue;
		}
		loops = 60;

		check_db();
	}

	exit(0);
}
