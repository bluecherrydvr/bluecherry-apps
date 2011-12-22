/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>

#include <libbluecherry.h>

/* These must match the defs in the SQL db */
static const char *level_to_str[] = {
	"info", "warn", "alrm", "crit"
};

static const char *cam_type_to_str[] = {
	"motion", "not found", "video signal loss", "audio signal loss", "continuous"
};

static const char *sys_type_to_str[] = {
	"disk-space", "crash", "boot", "shutdown", "reboot", "power-outage"
};

static const char db_status_file[] = "/var/run/bluecherry/db-writable";

static int bc_db_check_success(void)
{
	char cmd[256];

	sprintf(cmd, "touch -m -t 197001010000.00 %s", db_status_file);

	return system(cmd);
}

static int bc_db_check_fail(void)
{
	char cmd[256];

	sprintf(cmd, "touch -m %s", db_status_file);
	
	return system(cmd);
}

static struct bc_event_cam *__alloc_event_cam(int id, bc_event_level_t level,
					      bc_event_cam_type_t type,
					      bc_media_entry_t media)
{
	struct bc_event_cam *bce = malloc(sizeof(*bce));

	if (bce == NULL)
		return NULL;

	memset(bce, 0, sizeof(*bce));
	bce->id = id;
	bce->level = level;
	bce->type = type;
	bce->start_time = time(NULL);
	bce->media = media;

	return bce;
}

static void __update_stat(struct bc_media_entry *bcm)
{
	struct stat st;

	if (stat(bcm->filepath, &st))
		return;

	bcm->bytes = st.st_size;
}

static int __do_media(struct bc_media_entry *bcm)
{
	int res;

	if (bcm->table_id) {
		__update_stat(bcm);
		/* This is the common case to end a call */
		res = __bc_db_query("UPDATE Media SET end='%lu',size=%lu "
				  "WHERE id=%lu", time(NULL),
				  bcm->bytes, bcm->table_id);
	} else if (!bcm->start) {
		/* Insert open ended for later update */
		bcm->start = time(NULL);
		res = __bc_db_query("INSERT INTO Media (start,device_id,"
				  "filepath) VALUES('%lu',%d,'%s')",
				  bcm->start, bcm->cam_id, bcm->filepath);
		if (!res)
			bcm->table_id = bc_db_last_insert_rowid();
	} else {
		/* Insert fully. Usually means there was a failure */
		__update_stat(bcm);
		res = __bc_db_query("INSERT INTO Media (start,end,"
		                    "device_id,filepath,size) "
		                    "VALUES('%lu','%lu','%d','%s',%lu)",
		                    bcm->start, time(NULL), bcm->cam_id,
		                    bcm->filepath, bcm->bytes);
		if (!res)
			bcm->table_id = bc_db_last_insert_rowid();
	}

	return res;
}

static int do_media(struct bc_media_entry *bcm)
{
	int ret;

	if (bc_db_start_trans())
		return -1;

	ret = __do_media(bcm);
	if (ret) {
		bc_db_check_fail();
		bc_db_rollback_trans();
	} else {
		bc_db_check_success();
		bc_db_commit_trans();
	}

	return ret;
}

static int __do_cam(struct bc_event_cam *bce)
{
	int res;

	/* If the media isn't inserted yet, don't worry about the event */
	if (bce->media && !bce->media->table_id)
		return -1;

	if (bce->inserted) {
		/* This is the common route to end a call */
		res = __bc_db_query("UPDATE EventsCam SET length='%lu'"
				  " WHERE id=%lu", bce->end_time - bce->start_time,
				  bce->inserted);
	} else if (bce->end_time) {
		/* Insert with length, usually happens on failure, or singular */
		res = __bc_db_query("INSERT INTO EventsCam (time,level_id,"
				"device_id,type_id,length) VALUES('%lu','%s','%d',"
				"'%s','%lu')", bce->start_time, level_to_str[bce->level],
				bce->id, cam_type_to_str[bce->type], bce->end_time - bce->start_time);
		if (!res)
			bce->inserted = bc_db_last_insert_rowid();
	} else {
		/* Insert open ended (for later update), common start point */
		res = __bc_db_query("INSERT INTO EventsCam (time,level_id,"
				"device_id,type_id,length) VALUES('%lu','%s','%d',"
				"'%s',-1)", bce->start_time, level_to_str[bce->level],
				bce->id, cam_type_to_str[bce->type]);
		if (!res)
			bce->inserted = bc_db_last_insert_rowid();
	}

	/* If we have a media reference, update with that info */
	if (bce->inserted && bce->media) {
		__bc_db_query("UPDATE EventsCam SET media_id=%lu"
			      " WHERE id=%lu", bce->media->table_id,
			      bce->inserted);
	}

	return res;
}

static int do_cam(struct bc_event_cam *bce)
{
	int ret;

	if (bc_db_start_trans())
		return -1;

	ret = __do_cam(bce);
	if (ret) {
		bc_db_check_fail();
		bc_db_rollback_trans();
	} else {
		bc_db_check_success();
		bc_db_commit_trans();
	}

	return ret;
}

static int __do_sys_insert(struct bc_event_sys *bce)
{
	return bc_db_query("INSERT INTO EventsSystem (time,level_id,"
                           "type_id) VALUES('%lu','%s','%s')", bce->time,
			   level_to_str[bce->level], sys_type_to_str[bce->type]);
}

int bc_event_sys(bc_event_level_t level, bc_event_sys_type_t type)
{
	struct bc_event_sys bce;

	bce.time = time(NULL);
	bce.level = level;
	bce.type = type;

	return __do_sys_insert(&bce);
}

bc_media_entry_t bc_media_start(int id, const char *filepath,
				bc_event_cam_t bce)
{
	struct bc_media_entry *bcm = malloc(sizeof(*bcm));

	if (bcm == NULL)
		return BC_MEDIA_NULL;

	memset(bcm, 0, sizeof(*bcm));

	bcm->cam_id = id;
	strcpy(bcm->filepath, filepath);

	do_media(bcm);

	/* If no event associated with this, just carry on */
	if (!bce || !bcm->table_id || !bce->inserted)
		return bcm;

	/* Update to match */
	bcm->start = bce->start_time;

	/* Update cam event to link to correct media id */
	bc_db_query("UPDATE EventsCam SET media_id=%lu WHERE id=%lu",
		    bcm->table_id, bce->inserted);

	/* Update media start time */
	bc_db_query("UPDATE Media SET start=%lu WHERE id=%lu",
		    bcm->start, bcm->table_id);

	return bcm;
}

time_t bc_media_length(bc_media_entry_t *__bcm)
{
	bc_media_entry_t bcm = *__bcm;

	if (!bcm)
		return 0;

	if (!bcm->end)
		return time(NULL) - bcm->start;

	return bcm->end - bcm->start;
}

int bc_media_end(bc_media_entry_t *__bcm)
{
	bc_media_entry_t bcm = *__bcm;

	if (!bcm)
		return -1;

	*__bcm = NULL;

	/* If we have no end time, set it here */
	if (!bcm->end)
		bcm->end = time(NULL);

	/* On failure, we add it to the event_queue to try later */
	if (do_media(bcm))
		return -1;

	free(bcm);

	return 0;
}

void bc_media_destroy(bc_media_entry_t *__bcm)
{
	bc_media_entry_t bcm = *__bcm;

	if (!bcm)
		return;

	*__bcm = NULL;

	bc_db_query("DELETE FROM Media WHERE id=%lu", bcm->table_id);
}

bc_event_cam_t bc_event_cam_start(int id, bc_event_level_t level,
				  bc_event_cam_type_t type,
				  bc_media_entry_t media)
{
	struct bc_event_cam *bce = __alloc_event_cam(id, level, type, media);

	/* TODO: This failure may need to be directed somewhere */
	if (bce == NULL)
		return BC_EVENT_CAM_NULL;

	do_cam(bce);

	return bce;
}

void bc_event_cam_end(bc_event_cam_t *__bce)
{
	bc_event_cam_t bce = *__bce;

	if (!bce)
		return;

	*__bce = NULL;

	/* If we have no end time, set it here */
	if (!bce->end_time)
		bce->end_time = time(NULL);

	do_cam(bce);
	free(bce);
}

int bc_event_cam_fire(int id, bc_event_level_t level,
		 bc_event_cam_type_t type)
{
	struct bc_event_cam *bce = __alloc_event_cam(id, level, type, NULL);

	/* TODO: Failure probably needs to be sent somewhere */
	if (bce == NULL)
		return -ENOMEM;

	bce->end_time = bce->start_time;

	bc_event_cam_end(&bce);

	return 0;
}
