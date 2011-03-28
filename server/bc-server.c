/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <sys/statvfs.h>

#include <libavutil/log.h>

#include "bc-server.h"

static BC_DECLARE_LIST(bc_rec_list);

static int max_threads;
static int cur_threads;
static int record_id = -1;
static float min_avail = 5.00;
static float min_thresh = 10.00;

char global_sched[7 * 24];
char media_storage[256];

extern char *__progname;

static void __handle_motion_start(struct bc_handle *bc)
{
	struct bc_record *bc_rec = bc->__data;

	/* If we already have an event in progress, keep it going */
	if (bc_rec->event != BC_EVENT_CAM_NULL)
		return;

	bc_rec->event = bc_event_cam_start(bc_rec->id, BC_EVENT_L_WARN,
					   BC_EVENT_CAM_T_MOTION, bc_rec->media);

	bc_dev_info(bc_rec, "Motion event started");
}

static void __handle_motion_end(struct bc_handle *bc)
{
	struct bc_record *bc_rec = bc->__data;

	/* Ignore when no event is in progress */
	if (bc_rec->event == BC_EVENT_CAM_NULL)
		return;

	/* Do not stop event here, let that happen in bc-av.c */
	bc_dev_info(bc_rec, "Motion event stopped");
}

/* XXX Create a function here so that we don't have to do so many
 * SELECT's in bc_check_globals() */

/* Update our global settings */
static void bc_check_globals(void)
{
	BC_DB_RES dbres;

	/* Get global schedule, default to continuous */
	dbres = bc_db_get_table("SELECT * from GlobalSettings WHERE "
				"parameter='G_DEV_SCED'");

	if (dbres != NULL && !bc_db_fetch_row(dbres)) {
		const char *sched = bc_db_get_val(dbres, "value");
		if (sched && strlen(sched) == sizeof(global_sched))
			memcpy(global_sched, sched, sizeof(global_sched));
	} else {
		/* Default to continuous record */
		memset(global_sched, 'C', sizeof(global_sched));
	}
	bc_db_free_table(dbres);

	/* Get path to media storage location, or use default */
	dbres = bc_db_get_table("SELECT * from GlobalSettings WHERE "
				"parameter='G_DVR_MEDIA_STORE'");

	if (dbres != NULL && !bc_db_fetch_row(dbres)) {
		const char *stor = bc_db_get_val(dbres, "value");
		if (stor)
			strcpy(media_storage, stor);
	} else {
		strcpy(media_storage, "/var/lib/bluecherry/recordings");
	}
	bc_db_free_table(dbres);
}

static void bc_stop_threads(void)
{
	struct bc_record *bc_rec, *__t;
	char *errmsg = NULL;

	if (bc_list_empty(&bc_rec_list))
		return;

	bc_list_for_each_entry_safe(bc_rec, __t, &bc_rec_list, list)
		bc_rec->thread_should_die = "Shutting down";

	bc_list_for_each_entry_safe(bc_rec, __t, &bc_rec_list, list) {
		pthread_join(bc_rec->thread, (void **)&errmsg);
		bc_dev_info(bc_rec, "Camera thread stopped: %s", errmsg);
		bc_list_del(&bc_rec->list);
		bc_handle_free(bc_rec->bc);
		free(bc_rec);
		cur_threads--;
	}
}

/* Check for threads that have quit */
static void bc_check_threads(void)
{
	struct bc_record *bc_rec, *__t;
	char *errmsg = NULL;

	if (bc_list_empty(&bc_rec_list))
		return;

	bc_list_for_each_entry_safe(bc_rec, __t, &bc_rec_list, list) {
		if (pthread_tryjoin_np(bc_rec->thread, (void **)&errmsg))
			continue;

		bc_dev_info(bc_rec, "Camera thread stopped: %s", errmsg);
		bc_list_del(&bc_rec->list);
		bc_handle_free(bc_rec->bc);
		free(bc_rec);
		cur_threads--;
	}
}

static struct bc_record *bc_record_exists(const int id)
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

static float get_avail(void)
{
	struct statvfs st;

	if (statvfs(media_storage, &st))
		return -1.00;

	return (float)((float)st.f_bavail / (float)st.f_blocks) * 100;
}

/* Check if our media directory is getting full (min_avail%) and delete old
 * events until there's more than or equal to min_thresh% available. Do not
 * delete archived events. If we've deleted everything we can and we still
 * don't have more than min_avail% available, then complain....LOUDLY! */
static void bc_check_media(void)
{
	BC_DB_RES dbres;
	float avail;

	if ((avail = get_avail()) < 0)
		return;

	if (avail >= min_avail)
		return;

	bc_log("I: Filesystem for %s is %0.2f%% full, starting cleanup",
	       media_storage, 100.0 - avail);

	dbres = bc_db_get_table("SELECT * from Media WHERE archive=0 AND "
				"end!=0 ORDER BY start ASC");

	if (dbres == NULL)
		return;

	while (!bc_db_fetch_row(dbres) && avail < min_thresh) {
		const char *filepath = bc_db_get_val(dbres, "filepath");
		int id = bc_db_get_val_int(dbres, "id");

		if (filepath == NULL)
			continue;

		unlink(filepath);
		__bc_db_query("UPDATE Media SET filepath='',size=0 "
			    "WHERE id=%d", id);

		bc_log("W: Removed media %d, file %s to make space", id,
		       filepath);

		if ((avail = get_avail()) < 0)
			break;
        }

	bc_db_free_table(dbres);

	if (avail < 0)
		return;

	if (avail < min_avail) {
		bc_log("W: Filesystem is %0.2f%% full, but cannot delete "
		       "any more old media!", 100.0 - avail);
		bc_event_sys(BC_EVENT_L_ALRM, BC_EVENT_SYS_T_DISK);
	}
}

static void bc_check_db(void)
{
	struct bc_record *bc_rec;
	BC_DB_RES dbres;

	dbres = bc_db_get_table("SELECT * from Devices LEFT JOIN "
				"AvailableSources USING (device)");

	if (dbres == NULL)
		return;

	while (!bc_db_fetch_row(dbres)) {
		const char *proto = bc_db_get_val(dbres, "protocol");
		int id = bc_db_get_val_int(dbres, "id");

		if ((id < 0) || !proto)
			continue;

		bc_rec = bc_record_exists(id);
		if (bc_rec) {
			bc_update_record(bc_rec, dbres);
			continue;
		}

		/* Caller asked us to only use this record_id */
		if (record_id >= 0 && id != record_id)
			continue;

		/* Caller asked us to only start so many threads */
		if (max_threads && cur_threads >= max_threads)
			continue;

		/* If this is a V4L2 device, it needs to be detected */
		if (!strcasecmp(proto, "V4L2")) {
			int card_id = bc_db_get_val_int(dbres, "card_id");
			if (card_id < 0)
				continue;
		}

		bc_rec = bc_alloc_record(id, dbres);
		if (bc_rec == NULL)
			continue;

		cur_threads++;
		bc_list_add(&bc_rec->list, &bc_rec_list);
	}

	bc_db_free_table(dbres);
}

static void usage(void)
{
	fprintf(stderr, "Usage: %s [-s]\n", __progname);
	fprintf(stderr, "  -s\tDo not background\n");
	fprintf(stderr, "  -m\tMax threads to start\n");
	fprintf(stderr, "  -r\tRecord a specific ID only\n");

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
	time_t expire = 1304222400;

	if (t < expire)
		return;

	fprintf(stderr, "This beta expired %s", ctime(&expire));
	exit(1);
}

int main(int argc, char **argv)
{
	int opt;
	unsigned int loops;
	int bg = 1;
	int count;

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

        avcodec_init();
        av_register_all();
	/* Help pipe av* log to our log */
	av_log_set_callback(av_log_cb);

	if (bg && daemon(0, 0) == -1) {
		bc_log("E: Could not fork to background: %m");
		exit(1);
	}

	bc_log("I: Started Bluecherry daemon");

	for (count = 1; bc_db_open(); count++) {
		sleep(1);
		if (count % 30)
			continue;
		bc_log("E: Could not open SQL database after 30 seconds...");
	}

	bc_log("I: SQL database connection opened");

	/* Main loop */
	for (loops = 0 ;; loops++) {
		/* Every 2 minutes */
		if (!(loops % 120)) {
			/* Check for new devices */
			bc_check_avail();
			/* Check media locations for full */
			bc_check_media();
		}

		/* Every 10 seconds */
		if (!(loops % 10)) {
			/* Check global vars */
			bc_check_globals();
			/* Check for changes in cameras */
			bc_check_db();
		}

		/* Every second, check for dead threads */
		bc_check_threads();
		/* And resolve un-committed events/media */
		bc_media_event_clear();

		if (!bg && loops >= 240)
			break;

		sleep(1);
	}

	bc_stop_threads();
	bc_db_close();

	exit(0);
}
