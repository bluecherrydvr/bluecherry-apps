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
#include "list.h"

static LIST_HEAD(bc_rec_head);
static struct bc_db_handle *bc_db;

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

static int get_val_int(char **rows, int ncols, int row, const char *colname)
{
	char *val = get_val(rows, ncols, row, colname);

	return val ? atoi(val) : -1;
}

static void check_threads(void)
{
	struct bc_record *bc_rec;
	struct list_head *lh;
	int ret;
	char *errmsg = NULL;

	if (list_empty(&bc_rec_head))
		return;

	list_for_each(lh, &bc_rec_head) {
		bc_rec = list_entry(lh, struct bc_record, list);
		ret = pthread_tryjoin_np(bc_rec->thread, (void **)&errmsg);
		if (!ret) {
			bc_log("I(%d): Record thread stopped: %s: %s",
			       bc_rec->id, bc_rec->name, errmsg);
			list_del(&bc_rec->list);
			free(bc_rec->dev);
			free(bc_rec->name);
			free(bc_rec);
		}
	}
}

static int record_exists(const int id)
{
	struct bc_record *bc_rec;
	struct list_head *lh;

	if (list_empty(&bc_rec_head))
		return 0;

	list_for_each(lh, &bc_rec_head) {
		bc_rec = list_entry(lh, struct bc_record, list);
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
		char *dev = get_val(rows, ncols, i, "source_video");
		char *proto = get_val(rows, ncols, i, "protocol");
		int id = get_val_int(rows, ncols, i, "id");
		char *name = get_val(rows, ncols, i, "device_name");
		struct tm tm;
		time_t t;
		char tbuf[30];

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

		t = time(NULL);
		gmtime_r(&t, &tm);
		strftime(tbuf, sizeof(tbuf), "%F-%T", &tm);
		sprintf(bc_rec->outfile, BC_FILE_REC_BASE "%06d-%s.mkv",
			id, tbuf);
		bc_rec->id = id;
		bc_rec->dev = strdup(dev);
		bc_rec->name = strdup(name);
		bc_rec->width = atoi(get_val(rows, ncols, i, "resolutionX"));
		bc_rec->height = atoi(get_val(rows, ncols, i, "resolutionY"));

		if (bc_start_record(bc_rec)) {
			free(bc_rec);
			continue;
		}

		list_add(&bc_rec->list, &bc_rec_head);
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
