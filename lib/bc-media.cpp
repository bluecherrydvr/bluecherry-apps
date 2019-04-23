/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <bsd/string.h>

#include "libbluecherry.h"

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

static void __update_stat(struct bc_media_entry *bcm)
{
	struct stat st;

	if (stat(bcm->filepath, &st))
		return;

	bcm->bytes = st.st_size;
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

bc_event_cam_t bc_event_cam_start(int id, time_t start_ts,
                                  bc_event_level_t level,
                                  bc_event_cam_type_t type,
                                  const char *media_file)
{
	struct bc_event_cam *bce = (struct bc_event_cam*) malloc(sizeof(*bce));
	char media_id_str[24];
	int re;

	memset(bce, 0, sizeof(*bce));

	bce->id         = id;
	bce->start_time = start_ts;
	bce->level      = level;
	bce->type       = type;

	if (bc_db_start_trans()) {
		free(bce);
		return NULL;
	}

	if (media_file) {
		strlcpy(bce->media.filepath, media_file, PATH_MAX);

		re = __bc_db_query("INSERT INTO Media (start,device_id,filepath) "
		                   "VALUES(%lu,%d,'%s')", bce->start_time, bce->id,
		                   bce->media.filepath);
		if (re)
			goto error;

		bce->media.table_id = bc_db_last_insert_rowid();

		/* XXX: Lost connection? */
		if (!bce->media.table_id)
			goto error;

		snprintf(media_id_str, sizeof(media_id_str), "%lu", bce->media.table_id);
	} else
		strcpy(media_id_str, "NULL");

	re = __bc_db_query("INSERT INTO EventsCam (time,level_id,"
	                   "device_id,type_id,length,media_id) VALUES(%lu,'%s',%d,"
	                   "'%s',-1,%s)", bce->start_time, level_to_str[bce->level],
	                   bce->id, cam_type_to_str[bce->type], media_id_str);
	if (re)
		goto error;

	bce->inserted = bc_db_last_insert_rowid();

	if (bc_db_commit_trans()) {
	error:
		bc_db_rollback_trans();
		free(bce);
		return NULL;
	}

	return bce;
}

void bc_event_cam_end(bc_event_cam_t *__bce)
{
	bc_event_cam_t bce = *__bce;
	int re;
	if (!bce)
		return;
	*__bce = NULL;

	/* If we have no end time, set it here */
	if (!bce->end_time)
		bce->end_time = time(NULL);

	if (bce->media.table_id)
		__update_stat(&bce->media);

	if (bc_db_start_trans()) {
		free(bce);
		return;
	}

	re = __bc_db_query("UPDATE EventsCam SET length=%ld WHERE id=%lu",
	                   ((long)bce->end_time - (long)bce->start_time), bce->inserted);
	if (re)
		goto error;

	if (bce->media.table_id) {
		re = __bc_db_query("UPDATE Media SET end=%lu, size=%lu WHERE id=%lu",
		                   bce->end_time, bce->media.bytes, bce->media.table_id);
		if (re)
			goto error;
	}

	if (bc_db_commit_trans()) {
error:
		bc_db_rollback_trans();
	}

	free(bce);
	return;
}

int bc_event_has_media(bc_event_cam_t event)
{
	return event && event->media.table_id;
}

int bc_event_media_length(bc_event_cam_t event)
{
	if (!event || !event->media.table_id)
		return 0;

	if (event->end_time)
		return event->end_time - event->start_time;
	else
		return time(NULL) - event->start_time;
}

