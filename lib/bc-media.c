/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#include <libbluecherry.h>

/* These must match the defs in the SQL db */
static const char *level_to_str[] = {
	"info", "warn", "alrm", "crit"
};

static const char *cam_type_to_str[] = {
	"motion", "not found", "video signal loss", "audio signal loss"
};

static const char *sys_type_to_str[] = {
	"disk-space", "crash", "boot", "shutdown", "reboot", "power-outage"
};

static const char *video_type_to_str[] = {
	"h264", "m4v", "m2v"
};

static const char *audio_type_to_str[] = {
	"none", "mp2", "mp3", "wav"
};

static const char *cont_type_to_str[] = {
	"none", "mkv", "mp4", "mpeg2_ps", "mpeg2_ts", "avi"
};

/* Linked list if events that failed to write */
static BC_DECLARE_LIST(cam_event_queue);
static BC_DECLARE_LIST(sys_event_queue);
static struct bc_db_handle *bcdb;
static pthread_mutex_t db_lock;

struct bc_event_cam {
	int id;
	const char *level;
	const char *type;
	time_t start_time;
	time_t end_time;
	unsigned long inserted;
	struct bc_media_entry *media;
	struct bc_list_struct list;
};

struct bc_event_sys {
	const char *level;
	const char *type;
	time_t time;
	struct bc_list_struct list;
};

struct bc_media_entry {
	int cam_id;
	unsigned long table_id;
	time_t start, end;
	const char *video, *audio, *cont;
	struct bc_list_struct list;
};

static struct bc_event_cam *__alloc_event_cam(int id, bc_event_level_t level,
					      bc_event_cam_type_t type,
					      bc_media_entry_t media)
{
	struct bc_event_cam *bce = malloc(sizeof(*bce));

	if (bce == NULL)
		return NULL;

	memset(bce, 0, sizeof(*bce));
	bce->id = id;
	bce->level = level_to_str[level];
	bce->type = cam_type_to_str[type];
	bce->start_time = time(NULL);
	bce->media = media;

	return bce;
}

static int bce_open_db(void)
{
	if (bcdb)
		return 0;

	if ((bcdb = bc_db_open()) == NULL)
		return -1;

	pthread_mutex_init(&db_lock, NULL);

	return 0;
}

static int __do_media(struct bc_media_entry *bcm)
{
	int res;

	if (bce_open_db())
		return -1;

	if (bcm->table_id) {
		/* This is the common case to end a call */
		res = bc_db_query(bcdb, "UPDATE Media (end) VALUES('%lu')"
				  " WHERE id=%lu", time(NULL), bcm->table_id);
	} else if (!bcm->start) {
		/* Insert open ended for later update */
		bcm->start = time(NULL);
		pthread_mutex_lock(&db_lock);
		res = bc_db_query(bcdb, "INSERT INTO Media (start,device_id,"
				  "container,video,audio) VALUES('%lu',%d,'%s',"
				  "'%s','%s')", bcm->start, bcm->cam_id,
				  bcm->cont, bcm->video, bcm->audio);
		if (!res)
			bcm->table_id = bc_db_last_insert_rowid(bcdb);
		pthread_mutex_unlock(&db_lock);
	} else {
		/* Insert fully. Usually means there was a failure */
		pthread_mutex_lock(&db_lock);
		res = bc_db_query(bcdb, "INSERT INTO Media (start,end,device_id,"
				  "container,video,audio) VALUES('%lu','%lu',"
				  "'%d','%s','%s','%s')", bcm->start, time(NULL),
				  bcm->cam_id, bcm->cont, bcm->video,
				  bcm->audio);
		if (!res)
			bcm->table_id = bc_db_last_insert_rowid(bcdb);
		pthread_mutex_unlock(&db_lock);
	}

	return res;
}

static int __do_cam(struct bc_event_cam *bce)
{
	int res;

	if (bce_open_db())
		return -1;

	/* If the media isn't inserted yet, don't worry about the event */
	if (bce->media && !bce->media->table_id)
		return -1;

	if (bce->inserted) {
		/* This is the common route to end a call */
		res = bc_db_query(bcdb, "UPDATE EventsCam (length) VALUES('%lu')"
				  " WHERE id=%lu", bce->end_time - bce->start_time,
				  bce->inserted);
	} else if (bce->end_time) {
		/* Insert with length, usually happens on failure, or singular */
		pthread_mutex_lock(&db_lock);
		res = bc_db_query(bcdb, "INSERT INTO EventsCam (time,level_id,"
				"device_id,type_id,length) VALUES('%lu','%s','%d',"
				"'%s','%lu')", bce->start_time, bce->level,
				bce->id, bce->type, bce->end_time - bce->start_time);
		if (!res)
			bce->inserted = bc_db_last_insert_rowid(bcdb);
		pthread_mutex_unlock(&db_lock);
	} else {
		/* Insert open ended (for later update), common start point */
		pthread_mutex_lock(&db_lock);
		res = bc_db_query(bcdb, "INSERT INTO EventsCam (time,level_id,"
				"device_id,type_id,length) VALUES('%lu','%s','%d',"
				"'%s',-1)", bce->start_time, bce->level,
				bce->id, bce->type);
		if (!res)
			bce->inserted = bc_db_last_insert_rowid(bcdb);
		pthread_mutex_unlock(&db_lock);
	}

	/* If we have a media reference, update with that info */
	if (bce->inserted && bce->media) {
		bc_db_query(bcdb, "UPDATE EventsCam (media_id) VALUES(%lu)"
			    " WHERE id=%lu", bce->media->table_id, bce->inserted);
	}

	return res;
}

static int __do_sys_insert(struct bc_event_sys *bce)
{
	if (bce_open_db())
		return -1;

	return bc_db_query(bcdb, "INSERT INTO EventsSystem (time,level_id,"
                           "type_id) VALUES('%lu','%s','%s')", bce->time,
			   bce->level, bce->type);
}

int bc_event_sys(bc_event_level_t level, bc_event_sys_type_t type)
{
	struct bc_event_sys bce, *bu;

	bce.time = time(NULL);
	bce.level = level_to_str[level];
	bce.type = sys_type_to_str[type];

	if (!__do_sys_insert(&bce))
		return 0;

	bu = malloc(sizeof(bce));
	if (bu == NULL)
		return -1;

	memcpy(bu, &bce, sizeof(bce));

	bc_list_add(&bu->list, &sys_event_queue);

	return 0;
}

bc_media_entry_t bc_media_start(int id, bc_media_video_type_t video,
				bc_media_audio_type_t audio,
				bc_media_cont_type_t cont)
{
	struct bc_media_entry *bcm = malloc(sizeof(*bcm));

	if (bcm == NULL)
		return BC_MEDIA_FAIL;

	memset(bcm, 0, sizeof(*bcm));

	bcm->cam_id = id;
        bcm->video = video_type_to_str[video];
        bcm->audio = audio_type_to_str[audio];
        bcm->cont = cont_type_to_str[cont];

	__do_media(bcm);

	return bcm;
}

int bc_media_end(bc_media_entry_t bcm)
{
	/* If we have no end time, set it here */
	if (!bcm->end)
		bcm->end = time(NULL);

	/* On failure, we add it to the event_queue to try later */
	if (__do_media(bcm))
		return -1;

	free(bcm);
	return 0;
}

bc_event_cam_t bc_event_cam_start(int id, bc_event_level_t level,
				  bc_event_cam_type_t type,
				  bc_media_entry_t media)
{
	struct bc_event_cam *bce = __alloc_event_cam(id, level, type, media);

	/* TODO: This failure may need to be directed somewhere */
	if (bce == NULL)
		return BC_EVENT_CAM_FAIL;

	__do_cam(bce);

	return bce;
}

void bc_event_cam_end(bc_event_cam_t bce)
{
	/* If we have no end time, set it here */
	if (!bce->end_time)
		bce->end_time = time(NULL);

	/* On failure, we add it to the event_queue to try later */
	if (__do_cam(bce))
		bc_list_add(&bce->list, &cam_event_queue);
	else
		free(bce);
}

int bc_event_cam(int id, bc_event_level_t level,
		 bc_event_cam_type_t type)
{
	struct bc_event_cam *bce = __alloc_event_cam(id, level, type, NULL);

	/* TODO: Failure probably needs to be sent somewhere */
	if (bce == NULL)
		return -1;

	bce->end_time = bce->start_time;

	bc_event_cam_end(bce);

	return 0;
}

/* Run through the queues if needed */
void bc_media_event_clear(void)
{
	struct bc_event_cam *bcec;
	struct bc_event_sys *bces;
	struct bc_list_struct *lh;

	bc_list_for_each(lh, &cam_event_queue) {
		bcec = bc_list_entry(lh, struct bc_event_cam, list);
		/* On error, do nothing */
		if (__do_cam(bcec))
			continue;

		/* Finally, get this event out of the queue */
		bc_list_del(lh);
		free(bcec);
	}

	bc_list_for_each(lh, &sys_event_queue) {
		bces = bc_list_entry(lh, struct bc_event_sys, list);
		/* On error, do nothing */
		if (__do_sys_insert(bces))
			continue;

		/* Get this one out too */
		bc_list_del(lh);
		free(bces);
	}
}
