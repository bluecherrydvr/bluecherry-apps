/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <stdarg.h>

#include <libavutil/log.h>

#include "bc-server.h"

static BC_DECLARE_LIST(bc_rec_list);

static int max_threads;
static int cur_threads;
static int record_id = -1;

char global_sched[7 * 24];
char media_storage[256];

extern char *__progname;

static void __handle_motion_start(struct bc_handle *bc)
{
	struct bc_record *bc_rec = bc->__data;

	/* If we already have an event in progress, keep it going */
	if (bc_rec->event != BC_EVENT_CAM_FAIL)
		return;

	bc_rec->event = bc_event_cam_start(bc_rec->id, BC_EVENT_L_WARN,
					   BC_EVENT_CAM_T_MOTION, bc_rec->media);

	bc_log("I(%d): Motion event started", bc_rec->id);
}

static void __handle_motion_end(struct bc_handle *bc)
{
	struct bc_record *bc_rec = bc->__data;

	/* Ignore when no event is in progress */
	if (bc_rec->event == BC_EVENT_CAM_FAIL)
		return;

	bc_event_cam_end(&bc_rec->event);
	bc_log("I(%d): Motion event stopped", bc_rec->id);
}

static void check_globals(void)
{
	int nrows, ncols;
	char **rows;
	int res;

	/* Get global schedul, default to continuous */
        res = bc_db_get_table(&nrows, &ncols, &rows,
			      "SELECT * from GlobalSettings WHERE "
			      "parameter='G_DEV_SCED';");

	if (res == 0 && nrows == 1) {
		char *sched = bc_db_get_val(rows, ncols, 0, "value");
		if (sched && strlen(sched) == sizeof(global_sched))
			memcpy(global_sched, sched, sizeof(global_sched));
	} else {
		/* Default to continuous record */
		memset(global_sched, 'C', sizeof(global_sched));
	}
	if (res == 0)
		bc_db_free_table(rows);

	/* Get path to media storage location, or use default */
	res = bc_db_get_table(&nrows, &ncols, &rows,
			      "SELECT * from GlobalSettings WHERE "
			      "parameter='G_DVR_MEDIA_STORE';");

	if (res == 0 && nrows == 1) {
		char *stor = bc_db_get_val(rows, ncols, 0, "value");
		if (stor)
			strcpy(media_storage, stor);
	} else {
		strcpy(media_storage, "/var/lib/bluecherry/recordings");
	}
	if (res == 0)
		bc_db_free_table(rows);
}

static void check_threads(void)
{
	struct bc_record *bc_rec, *__t;
	int ret;
	char *errmsg = NULL;

	if (bc_list_empty(&bc_rec_list))
		return;

	bc_list_for_each_entry_safe(bc_rec, __t, &bc_rec_list, list) {
		ret = pthread_tryjoin_np(bc_rec->thread, (void **)&errmsg);
		if (!ret) {
			bc_log("I(%d): Record thread stopped: %s: %s",
			       bc_rec->id, bc_rec->name, errmsg);
			bc_list_del(&bc_rec->list);
			bc_handle_free(bc_rec->bc);
			free(bc_rec->aud_dev);
			free(bc_rec->dev);
			free(bc_rec->name);
			free(bc_rec);
			cur_threads--;
		}
	}
}

static struct bc_record *record_exists(const int id)
{
	struct bc_record *bc_rec;
	struct bc_list_struct *lh;

	if (bc_list_empty(&bc_rec_list))
		return NULL;

	bc_list_for_each(lh, &bc_rec_list) {
		bc_rec = bc_list_entry(lh, struct bc_record, list);
		if (bc_rec->id == id)
			return bc_rec;
	}

	return NULL;
}

static void check_db(void)
{
	struct bc_record *bc_rec;
	int nrows, ncols;
	char **rows;
	int i;
	int res;

	check_globals();

	pthread_mutex_lock(&db_lock);

	res = bc_db_get_table(&nrows, &ncols, &rows,
			      "SELECT * from Devices;");

	if (res != 0 || nrows == 0) {
		pthread_mutex_unlock(&db_lock);
		return;
	}

	for (i = 0; i < nrows; i++) {
		char *proto = bc_db_get_val(rows, ncols, i, "protocol");
		int id = bc_db_get_val_int(rows, ncols, i, "id");

		if ((id < 0) || !proto)
			continue;

		bc_rec = record_exists(id);
		if (bc_rec) {
			bc_update_record(bc_rec, rows, ncols, i);
			continue;
		}

		/* Caller asked us to only use this record_id */
		if (record_id >= 0 && id != record_id)
			continue;

		/* Caller asked us to only start so many threads */
		if (max_threads && cur_threads >= max_threads)
			continue;

		/* Only v4l2 for now */
		if (strcmp(proto, "V4L2") != 0)
			continue;

		bc_rec = bc_alloc_record(id, rows, ncols, i);
		if (bc_rec == NULL)
			continue;

		cur_threads++;
		bc_list_add(&bc_rec->list, &bc_rec_list);
	}

	bc_db_free_table(rows);

	pthread_mutex_unlock(&db_lock);
}

static void usage(void)
{
	fprintf(stderr, "Usage: %s [-s]\n", __progname);
	fprintf(stderr, "  -s\tDo not background\n");
	fprintf(stderr, "  -m\tMax threads to start\n");
	fprintf(stderr, "  -r\tRecord a specifc ID only\n");
	exit(1);
}

static void av_log_cb(void *avcl, int level, const char *fmt, va_list ap)
{
	char msg[strlen(fmt) + 20];

	if (level > AV_LOG_ERROR)
		return;

	sprintf(msg, "[avlib]: %s", fmt);
	bc_vlog(msg, ap);
}

static void check_expire(void)
{
	time_t t = time(NULL);

	if (t < 1293858000)
		return;

	fprintf(stderr, "This beta expires Jan 1, 2011 00:00:00\n");
	exit(1);
}

int main(int argc, char **argv)
{
	int opt;
	int loops;
	int bg = 1;

	check_expire();

	while ((opt = getopt(argc, argv, "hsm:r:")) != -1) {
		switch (opt) {
		case 's': bg = 0; break;
		case 'm': max_threads = atoi(optarg); break;
		case 'r': record_id = atoi(optarg); break;
		case 'h': default: usage();
		}
	}

	/* Setup our motion event handlers */
	bc_handle_motion_start = __handle_motion_start;
	bc_handle_motion_end = __handle_motion_end;

	pthread_mutex_init(&av_lock, NULL);
        avcodec_init();
        av_register_all();
	/* Help pipe av* log to our log */
	av_log_set_callback(av_log_cb);

	if (bc_db_open()) {
		bc_log("E: Could not open SQL database");
		exit(1);
	}

	if (bg && daemon(0, 0) == -1) {
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

		bc_check_avail();
		check_db();
	}

	exit(0);
}
