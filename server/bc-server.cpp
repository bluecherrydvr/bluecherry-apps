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
#include <stdarg.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <bsd/string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <signal.h>
#include <limits.h>
#include <dirent.h>

extern "C" {
#include <libavutil/log.h>
}

#include "bc-server.h"
#include "bc-cleaner.h"
#include "rtsp.h"
#include "bc-syslog.h"
#include "bc-stats.h"
#include "bc-api.h"
#include "version.h"
#include "status_server.h"
#include "trigger_server.h"
#include "vaapi.h"
#include "bc-cleanup.h"

#ifdef V3_LICENSING
#include "v3license_server.h"
#endif /* V3_LICENSING */

#include <unordered_map>
#include <vector>
#include <queue>

/* Global Mutexes */
pthread_mutex_t mutex_global_sched;
pthread_mutex_t mutex_streaming_setup;
pthread_mutex_t mutex_snapshot_delay_ms;
pthread_mutex_t mutex_max_record_time_sec;
pthread_mutex_t mutex_abandoned_media;

static std::vector<bc_record*> bc_rec_list;
static pthread_mutex_t bc_rec_list_lock = PTHREAD_MUTEX_INITIALIZER;

static int cur_threads;
static int record_id = -1;
static int hwcard_ready = 0;

char global_sched[7 * 24 + 1];
int snapshot_delay_ms;
int max_record_time_sec;

static pthread_rwlock_t media_lock = PTHREAD_RWLOCK_INITIALIZER;

struct bc_storage {
	char path[PATH_MAX];
	float min_thresh, max_thresh;
};

#define MAX_STOR_LOCS	10
static struct bc_storage media_stor[MAX_STOR_LOCS];

/* Timestamp of when the server was last known to be running prior to this
 * instance; may be zero if unknown or never */
time_t last_known_running;


struct media_record {
	int id;
	int media_id;
	int duration;
	std::string filepath;
};

static bool abandoned_media_update_in_progress = false;
static std::queue<struct media_record> abandoned_media_to_update;
static std::queue<struct media_record> abandoned_media_updated;
typedef std::vector<std::string> bc_string_array;

static rtsp_server *rtsp = NULL;
static hls_listener *hls = NULL;
static bc_api *api = NULL;
#ifdef V3_LICENSING
static v3license_server *v3license = NULL;
#endif /* V3_LICENSING */

static char *component_error[NUM_STATUS_COMPONENTS];
static char *component_error_tmp;
/* XXX XXX XXX thread-local */
static bc_status_component status_component_active = (bc_status_component)-1;

typedef struct {
	int timestamp = 0;
	int try_count = 0;
} bc_cleanup_ctx_t;

typedef struct {
	/* Cleanup file list */
	bc_string_array files;

	/* Counters */
	int archived = 0;
	int removed = 0;
	int errors = 0;
	int others = 0;

	/* Oldest media date */
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int min = 0;
	int sec = 0;
} bc_oldest_media_t;

typedef std::unordered_map<std::string, bc_cleanup_ctx_t> bc_media_files;
typedef std::unordered_map<std::string, bc_cleanup_ctx_t>::iterator bc_media_files_it;

static bc_media_files g_media_files;
#define BC_CLEANUP_RETRY_COUNT	5
#define BC_CLEANUP_RETRY_SEC	5

extern volatile sig_atomic_t shutdown_flag;

static std::unique_ptr<CleanupManager> g_cleanup_manager;

void bc_status_component_begin(bc_status_component c)
{
	if (status_component_active >= 0) {
		bc_status_component_error("BUG: Component status not properly "
		                          "ended when starting %d", (int)c);
		bc_status_component_end(status_component_active, 0);
	}

	status_component_active = c;
	free(component_error_tmp);
	component_error_tmp = NULL;
}

int bc_status_component_end(bc_status_component c, int ok)
{
	if (c != status_component_active) {
		bc_status_component_error("BUG: Component end unexpectedly called "
		                          "for %d", (int)c);
		return 0;
	}

	if (!ok && !component_error_tmp)
		component_error_tmp = strdup("Unknown error");

	free(component_error[c]);
	component_error[c]      = component_error_tmp;
	component_error_tmp     = NULL;
	status_component_active = (bc_status_component)-1;

	return component_error[c] == NULL;
}

void bc_status_component_error(const char *error, ...)
{
	va_list args;
	va_start(args, error);

	if (component_error_tmp) {
		int l = strlen(component_error_tmp);
		component_error_tmp = (char*) realloc(component_error_tmp, l + 1024);
		component_error_tmp[l++] = '\n';
		vsnprintf(component_error_tmp + l, 1024, error, args);
	} else {
		component_error_tmp = (char*) malloc(1024);
		vsnprintf(component_error_tmp, 1024, error, args);
	}

	va_end(args);
}

static void bc_initialize_mutexes()
{
	pthread_mutex_init(&mutex_global_sched,
			   (pthread_mutexattr_t *) NULL);
	pthread_mutex_init(&mutex_streaming_setup,
			   (pthread_mutexattr_t *) NULL);
	pthread_mutex_init(&mutex_snapshot_delay_ms,
			   (pthread_mutexattr_t *) NULL);
	pthread_mutex_init(&mutex_max_record_time_sec,
			   (pthread_mutexattr_t *) NULL);
	pthread_mutex_init(&mutex_abandoned_media,
			   (pthread_mutexattr_t *) NULL);
}

static const char *component_string(bc_status_component c)
{
	switch (c) {
		case STATUS_DB_POLLING1: return "database-1";
		case STATUS_MEDIA_CHECK: return "media";
		case STATUS_HWCARD_DETECT: return "hwcard";
		default: return "";
	}
}

static void bc_update_server_status()
{
	int ret;
	char *full_error = NULL;
	int full_error_sz = 0;
	time_t ts;
	BC_DB_RES dbres;

	for (int i = 0; i < NUM_STATUS_COMPONENTS; ++i) {
		const char *component_str = component_string((bc_status_component)i);
		if (!component_error[i])
			continue;

		if (full_error) {
			int nl = strlen(component_error[i]);
			full_error = (char*) realloc(full_error, full_error_sz + nl + 128);
			snprintf(full_error + strlen(full_error), nl + 128, "\n\n[%s] %s",
			         component_str, component_error[i]);
			full_error_sz += nl + 128;
		} else {
			full_error_sz = asprintf(&full_error, "[%s] %s", component_str, component_error[i]);
			if (full_error_sz < 0) full_error = NULL; // Failed to allocate
		}

		bc_log(Fatal, "[%s] %s", component_str, component_error[i]);
	}

	if (bc_db_start_trans())
		goto error;

	dbres = __bc_db_get_table("SELECT * FROM ServerStatus");
	if (!dbres)
		goto rollback;

	ts = time(NULL);

	const char *query;
	if (!bc_db_fetch_row(dbres)) {
		if (full_error)
			ts = bc_db_get_val_int(dbres, "timestamp");
		query = "UPDATE ServerStatus SET pid=%d, timestamp=%lu, "
			"message=%s%s%s";
	} else {
		query = "INSERT INTO ServerStatus (pid, timestamp, message) "
			"VALUES (%d,%lu,%s%s%s)";
	}

	{
		const char *delim = full_error ? "'" : "";

		ret = __bc_db_query(query, (int)getpid(), ts, delim,
				full_error ? full_error : "NULL", delim);
	}

	bc_db_free_table(dbres);

	if (!ret)
		ret = bc_db_commit_trans();

	if (ret) {
rollback:
		bc_db_rollback_trans();
error:
		bc_log(Fatal, "Unable to update server status in database");
	}
	free(full_error);
}


static int canonicalize_path(const char *given_path, char *buf, size_t bufsiz)
{
	/* Getting canonical path using `readlink -m `.
	 * readlink util resides in coreutils, actual logic is not exposed in
	 * libraries and is too complex to copy-paste it.
	 */
	int r;
	char cmd[PATH_MAX + 100];
	/* Drop stderr and trailing newline */
	snprintf(cmd, sizeof(cmd), "echo -n `readlink -m %s 2>/dev/null`", given_path);

	FILE *pipe;
	pipe = popen(cmd, "r");
	if (!pipe) {
		bc_log(Error, "Failed to exec readlink");
		return 1;
	}

	size_t read = 0;
	while (!feof(pipe) && read < bufsiz) {
		r = fread(buf + read, 1, bufsiz - read, pipe);
		read += r;
	}
	pclose(pipe);

	if (read >= bufsiz)
		return 1;  /* Shouldn't happen if bufsiz == PATH_MAX */

	buf[read] = '\0';  /* Terminate with newline */
	/* If result is empty, treat as failure */
	return read ? 0 : 1;
}

static int load_storage_paths(void)
{
	BC_DB_RES dbres;
	int ret = 0;

	dbres = bc_db_get_table("SELECT * from Storage ORDER BY priority ASC");

	if (!dbres) {
		bc_status_component_error("Database failure for storage paths");
		return -1;
	}

	pthread_rwlock_wrlock(&media_lock);

	memset(media_stor, 0, sizeof(media_stor));

	int i = 0;
	while (!bc_db_fetch_row(dbres) && i < MAX_STOR_LOCS) {
		const char *path = bc_db_get_val(dbres, "path", NULL);
		float max_thresh = bc_db_get_val_float(dbres, "max_thresh");
		float min_thresh = bc_db_get_val_float(dbres, "min_thresh");

		if (!path || !*path || max_thresh < 0 || min_thresh < 0)
			continue;

		if (min_thresh < max_thresh / 2)
			bc_log(Warning, "Suspiciously low min_thresh for %s", path);

		if (canonicalize_path(path, media_stor[i].path, sizeof(media_stor[i].path))) {
			bc_log(Warning, "Failed to canonicalize storage path %s", path);
			continue;
		}
		bc_log(Debug, "Loaded storage path: '%s' => '%s'", path, media_stor[i].path);

		media_stor[i].max_thresh = max_thresh;
		media_stor[i].min_thresh = min_thresh;

		if (bc_mkdir_recursive(media_stor[i].path)) {
			bc_status_component_error("Cannot create storage path %s",
			                          media_stor[i].path);
		} else
			i++;
	}

	/* Cleanup in case of the last location failing */
	if (i < MAX_STOR_LOCS)
		memset(&media_stor[i], 0, sizeof(media_stor[i]));

	if (i == 0) {
		/* Fall back to one single default location */
		if (canonicalize_path("/var/lib/bluecherry/recordings",
					media_stor[0].path, sizeof(media_stor[0].path))) {
			bc_log(Warning, "Failed to canonicalize storage path"
					" /var/lib/bluecherry/recordings");
			ret = -1;
			/* Due to main() logics, we still need to end up with valid state */
			strcpy(media_stor[0].path, "/var/lib/bluecherry/recordings");

		}
		if (bc_mkdir_recursive(media_stor[0].path)) {
			bc_status_component_error("Cannot create storage path %s",
			                          media_stor[i].path);
		}
		media_stor[0].max_thresh = 95.00;
		media_stor[0].min_thresh = 90.00;
	}

	pthread_rwlock_unlock(&media_lock);

	bc_db_free_table(dbres);
	return ret;
}

/* Update our global settings */
static int bc_check_globals(void)
{
	BC_DB_RES dbres;

	/* Get global schedule, default to continuous */
	dbres = bc_db_get_table("SELECT * from GlobalSettings WHERE "
				"parameter='G_DEV_SCED'");

	if (!dbres)
		bc_status_component_error("Database failure for global schedule");

	if (dbres && !bc_db_fetch_row(dbres)) {
		const char *sched = bc_db_get_val(dbres, "value", NULL);
		if (sched) {
			pthread_mutex_lock(&mutex_global_sched);
			strlcpy(global_sched, sched, sizeof(global_sched));
			pthread_mutex_unlock(&mutex_global_sched);
		}
	} else {
		/* Default to continuous record */
		memset(global_sched, 'C', sizeof(global_sched));
		global_sched[sizeof(global_sched)-1] = 0;
	}
	bc_db_free_table(dbres);

	/* Get snapshotting delay */
	dbres = bc_db_get_table("SELECT * from GlobalSettings WHERE "
			"parameter='G_SNAPSHOT_DELAY'");

	if (!dbres)
		bc_status_component_error("Database failure for snapshot delay");

	pthread_mutex_lock(&mutex_snapshot_delay_ms);
	if (dbres && !bc_db_fetch_row(dbres)) {
		snapshot_delay_ms = bc_db_get_val_int(dbres, "value");
	} else {
		/* Set default */
		snapshot_delay_ms = SNAPSHOT_DELAY_MS_DEFAULT;
	}
	pthread_mutex_unlock(&mutex_snapshot_delay_ms);
	bc_db_free_table(dbres);

	/* Get max recording time */
	dbres = bc_db_get_table("SELECT * from GlobalSettings WHERE "
			"parameter='G_MAX_RECORD_TIME'");

	if (!dbres)
		bc_status_component_error("Database failure for max recording time");

	pthread_mutex_lock(&mutex_max_record_time_sec);
	if (dbres && !bc_db_fetch_row(dbres)) {
		max_record_time_sec = bc_db_get_val_int(dbres, "value");
	} else {
		/* Set default */
		max_record_time_sec = MAX_RECORD_TIME_SEC_DEFAULT;
	}
	pthread_mutex_unlock(&mutex_max_record_time_sec);
	bc_db_free_table(dbres);

	/* Get path to media storage locations, or use default */
	return load_storage_paths();
}

static void bc_stop_threads(void)
{
	pthread_mutex_lock(&bc_rec_list_lock);
	for (auto it = bc_rec_list.begin(); it != bc_rec_list.end(); it++)
		(*it)->thread_should_die = "Shutting down";

	char *errmsg = 0;
	for (auto it = bc_rec_list.begin(); it != bc_rec_list.end(); it++) {
		bc_record *bc_rec = *it;
		pthread_join(bc_rec->thread, (void **)&errmsg);

		char message[128];
		snprintf(message, sizeof(message), "Device stopped: %s", errmsg);

		bc_rec->log.log(Info, "%s", message);
		bc_rec->notify_device_state(message);

		delete bc_rec;
		cur_threads--;
	}

	bc_rec_list.clear();
	pthread_mutex_unlock(&bc_rec_list_lock);
}

/* Check for threads that have quit */
static void bc_check_threads(void)
{
	char *errmsg = 0;

	pthread_mutex_lock(&bc_rec_list_lock);
	for (unsigned int i = 0; i < bc_rec_list.size(); i++) {
		bc_record *bc_rec = bc_rec_list[i];
		if (pthread_tryjoin_np(bc_rec->thread, (void **)&errmsg))
			continue;

		char message[128];
		snprintf(message, sizeof(message), "Device stopped: %s", errmsg);

		bc_rec->log.log(Info, "%s", message);
		bc_rec->notify_device_state(message);

		delete bc_rec;
		cur_threads--;

		bc_rec_list.erase(bc_rec_list.begin()+i);
		i--;
	}
	pthread_mutex_unlock(&bc_rec_list_lock);
}

static struct bc_record *bc_record_exists(const int id) __attribute__((pure));

static struct bc_record *bc_record_exists(const int id)
{
	struct bc_record *ret = NULL;
	pthread_mutex_lock(&bc_rec_list_lock);
	for (auto it = bc_rec_list.begin(); it != bc_rec_list.end(); it++) {
		if ((*it)->id == id) {
			ret = *it;
			break;
		}
	}
	pthread_mutex_unlock(&bc_rec_list_lock);

	return ret;
}

/* TODO: use fixed point insead of float */
static float path_used_percent(const char *path)
{
	struct statvfs st;

	if (statvfs(path, &st))
		return -1.00;

	bc_log(Debug, "path_used_percent(%s) = %f, f_bavail = %li, f_blocks = %li", path, 100.0 - 100.0 * (float)st.f_bavail / (float)st.f_blocks, st.f_bavail, st.f_blocks);

	return 100.0 - 100.0 * (float)st.f_bavail / (float)st.f_blocks;
}

static uint64_t path_freespace(const char *path)
{
	struct statvfs st;

	if (statvfs(path, &st))
		return 0;

	bc_log(Debug, "path_freespace(%s) = %ld, f_bavail = %li, f_bsize = %li", path, (uint64_t)st.f_bavail * (uint64_t)st.f_bsize, st.f_bavail, st.f_bsize);

	return (uint64_t)st.f_bavail * (uint64_t)st.f_bsize;
}

/* Returns -1 on error, 0 for non-full, 1 for full */
static int is_storage_full(const struct bc_storage *stor)
{
	float used = path_used_percent(stor->path);

	if (used < 0)
		return -1;

	return (used >= stor->max_thresh);
}

/**
 * Return the best storage path. I.e. the least used, prioritizing those below
 * their threshold.
 */
const char *select_best_path(void)
{
	struct bc_storage *ret = NULL, *overused = NULL, *p;

	uint64_t cur, best = 0, best_overused = 0;
	for (p = media_stor; p < media_stor+MAX_STOR_LOCS && p->max_thresh; p++) {

		cur = path_freespace(p->path);
		if (cur < best)
			continue;

		if (is_storage_full(p) != 1) {
			best = cur;
			ret = p;
		} else if (cur > best_overused) {
			best_overused = cur;
			overused = p;
		}
	}

	if (!ret)
		ret = overused;

	return ret ? ret->path : NULL;
}

size_t bc_get_media_locations(bc_string_array &locations)
{
	struct bc_storage *p;

	for (p = media_stor; p < media_stor+MAX_STOR_LOCS && p->max_thresh; p++)
		locations.push_back(std::string(p->path));

	return locations.size();
}

int bc_get_media_loc(char *dest, size_t size)
{
	int ret = 0;

	pthread_rwlock_rdlock(&media_lock);

	const char *sel = select_best_path();
	if (!sel)
		ret = -1;
	else
		strlcpy(dest, sel, size);

	pthread_rwlock_unlock(&media_lock);
	return ret;
}

static bool is_media_max_age_exceeded()
{
	BC_DB_RES dbres;
	long start;
	int max_age;
	long now;
	bool exceeds_max_age;

	/* There's no binary search key at timestamp fields, I see no possibility
	 * for smart single no-overhead SQL query to determine the requested fact
	 */

	// Get max allowed recording age
	dbres = bc_db_get_table("SELECT value FROM GlobalSettings WHERE parameter = 'G_MAX_RECORD_AGE' LIMIT 1");
	if (!dbres)
		return false;

	// No setting, unlimited age allowed
	if (bc_db_fetch_row(dbres)) {
		bc_db_free_table(dbres);
		return false;
	}

	max_age = bc_db_get_val_int(dbres, "value");
	bc_db_free_table(dbres);

	if (max_age < 0) {
		bc_log(Debug, "is_media_max_age_exceeded(): max_age < 0, %d, illegal value, aborting further checks", max_age);
		return false;
	}

	if (max_age == 0) {
		bc_log(Debug, "is_media_max_age_exceeded(): max_age == 0, assuming no age limit");
		return false;
	}

	// Get oldest recording date
	dbres = bc_db_get_table("SELECT start FROM Media ORDER BY id ASC LIMIT 1");
	if (!dbres)
		return false;

	if (bc_db_fetch_row(dbres)) {
		bc_db_free_table(dbres);
		return false;
	}

	start = bc_db_get_val_int(dbres, "start");
	bc_db_free_table(dbres);

	if (start <= 0) {
		bc_log(Debug, "is_media_max_age_exceeded(): start <= 0, %ld, illegal value, aborting further checks", start);
		return false;
	}

	now = time(NULL);

	exceeds_max_age = start < now - max_age;
	bc_log(Debug, "Oldest recording is dated with timestamp %ld, now %ld, max_age %d. Too old? %s", start, now, max_age, exceeds_max_age ? "YES" : "NO");

	return exceeds_max_age;
}

/*
	Sidecars maybe created with delay and they will no exist during cleanup 
	in main loop. We need to remember sidecar path and try again few times.
*/
static void bc_cleanup_media_retry()
{
	bc_media_files_it it = g_media_files.begin();
	int removed = 0, error_count = 0;

	while (it != g_media_files.end())
	{
		const std::string &filepath = it->first;
		bc_cleanup_ctx_t &file = it->second;
		int now_time = time(NULL);
		bool deleted = false;

		if (now_time - file.timestamp >= BC_CLEANUP_RETRY_SEC) {
			if (unlink(filepath.c_str()) < 0 && errno != ENOENT) {
				bc_log(Warning, "Cannot remove file %s: %s", filepath.c_str(), strerror(errno));
				error_count++;
			} else if (errno == ENOENT) {
				file.timestamp = now_time;
				file.try_count++;
			} else {
				bc_remove_dir_if_empty(filepath);
				deleted = true;
				removed++;
			}
		}

		if (deleted || file.try_count >= BC_CLEANUP_RETRY_COUNT) {
			it = g_media_files.erase(it);
			continue;
		}

		++it;
	}

	if (removed || error_count) {
		bc_log(Info, "Cleaned up %d files on retry, errors(%d), remaining(%zu)", 
			removed, error_count, g_media_files.size());
	}
}

static int bc_media_is_archived(const char *filepath)
{
	int archived = 0;
	BC_DB_RES dbres = bc_db_get_table("SELECT * FROM Media WHERE archive=1 AND "
		"filepath='%s'", filepath);

	if (!dbres) {
		bc_log(Warning, "Failed to get media entry from database");
		return -1;
	}

	if (!bc_db_fetch_row(dbres)) archived = 1;
	bc_db_free_table(dbres);

	return archived;
}

static int bc_cleanup_motion_debug_folder(bc_oldest_media_t &ctx, const std::string& full_path, const std::string& time_path)
{
	bc_time rec_time;
	int id = 0;

	int count = sscanf(time_path.c_str(), "%04d/%02d/%02d/%06d/%02d-%02d-%02d",
		(int*)&rec_time.year, (int*)&rec_time.month, (int*)&rec_time.day, (int*)&id,
		(int*)&rec_time.hour, (int*)&rec_time.min, (int*)&rec_time.sec);

	if (count != 7) {
		bc_log(Error, "Can not parse date from path: %s", time_path.c_str());
		ctx.errors++;
		return -1;
	}

	bc_time old_time;
	old_time.year = ctx.year;
	old_time.month = ctx.month;
	old_time.day = ctx.day;
	old_time.hour = ctx.hour;
	old_time.min = ctx.min;
	old_time.sec = ctx.sec;

	/* Check if found entry is older than oldest media record in DB */
	bool entry_is_old = bc_compare_time(rec_time, old_time);

	if (entry_is_old) {
		bc_remove_directory(full_path);
		return 1;
	}

	return 0;
}

/*
	Recursively read media directory and delete every untracked file.
	This may take a while but will only happen at startup once.
*/
static int bc_recursive_cleanup_untracked_media(bc_oldest_media_t &ctx, std::string dir_path)
{
	if (dir_path.back() != '/') dir_path.append("/");
	DIR *pdir = opendir(dir_path.c_str());

	if (pdir == NULL) {
		bc_log(Warning, "Can not open directory %s: %s",
			dir_path.c_str(), strerror(errno));

		ctx.errors++;
		return -1;
	}

	struct dirent *entry = readdir(pdir);
	while (entry != NULL)
	{
		/* Found an entry, but ignore . and .. */
		if (strcmp(".", entry->d_name) == 0 ||
			strcmp("..", entry->d_name) == 0) {
			entry = readdir(pdir);
			continue;
		}

		struct stat statbuf;
		std::string entry_name = entry->d_name;
		std::string full_path = dir_path + entry_name;

		if (lstat(full_path.c_str(), &statbuf) < 0) {
			bc_log(Error, "Can not stat file: %s", full_path.c_str());
			entry = readdir(pdir);
			ctx.errors++;
			continue;
		}

		if (S_ISDIR(statbuf.st_mode)) { // Recursive read directory

			std::size_t debug_pos = full_path.find(".debug");
			if (debug_pos != std::string::npos)
			{
				std::string sub_path = full_path.substr(0, debug_pos);
				if (sub_path.length() > 26)
				{
					std::string time_path = sub_path.substr(sub_path.size() - 26, std::string::npos);
					bc_cleanup_motion_debug_folder(ctx, full_path, time_path);
				}

				entry = readdir(pdir);
				continue;
			}

			bc_recursive_cleanup_untracked_media(ctx, full_path);
			bc_remove_dir_if_empty(full_path);

			entry = readdir(pdir);
			continue;
		}

		if (entry_name.compare(entry_name.size()-4, 4, ".mkv") &&
			entry_name.compare(entry_name.size()-4, 4, ".mp4") &&
			entry_name.compare(entry_name.size()-4, 4, ".jpg")) {
			/* Not a bc media file, leave it unchanged */
			entry = readdir(pdir);
			ctx.others++;
			continue;
		}

		size_t time_len = 26;
		size_t suffix_len = 4;

		if (!entry_name.compare(entry_name.size()-11, 11, "-motion.jpg") ||
			!entry_name.compare(entry_name.size()-11, 11, "-motion.mp4") ||
			!entry_name.compare(entry_name.size()-11, 11, "-motion.mkv"))
		{
			/* Not a bc media file, leave it unchanged */
			suffix_len = 11;
		}

		size_t ending_len = time_len + suffix_len;
		if (full_path.length() < ending_len)
		{
			entry = readdir(pdir);
			continue;
		}

		std::string timed_path = full_path.substr(full_path.size() - ending_len, time_len);
		bc_time rec_time;
		int id = 0;

		int count = sscanf(timed_path.c_str(), "%04d/%02d/%02d/%06d/%02d-%02d-%02d",
			(int*)&rec_time.year, (int*)&rec_time.month, (int*)&rec_time.day, (int*)&id,
			(int*)&rec_time.hour, (int*)&rec_time.min, (int*)&rec_time.sec);

		if (count != 7) {
			bc_log(Error, "Can not parse date from media file: %s (%s)",
				full_path.c_str(), timed_path.c_str());

			entry = readdir(pdir);
			ctx.errors++;
			continue;
		}

		bc_time old_time;
		old_time.year = ctx.year;
		old_time.month = ctx.month;
		old_time.day = ctx.day;
		old_time.hour = ctx.hour;
		old_time.min = ctx.min;
		old_time.sec = ctx.sec;

		/* Check if found entry is older than oldest media record in DB */
		bool entry_is_old = bc_compare_time(rec_time, old_time);

		if (entry_is_old) {
			std::string video_file = full_path;
			video_file.replace(video_file.size()-3, 3, "mkv");

			int rv = bc_media_is_archived(video_file.c_str());
			if (rv > 0) {
				entry = readdir(pdir);
				ctx.archived++;
				continue;
			} else if (rv < 0) {
				entry = readdir(pdir);
				ctx.errors++;
				continue;
			}

			if (unlink(full_path.c_str()) < 0) {
				bc_log(Warning, "Cannot remove old file %s: %s",
			       full_path.c_str(), strerror(errno));

				entry = readdir(pdir);
				ctx.errors++;
				continue;
			}

			ctx.removed++;
		}

		/* Move forward */
		entry = readdir(pdir);
	}

	closedir(pdir);
	return 0;
}

/*
	Determine tame of oldest media entry in database and delete older files on it. 
	At this case those files are untracked anyway (i.e. not recognized in database).
*/
static int bc_initial_cleanup_untracked_media()
{
	/* Get oldest not archived media entry from database */
	BC_DB_RES dbres = bc_db_get_table("SELECT * FROM Media WHERE archive=0 AND filepath!='' ORDER BY id ASC LIMIT 1");

	if (!dbres) {
		bc_log(Warning, "Failed to get oldest media entry from database");
		return -1;
	}

	if (bc_db_fetch_row(dbres)) {
		bc_log(Info, "Database doesnot contain oldest media file entry");
		bc_db_free_table(dbres);
		return -1;
	}

	const char *filepath = bc_db_get_val(dbres, "filepath", NULL);

	if (!filepath || !*filepath) {
		bc_log(Warning, "Failed to read oldest file path");
		bc_db_free_table(dbres);
		return -1;
	}

	std::string full_path = std::string(filepath);
	if (full_path.length() < 30)
	{
		bc_log(Warning, "Failed to determine oldest media time from path: %s", filepath);
		bc_db_free_table(dbres);
		return -1;
	}

	std::string timed_path = full_path.substr(full_path.size()-30, 26);
	bc_string_array locations;
	bc_oldest_media_t ctx;
	int count, id;

	/* Determine date and time of the oldest media file */
	count = sscanf(timed_path.c_str(), "%04d/%02d/%02d/%06d/%02d-%02d-%02d",
		(int*)&ctx.year, (int*)&ctx.month, (int*)&ctx.day, (int*)&id,
		(int*)&ctx.hour, (int*)&ctx.min, (int*)&ctx.sec);

	if (count != 7) {
		bc_log(Warning, "Failed determine oldest media time");
		bc_db_free_table(dbres);
		return -1;
	}

	bc_log(Info, "Determined oldest media time: %04d/%02d/%02d %02d-%02d-%02d", 
		ctx.year, ctx.month, ctx.day, ctx.hour, ctx.min, ctx.sec);

	if (!bc_get_media_locations(locations)) {
		bc_log(Warning, "No media locations available for cleanup");
		bc_db_free_table(dbres);
		return -1;
	}

	for (size_t i = 0; i < locations.size(); i++)
	{
		bc_log(Info, "Checking location: %s", locations[i].c_str());
		bc_recursive_cleanup_untracked_media(ctx, locations[i]);
	}

	bc_log(Info, "Initial media cleanup finished: removed(%d), archived(%d), errors(%d), others(%d)",
		ctx.removed, ctx.archived, ctx.errors, ctx.others);

	bc_db_free_table(dbres);
	return 0;
}

/*
	Delete everything in the directory thats older than current cleanup file.
	At this case those files are untracked anyway (i.e. not recognized in DB)
*/
static int bc_cleanup_older_media(const char *filepath)
{
	std::string full_path = std::string(filepath);
	std::string dir_path = bc_get_directory_path(full_path);
	std::string file_name = bc_get_file_name(full_path);
	if (dir_path.length() || file_name.length()) return -1;

	char timestr[9];
	int hour = 0, min = 0, sec = 0;
	int archived = 0, removed = 0, error_count = 0;

	snprintf(timestr, sizeof(timestr), "%s", file_name.c_str());
	sscanf(timestr, "%02d-%02d-%02d", (int*)&hour, (int*)&min, (int*)&sec);

	DIR *pdir = opendir(dir_path.c_str());
	if (pdir == NULL) {
		bc_log(Warning, "Can not open directory %s: %s", dir_path.c_str(), strerror(errno));
		return -1;
	}

	struct dirent *entry = readdir(pdir);
	while (entry != NULL)
	{
		/* Found an entry, but ignore . and .. */
		if (strcmp(".", entry->d_name) == 0 ||
			strcmp("..", entry->d_name) == 0) {
			entry = readdir(pdir);
			continue;
		}

		timestr[0] = '\0';
		bool entry_is_old = false;
		int found_hour = 0, found_min = 0, found_sec = 0;

		snprintf(timestr, sizeof(timestr), "%s", entry->d_name);
		sscanf(timestr, "%02d-%02d-%02d", (int*)&found_hour, (int*)&found_min, (int*)&found_sec);

		/* Check if found entry is older than last deleted file */
		if (found_hour < hour) entry_is_old = true;
		else if (found_hour == hour && found_min < min) entry_is_old = true;
		else if (found_hour == hour && found_min == min && found_sec < sec) entry_is_old = true;

		if (entry_is_old) {
			full_path = dir_path + std::string(entry->d_name);
			std::string video_file = full_path;
			video_file.replace(video_file.size()-3, 3, "mkv");

			int rv = bc_media_is_archived(video_file.c_str());
			if (rv > 0) {
				entry = readdir(pdir);
				archived++;
				continue;
			} else if (rv < 0) {
				entry = readdir(pdir);
				error_count++;
				continue;
			}

			struct stat statbuf;
			if (lstat(full_path.c_str(), &statbuf) < 0) {
				bc_log(Error, "Can not stat file: %s", full_path.c_str());
				entry = readdir(pdir);
				error_count++;
				continue;
			}

			if (S_ISDIR(statbuf.st_mode)) {
				bc_remove_directory(full_path);
				entry = readdir(pdir);
				continue;
			} else if (unlink(full_path.c_str()) < 0) {
				bc_log(Warning, "Cannot remove old file %s: %s",
			       full_path.c_str(), strerror(errno));

				entry = readdir(pdir);
				error_count++;
				continue;
			}

			/* Remove file from retry list if exists */
			bc_media_files_it it = g_media_files.find(full_path);
			if (it != g_media_files.end()) g_media_files.erase(it);

			removed++;
		}

		/* Move forward */
		entry = readdir(pdir);
	}

	if (removed || error_count) {
		bc_log(Info, "Cleaned up %d older files, archived(%d), errors(%d)",
			removed, archived, error_count);
	}

	closedir(pdir);
	return 0;
}

static int bc_cleanup_media()
{
	BC_DB_RES dbres;
	int removed = 0, error_count = 0;

	/* Retry last cleanup list first */
	bc_cleanup_media_retry();

	/* XXX: We limit the files to 100 because otherwise updating the db
	 * would take too long */
	dbres = bc_db_get_table("SELECT * FROM Media WHERE archive=0 AND "
				"filepath!='' ORDER BY id ASC LIMIT 100");

	if (!dbres) {
		bc_status_component_error("Database error during media cleanup");
		return -1;
	}

	while (!bc_db_fetch_row(dbres)) {
		const char *filepath = bc_db_get_val(dbres, "filepath", NULL);
		int id = bc_db_get_val_int(dbres, "id");

		if (!filepath || !*filepath)
			continue;

		if (unlink(filepath) < 0 && errno != ENOENT) {
			bc_log(Warning, "Cannot remove file %s for cleanup: %s",
			       filepath, strerror(errno));
			error_count++;
			continue;
		} else if (errno == ENOENT) {
			// Maybe dangling link caused by mount error or permission issue, try again
			g_media_files[filepath].timestamp = time(NULL);
			g_media_files[filepath].try_count = 0;
		}

		std::string sidecar = filepath;
		if (sidecar.size() > 3)
			sidecar.replace(sidecar.size()-3, 3, "jpg");

		if (unlink(sidecar.c_str()) < 0 && errno != ENOENT) {
			bc_log(Warning, "Cannot remove sidecar file %s for cleanup: %s",
			       sidecar.c_str(), strerror(errno));
			error_count++;
			continue;
		} else if (errno == ENOENT) {
			// Maybe sidecar is not created yet, try again
			g_media_files[sidecar].timestamp = time(NULL);
			g_media_files[sidecar].try_count = 0;
		}

		sync();

		removed++;

		if (bc_db_query("DELETE FROM Media WHERE id=%d", id)) {
			bc_status_component_error("Database error during Media cleanup");
		}

		/* Delete older files than last deleted file */
		bc_cleanup_older_media(filepath);
		/* Check containing directory and remove if empty */
		bc_remove_dir_if_empty(filepath);

		/* Check if the conditions changed */
		bool enough_space_available = false;
		for (int i = 0; i < MAX_STOR_LOCS && media_stor[i].min_thresh; i++) {
			float used = path_used_percent(media_stor[i].path);
			if (used >= 0 && used <= media_stor[i].min_thresh) {
				enough_space_available = true;
				break;
			}
		}

		if (enough_space_available && !is_media_max_age_exceeded())
			goto done;
	}

done:
	bc_db_free_table(dbres);

	if (error_count)
		bc_log(Error, "Cleaned up %d files with %d errors, which may cause undeletable files", removed, error_count);
	else
		bc_log(Info, "Cleaned up %d files", removed);

	return 0;
}

static int bc_check_media(void)
{
	int ret = 0;
	bool storage_overloaded = false;

	/* If there's some space left, skip cleanup */
	for (int i = 0; i < MAX_STOR_LOCS && media_stor[i].max_thresh; i++) {
		if (is_storage_full(&media_stor[i]) == 1) {
			storage_overloaded = true;
			break;
		}
	}

	if (storage_overloaded || is_media_max_age_exceeded()) {
		if (!g_cleanup_manager) {
			g_cleanup_manager = std::make_unique<CleanupManager>();
		}
		ret = g_cleanup_manager->run_cleanup();
		
		// Log cleanup statistics
		const cleanup_stats_report& stats = g_cleanup_manager->get_stats_report();
		bc_log(Info, "Cleanup stats: processed=%d, deleted=%d, errors=%d, bytes_freed=%zu, retries=%d",
			   stats.files_processed, stats.files_deleted,
			   stats.errors, stats.bytes_freed, stats.retries);
	}

	return ret;
}

static int bc_check_db(void)
{
	struct bc_record *bc_rec;
	BC_DB_RES dbres;
	int re = 0;
	int i;

	pthread_mutex_lock(&bc_rec_list_lock);
	for (i = bc_rec_list.size() - 1; i >= 0; i--) {
		bc_rec = bc_rec_list[i];
		bc_rec->cfg_just_updated = false;
	}
	pthread_mutex_unlock(&bc_rec_list_lock);

	dbres = bc_db_get_table("SELECT * from Devices LEFT JOIN "
				"AvailableSources USING (device)");

	if (!dbres)
		return -1;

	while (!bc_db_fetch_row(dbres)) {
		const char *proto = bc_db_get_val(dbres, "protocol", NULL);
		int id = bc_db_get_val_int(dbres, "id");

		if (id < 0 || !proto)
			continue;

		bc_rec = bc_record_exists(id);
		if (bc_rec) {
			if (bc_record_update_cfg(bc_rec, dbres)) {
				bc_status_component_error("Database error while updating"
					"device configuration");
				re |= -1;
			}
			bc_rec->cfg_just_updated = true;
			continue;
		}

		/* Caller asked us to only use this record_id */
		if (record_id >= 0 && id != record_id)
			continue;

		if (bc_db_get_val_bool(dbres, "disabled"))
			continue;

		/* If this is a V4L2 device, it needs to be detected */
		if (!strcasecmp(proto, "V4L2")) {
			int card_id = bc_db_get_val_int(dbres, "card_id");
			if (!hwcard_ready || card_id < 0)
				continue;
		}

		bc_rec = bc_record::create_from_db(id, dbres);
		if (!bc_rec) {
			re |= -1;
			continue;
		}

		cur_threads++;
		bc_rec->cfg_just_updated = true;
		pthread_mutex_lock(&bc_rec_list_lock);
		bc_rec->hls_stream = hls;
		bc_rec_list.push_back(bc_rec);
		pthread_mutex_unlock(&bc_rec_list_lock);
	}

	// Traverse bc_rec_list and remove entries not having "have just been updated" flag set
	pthread_mutex_lock(&bc_rec_list_lock);
	for (i = bc_rec_list.size() - 1; i >= 0; i--) {
		bc_rec = bc_rec_list[i];
		if (!bc_rec->cfg_just_updated) {
			bc_rec->thread_should_die = "DB entry deleted";
			bc_rec_list.erase(bc_rec_list.begin() + i);
		}
	}
	pthread_mutex_unlock(&bc_rec_list_lock);

	bc_db_free_table(dbres);
	return re;
}

static void get_last_run_date()
{
	BC_DB_RES dbres;
	dbres = bc_db_get_table("SELECT timestamp FROM ServerStatus");

	if (dbres && !bc_db_fetch_row(dbres))
		last_known_running = bc_db_get_val_int(dbres, "timestamp");

	if (dbres)
		bc_db_free_table(dbres);
}

static void *bc_update_abandoned_media_threadproc(void *arg)
{
	/* Running mkvinfo could take a significant amount time on long recordings,
	 * so processing of abandoned recordings is done in this separate thread.
	 */
	bc_log(Info, "started processing abandoned recordings");
	while(!abandoned_media_to_update.empty()) {
		char cmd[4096];
		char *line = NULL;
		size_t len;
		FILE *fp;
		struct media_record m;

		m = abandoned_media_to_update.front();
		sprintf(cmd, "mkvinfo -v %s", m.filepath.c_str());

		fp = popen(cmd, "r");
		if (fp == NULL)
			continue;

		while (getline(&line, &len, fp) >= 0) {
			char *p;
			if ((p = strstr(line, "timecode ")))
				sscanf(p, "timecode %d.", &m.duration);
			free(line);
			line = NULL;
		}

		free(line);

		pclose(fp);

		pthread_mutex_lock(&mutex_abandoned_media);
		abandoned_media_updated.push(m);
		pthread_mutex_unlock(&mutex_abandoned_media);

		abandoned_media_to_update.pop();
	}

	bc_log(Info, "finished processing abandoned recordings");

	return NULL;
}

static void bc_check_abandoned_media_updates()
{
	/* Called from main loop, updates lengths of processed recordings in database */
	bc_log(Info, "Updating length of %ld abandoned recordings", abandoned_media_updated.size());
	while(!abandoned_media_updated.empty()) {
		struct media_record m;

		pthread_mutex_lock(&mutex_abandoned_media);
		m = abandoned_media_updated.front();
		pthread_mutex_unlock(&mutex_abandoned_media);

		if (!m.duration) {
			bc_log(Info, "Deleting empty media %s", m.filepath.c_str());
			bc_db_query("DELETE FROM Media WHERE id=%u", m.media_id);

			if (unlink(m.filepath.c_str()) < 0) {
				g_media_files[m.filepath].timestamp = time(NULL);
				g_media_files[m.filepath].try_count = 0;
			}

		} else {
			bc_log(Info, "Updating length of abandoned media %s to %d", m.filepath.c_str(), m.duration);
			bc_db_query("UPDATE EventsCam SET length=%d WHERE "
				    "id=%d", m.duration, m.id);
		}

		pthread_mutex_lock(&mutex_abandoned_media);
		abandoned_media_updated.pop();
		pthread_mutex_unlock(&mutex_abandoned_media);
	}

	if (abandoned_media_to_update.empty())
		abandoned_media_update_in_progress = false;
}

static void bc_check_inprogress(void)
{
	BC_DB_RES dbres;

	dbres = bc_db_get_table("SELECT EventsCam.*, Media.filepath FROM EventsCam "
	                        "LEFT JOIN Media ON (EventsCam.media_id=Media.id) "
	                        "WHERE length=-1");

	if (dbres == NULL)
		return;

	while (!bc_db_fetch_row(dbres)) {
		const char *filepath = bc_db_get_val(dbres, "filepath", NULL);
		int event_id = bc_db_get_val_int(dbres, "id");
		int m_id = bc_db_get_val_int(dbres, "media_id");

		if (!filepath) {
			int duration = 0;
			bc_log(Info, "Event %d left in-progress at shutdown; setting duration", event_id);
			time_t start = bc_db_get_val_int(dbres, "time");
			duration = int64_t(last_known_running) - int64_t(start);
			if (duration < 0)
				duration = 0;
			bc_db_query("UPDATE EventsCam SET length=%d WHERE id=%d", duration, event_id);
			continue;
		}

		struct media_record m;

		m.id = event_id;
		m.media_id = m_id;
		m.filepath = std::string(filepath);
		m.duration = 0;

		abandoned_media_to_update.push(m);
		abandoned_media_update_in_progress = true;
		bc_log(Info, "Found abandoned recording %s, queued for length update", m.filepath.c_str());
        }
	if (abandoned_media_update_in_progress) {
		pthread_t thread_id;
		pthread_attr_t attr;

		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread_id, NULL, bc_update_abandoned_media_threadproc, NULL);
		pthread_attr_destroy(&attr);
	}

	bc_db_free_table(dbres);
}

static void usage(const char *progname)
{
	fprintf(stderr, "Usage: %s [-s]\n", progname);
	fprintf(stderr, "  -s\tDo not background\n");
	fprintf(stderr, "  -l\tLogging level ([d]ebug, [i]nfo, [w]arning, [e]rror, [b]ug, [f]atal)\n");
	fprintf(stderr, "  -f\tSpecify configuration file (default: %s)\n", BC_CONFIG_DEFAULT);
	fprintf(stderr, "  -u\tDrop privileges to user\n");
	fprintf(stderr, "  -g\tDrop privileges to group\n");
	fprintf(stderr, "  -r\tRecord a specific ID only\n");
}

static int open_db_loop(const char *config_file)
{
	for (int count = 1; bc_db_open(config_file); sleep(1), count++)
		if (count >= 30)
			goto db_error;
	return 0;

db_error:
	bc_log(Error, "Could not open database after 30 tries, aborting");
	return -1;
}

static log_level str_to_log_level(const char *str)
{
	switch (tolower(str[0]))
	{
		case 'd': return Debug;
		case 'i': return Info;
		case 'w': return Warning;
		case 'e': return Error;
		case 'b': return Bug;
		case 'f': return Fatal;
		default: return Info;
	}
}

static
void db_cleanup(void)
{
	bc_db_query("DELETE FROM Media WHERE filepath=''");
}

static void xml_general_status(pugi::xml_node& node)
{
	node.append_attribute("Revision") = GIT_REVISION;
	node.append_attribute("PID") = getpid();
}

static void xml_solo6x10_status(pugi::xml_node& node)
{
	// TODO FIXME Volatile or mutex-guarded
	node.append_attribute("hwcard_ready") = hwcard_ready;
}

static void xml_devices_status(pugi::xml_node& node)
{
	pthread_mutex_lock(&bc_rec_list_lock);
	for (auto it = bc_rec_list.begin(); it != bc_rec_list.end(); it++) {
		pugi::xml_node device = node.append_child("Device");
		(*it)->getStatusXml(device);
	}
	pthread_mutex_unlock(&bc_rec_list_lock);
}

static void xml_status_callback(pugi::xml_document& xmldoc)
{
	pugi::xml_node rootNode = xmldoc.append_child();
	rootNode.set_name("Status");

	// General state
	pugi::xml_node subNode = rootNode.append_child("General");
	xml_general_status(subNode);

	// Devices status
	subNode = rootNode.append_child("Devices");
	xml_devices_status(subNode);

	// RTSP server report
	subNode = rootNode.append_child("RTSP");
	rtsp->getStatusXml(subNode);

	// solo6x10 cards status
	subNode = rootNode.append_child("solo6x10");
	xml_solo6x10_status(subNode);
}

static void setup_vaapi()
{
	const char autodetect[] = "Autodetect";
	const char none[] = "None";
	const char *renderNode = autodetect;
	BC_DB_RES dbres;

	dbres = bc_db_get_table("SELECT value from GlobalSettings WHERE "
                                "parameter='G_VAAPI_DEVICE'");

        if (!dbres)
                bc_status_component_error("Database failure for VAAPI device parameter");

        if (dbres && !bc_db_fetch_row(dbres)) {
                renderNode = bc_db_get_val(dbres, "value", NULL);
	}

	if (strncasecmp(renderNode, autodetect, sizeof autodetect) == 0) {
		int ret = system("/usr/bin/php /usr/share/bluecherry/www/lib/vaapi_autodetect.php");
		if (ret != 0) {
			bc_log(Warning, "VAAPI autodetect script failed with exit code %d", ret);
		}

		if (dbres)
			bc_db_free_table(dbres);

		dbres = bc_db_get_table("SELECT value from GlobalSettings WHERE "
					"parameter='G_VAAPI_DEVICE'");

		if (!dbres)
			bc_status_component_error("Database failure for VAAPI device parameter");

		if (dbres && !bc_db_fetch_row(dbres)) {
			renderNode = bc_db_get_val(dbres, "value", NULL);
			bc_log(Info, "Auto-detected VAAPI device: %s", renderNode);
		}
	}

	if (strncasecmp(renderNode, none, sizeof none) == 0)
		goto exit;

	if (access(renderNode, R_OK) == -1) {
		bc_log(Warning, "VAAPI device %s does not exist or is not readable to bluecherry, hardware acceleration is not available", renderNode);
		goto exit;
	}

	if (!vaapi_hwaccel::init(renderNode)) {
		bc_log(Warning, "Failed to initialize VAAPI device %s, VAAPI hardware acceleration is not available", renderNode);
	} else
		bc_log(Info, "Initialized render node %s for VAAPI hardware acceleration", renderNode);

exit:
	if (dbres)
		bc_db_free_table(dbres);
}

int main(int argc, char **argv)
{
	int opt;
	int bg = 1;
	int ret;
	const char *config_file = BC_CONFIG_DEFAULT;
	const char *user = 0, *group = 0;
	int hwcard_down_reported = 0;

	bc_syslog_init();
	pthread_setname_np(pthread_self(), "MAIN");

	umask(007);

	while ((opt = getopt(argc, argv, "hsm:r:u:g:l:f:")) != -1) {
		switch (opt) {
		case 's': bg = 0; break;
		case 'r': record_id = atoi(optarg); break;
		case 'u': user = optarg; break;
		case 'g': group = optarg; break;
		case 'l': log_context::default_context().set_level(str_to_log_level(optarg)); break;
		case 'f': config_file = optarg; break;
		case 'h':
		default:
			usage(argv[0]);
			return 1;
		}
	}

	server_log::open();

	if (user || group) {
		struct passwd *u   = 0;
		gid_t          gid = 0;

		if (group) {
			struct group *g = 0;
			if (!(g = getgrnam(group))) {
				bc_log(Fatal, "Group '%s' does not exist", group);
				return 1;
			}
			gid = g->gr_gid;
		}

		if (user) {
			if (!(u = getpwnam(user))) {
				bc_log(Fatal, "User '%s' does not exist", user);
				return 1;
			}
			if (!group)
				gid = u->pw_gid;
			if (initgroups(user, gid) < 0) {
				bc_log(Fatal, "Setting supplemental groups failed");
				return 1;
			}
		}

		if (setregid(gid, gid) < 0) {
			bc_log(Fatal, "Setting group failed");
			return 1;
		}

		if (u && setreuid(u->pw_uid, u->pw_uid) < 0) {
			bc_log(Fatal, "Setting user failed");
			return 1;
		}
	}

	signals_setup();

	/* XXX This is not suitable for much of anything, really. */
	srand((unsigned)(getpid() * time(NULL)));

	bc_ffmpeg_init();

	if (bg && daemon(0, 0) == -1) {
		bc_log(Fatal, "Daemonization failed");
		return 1;
	}

	status_server *status_serv = new status_server(xml_status_callback);
	ret = status_serv->reconfigure("/tmp/bluecherry_status");
	if (ret) {
		bc_log(Error, "Failed to setup the status server");
		return 1;
	}

	ret = trigger_server::Instance().reconfigure("/tmp/bluecherry_trigger");
	if (ret) {
		bc_log(Error, "Failed to setup the trigger server");
		return 1;
	}

	bc_log(Info, "Started bc-server " BC_VERSION " (toolchain "
	       __VERSION__ ") " GIT_REVISION );

	/* Mutex */
	bc_initialize_mutexes();

	/* Start monitoring */
	bc_stats stats;
	stats.start_monithoring();

	api = new bc_api;
	api->set_stats(&stats);

	if (!api->start_listener(7005)) {
		bc_log(Error, "Failed to setup API listener");
		delete api;
		return 1;
	} else {
		std::thread api_th(&bc_api::run, api);
		api_th.detach();
	}

	rtsp = new rtsp_server;
	if (rtsp->setup(7002)) {
		delete rtsp;
		rtsp = NULL;

		bc_log(Error, "Failed to setup RTSP server");
		return 1;
	}

	hls = new hls_listener;
	if (!hls->register_listener(7003)) {
		bc_log(Error, "Failed to setup HLS listener");
		delete hls;
		return 1;
	} else {
		hls->set_auth(true); // enable authentication
		std::thread hls_th(&hls_listener::run, hls);
		hls_th.detach();
	}

#ifdef V3_LICENSING
	v3license = new v3license_server;
	if (v3license->setup(7004)) {
		delete rtsp;
		rtsp = NULL;

		delete v3license;
		v3license = NULL;

		bc_log(Error, "Failed to setup V3LICENSE server");
		return 1;
	}
#endif /* V3_LICENSING */

	if (open_db_loop(config_file))
		return 1;

	bc_log(Info, "SQL database connection opened");

	db_cleanup();

	setup_vaapi();

	bc_status_component_begin(STATUS_DB_POLLING1);
	{
		int ret = bc_check_globals();
		/* Do some cleanup */
		get_last_run_date();
		bc_check_inprogress();
		bc_status_component_end(STATUS_DB_POLLING1, ret == 0);
	}

	/* Delete untracked media files */
	bc_initial_cleanup_untracked_media();

	pthread_t rtsp_thread;
	pthread_create(&rtsp_thread, NULL, rtsp_server::runThread, rtsp);

#ifdef V3_LICENSING
	pthread_t v3license_thread;
	pthread_create(&v3license_thread, NULL, v3license_server::runThread, v3license);
#endif /* V3_LICENSING */

	/* Main loop */
	for (unsigned int loops = 0 ;; sleep(1), loops++) {
		if (shutdown_flag) {
			bc_log(Info, "Shutting down");
			break;
		}
		/* Every 16 seconds until initialized, then every 4:16 minutes */
		if ((!hwcard_ready && !(loops & 15)) || (hwcard_ready && !(loops & 255))) {
			bc_status_component_begin(STATUS_HWCARD_DETECT);
			int ret = bc_check_avail();
			hwcard_ready = (ret == 0);
			if (ret != 0 && !hwcard_down_reported) {
				hwcard_down_reported = 1;

				bc_log(Error, "Some hardware capture devices are not initialized yet");

				bc_status_component_error(
					"Hardware capture devices are not initialized: %s",
					(ret == -EAGAIN) ?
					"Driver not ready" : strerror(-ret));

				int system_ret = system("php /usr/share/bluecherry/www/lib/mailer.php 'solo' 'DOWN'");
				/* Below is recommended by system(3) man page */
				if (WIFSIGNALED(system_ret) && (WTERMSIG(system_ret) == SIGINT || WTERMSIG(system_ret) == SIGQUIT))
					break;
				if (system_ret)
					bc_log(Error, "Failed to run mailer.php for notification");
			} else if (ret == 0 && hwcard_down_reported) {
				hwcard_down_reported = 0;

				bc_log(Error, "Hardware capture devices got initialized after being down");

				int system_ret = system("php /usr/share/bluecherry/www/lib/mailer.php 'solo' 'RECOVERED'");
				/* Below is recommended by system(3) man page */
				if (WIFSIGNALED(system_ret) && (WTERMSIG(system_ret) == SIGINT || WTERMSIG(system_ret) == SIGQUIT))
					break;
				if (system_ret)
					bc_log(Error, "Failed to run mailer.php for notification");
			}
			bc_status_component_end(STATUS_HWCARD_DETECT, ret == 0);
		}

		bc_status_component_begin(STATUS_MEDIA_CHECK);
		/* Check media locations for full */
		int mc_ret = bc_check_media();
		bc_status_component_end(STATUS_MEDIA_CHECK, mc_ret == 0);

		/* Every 8 seconds */
		if ((loops & 7) == 0) {
			bc_status_component_begin(STATUS_DB_POLLING1);
			/* Check global vars */
			int error = bc_check_globals();
			/* Check for changes in cameras */
			error |= bc_check_db();
			bc_status_component_end(STATUS_DB_POLLING1, error == 0);
		}

		/* Every 16 seconds */
		if (abandoned_media_update_in_progress && (loops % 15) == 0)
			bc_check_abandoned_media_updates();

		/* Every second, check for dead threads */
		bc_check_threads();

		if (!(loops & 63))
			bc_update_server_status();
	}

	stats.stop_monithoring();
#ifdef V3_LICENSING
	v3license_server::stopThread();
	pthread_join(v3license_thread, NULL);
#endif /* V3_LICENSING */

	bc_stop_threads();
	bc_db_close();
	bc_ffmpeg_teardown();

	pthread_rwlock_destroy(&media_lock);

	return 0;
}
