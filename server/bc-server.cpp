/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
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

extern "C" {
#include <libavutil/log.h>
}

#include "bc-server.h"
#include "rtsp.h"
#include "watchdog.h"

/* Global Mutexes */
pthread_mutex_t mutex_global_sched;
pthread_mutex_t mutex_streaming_setup;

static std::vector<bc_record*> bc_rec_list;

static int max_threads;
static int cur_threads;
static int record_id = -1;
static int solo_ready = 0;

char global_sched[7 * 24 + 1];

static pthread_mutex_t media_lock = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

struct bc_storage {
	char path[PATH_MAX];
	float min_thresh, max_thresh;
};

#define MAX_STOR_LOCS	10
static struct bc_storage media_stor[MAX_STOR_LOCS];

/* Timestamp of when the server was last known to be running prior to this
 * instance; may be zero if unknown or never */
time_t last_known_running;

/* Prerelease */
#define DONT_ENFORCE_LICENSING 0
#define TRIAL_DAYS 30
#define TRIAL_MAX_DEVICES 32
static std::vector<bc_license> licenses;


static char *component_error[NUM_STATUS_COMPONENTS];
static char *component_error_tmp;
/* XXX XXX XXX thread-local */
static bc_status_component status_component_active = (bc_status_component)-1;

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
}

static const char *component_string(bc_status_component c)
{
	switch (c) {
		case STATUS_DB_POLLING1: return "database-1";
		case STATUS_MEDIA_CHECK: return "media";
		case STATUS_LICENSE: return "licensing";
		case STATUS_SOLO_DETECT: return "solo6x10";
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
			asprintf(&full_error, "[%s] %s", component_str, component_error[i]);
			full_error_sz = strlen(full_error);
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


static int load_storage_paths(void)
{
	BC_DB_RES dbres;

	dbres = bc_db_get_table("SELECT * from Storage ORDER BY priority ASC");

	if (!dbres) {
		bc_status_component_error("Database failure for storage paths");
		return -1;
	}

	if (pthread_mutex_lock(&media_lock) == EDEADLK)
		bc_log(Error, "Deadlock detected in media_lock on db_check");

	memset(media_stor, 0, sizeof(media_stor));

	int i = 0;
	while (!bc_db_fetch_row(dbres) && i < MAX_STOR_LOCS) {
		const char *path = bc_db_get_val(dbres, "path", NULL);
		float max_thresh = bc_db_get_val_float(dbres, "max_thresh");
		float min_thresh = bc_db_get_val_float(dbres, "min_thresh");

		if (!path || !*path || max_thresh <= 0 || min_thresh <= 0)
			continue;

		if (strlcpy(media_stor[i].path, path, sizeof(media_stor[i].path)) >=
		    sizeof(media_stor[i].path))
			continue;

		media_stor[i].max_thresh = max_thresh;
		media_stor[i].min_thresh = min_thresh;

		if (bc_mkdir_recursive(media_stor[i].path)) {
			bc_status_component_error("Cannot create storage path %s: %m",
			                          media_stor[i].path);
		} else
			i++;
	}

	/* Cleanup in case of the last location failing */
	if (i < MAX_STOR_LOCS)
		memset(&media_stor[i], 0, sizeof(media_stor[i]));

	if (i == 0) {
		/* Fall back to one single default location */
		strcpy(media_stor[0].path, "/var/lib/bluecherry/recordings");
		if (bc_mkdir_recursive(media_stor[0].path)) {
			bc_status_component_error("Cannot create storage path %s: %m",
			                          media_stor[i].path);
		}
		media_stor[0].max_thresh = 95.00;
		media_stor[0].min_thresh = 90.00;
	}

	pthread_mutex_unlock(&media_lock);

	bc_db_free_table(dbres);
	return 0;
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

	/* Get path to media storage locations, or use default */
	return load_storage_paths();
}

static void bc_stop_threads(void)
{
	for (auto it = bc_rec_list.begin(); it != bc_rec_list.end(); it++)
		(*it)->thread_should_die = "Shutting down";

	char *errmsg = 0;
	for (auto it = bc_rec_list.begin(); it != bc_rec_list.end(); it++) {
		bc_record *bc_rec = *it;
		pthread_join(bc_rec->thread, (void **)&errmsg);
		bc_rec->log.log(Info, "Device stopped: %s", errmsg);
		delete bc_rec;
		cur_threads--;
	}

	bc_rec_list.clear();
}

/* Check for threads that have quit */
static void bc_check_threads(void)
{
	char *errmsg = 0;

	for (unsigned int i = 0; i < bc_rec_list.size(); i++) {
		bc_record *bc_rec = bc_rec_list[i];
		if (pthread_tryjoin_np(bc_rec->thread, (void **)&errmsg))
			continue;

		bc_rec->log.log(Info, "Device stopped: %s", errmsg);
		delete bc_rec;
		cur_threads--;

		bc_rec_list.erase(bc_rec_list.begin()+i);
		i--;
	}
}

static struct bc_record *bc_record_exists(const int id) __attribute__((pure));

static struct bc_record *bc_record_exists(const int id)
{
	for (auto it = bc_rec_list.begin(); it != bc_rec_list.end(); it++) {
		if ((*it)->id == id)
			return *it;
	}

	return NULL;
}

/* TODO: use fixed point insead of float */
static float path_used_percent(const char *path)
{
	struct statvfs st;

	if (statvfs(path, &st))
		return -1.00;

	return 100.0 - 100.0 * (float)st.f_bavail / (float)st.f_blocks;
}

static uint64_t path_freespace(const char *path)
{
	struct statvfs st;

	if (statvfs(path, &st))
		return 0;

	if (!st.f_favail) {
		bc_log(Info, "No available inodes on %s", path);
		return 0;
	}

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

	for (p = media_stor; p < media_stor+MAX_STOR_LOCS && p->max_thresh; p++) {
		uint64_t cur, best = 0, best_overused = 0;

		cur = path_freespace(p->path);
		if (cur < best)
			continue;

		if (!is_storage_full(p)) {
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

int bc_get_media_loc(char *dest, size_t size)
{
	int ret = 0;
	dest[0] = 0;

	if (pthread_mutex_lock(&media_lock) == EDEADLK)
		bc_log(Error, "Deadlock detected in media_lock on get_loc");

	const char *sel = select_best_path();

	if (!sel || strlcpy(dest, sel, size) > size)
		ret = -1;

	pthread_mutex_unlock(&media_lock);
	return ret;
}

static int bc_cleanup_media()
{
	BC_DB_RES dbres;
	int removed = 0, error_count = 0;

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
		}

		std::string sidecar = filepath;
		if (sidecar.size() > 3)
			sidecar.replace(sidecar.size()-3, 3, "jpg");
		if (unlink(sidecar.c_str()) < 0 && errno != ENOENT) {
			bc_log(Warning, "Cannot remove sidecar file %s for cleanup: %s",
			       sidecar.c_str(), strerror(errno));
			error_count++;
			continue;
		}

		removed++;

		if (__bc_db_query("DELETE FROM Media WHERE id=%d", id)) {
			bc_status_component_error("Database error during Media cleanup");
		}

		/* Every four files removed check if enough space has been
		 * freed.
		 */
		if ((removed & 3) == 0) {
			for (int i = 0; i < MAX_STOR_LOCS && media_stor[i].min_thresh; i++) {
				float used = path_used_percent(media_stor[i].path);
				if (used >= 0 && used <= media_stor[i].min_thresh)
					goto done;
			}
		}
        }

done:
	bc_db_free_table(dbres);

	if (error_count)
		bc_log(Error, "Cleaned up %d files with %d errors, which may cause undeletable files", removed, error_count);
	else
		bc_log(Info, "Cleaned up %d files", removed);

	return 0;
}

static void bc_estimate_recording_time(const char *path)
{
	const char query[] = "UPDATE Storage SET record_time = ("
		" SELECT (%" PRId64 ") * period / size + period"
		" FROM (SELECT SUM("
			"CASE WHEN end - start < 0 THEN 0 ELSE end - start END"
		") period, SUM(size) DIV 1048576 size"
		" FROM Media WHERE filepath LIKE '%s%%') q)"
		" WHERE path = '%s'";

	uint64_t free_space = path_freespace(path) >> 20;
	__bc_db_query(query, free_space, path, path);
}

static int bc_check_media(void)
{
	if (pthread_mutex_lock(&media_lock) == EDEADLK)
		bc_log(Error, "Deadlock detected in media_lock on check_media");

	int ret = 0;

	/* If there's some space left, skip cleanup */
	for (int i = 0; i < MAX_STOR_LOCS && media_stor[i].max_thresh; i++) {
		if (!is_storage_full(&media_stor[i]))
			goto out;
	}

	ret = bc_cleanup_media();

 out:
	pthread_mutex_unlock(&media_lock);

	for (int i = 0; i < MAX_STOR_LOCS; i++)
		bc_estimate_recording_time(media_stor[i].path);

	return ret;
}

static int bc_check_db(void)
{
	struct bc_record *bc_rec;
	BC_DB_RES dbres;
	int re = 0;

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
			if (!solo_ready || card_id < 0)
				continue;
		}

		/* Maximum threads, used for licensing control */
		if (!DONT_ENFORCE_LICENSING && max_threads >= 0 && cur_threads >= max_threads) {
			bc_status_component_error("Device %d disabled due to licensing "
				"restrictions (%d/%d devices used)", id, cur_threads, max_threads);
			re |= -1;
			continue;
		}

		bc_rec = bc_record::create_from_db(id, dbres);
		if (!bc_rec) {
			re |= -1;
			continue;
		}

		cur_threads++;
		bc_rec_list.push_back(bc_rec);
	}

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
		int duration = 0;
		char cmd[4096];
		char *line = NULL;
		size_t len;
		FILE *fp;
		int event_id = bc_db_get_val_int(dbres, "id");

		if (!filepath) {
			bc_log(Info, "Event %d left in-progress at shutdown; setting duration", event_id);
			time_t start = bc_db_get_val_int(dbres, "time");
			duration = int64_t(last_known_running) - int64_t(start);
			if (duration < 0)
				duration = 0;
			bc_db_query("UPDATE EventsCam SET length=%d WHERE id=%d", duration, event_id);
			continue;
		}

		sprintf(cmd, "mkvinfo -v %s", filepath);

		fp = popen(cmd, "r");
		if (fp == NULL)
			continue;

		while (getline(&line, &len, fp) >= 0) {
			char *p;
			if ((p = strstr(line, "timecode ")))
				sscanf(p, "timecode %d.", &duration);
			free(line);
			line = NULL;
		}

		if (!duration) {
			unsigned int e_id, m_id;

			e_id = bc_db_get_val_int(dbres, "id");
			m_id = bc_db_get_val_int(dbres, "media_id");

			bc_log(Info, "Deleting empty media %s", filepath);

			bc_db_query("DELETE FROM EventsCam WHERE id=%u", e_id);
			bc_db_query("DELETE FROM Media WHERE id=%u", m_id);

			unlink(filepath);
		} else {
			unsigned int id = bc_db_get_val_int(dbres, "id");

			bc_log(Info, "Updating length of abandoned media %s to %d", filepath, duration);
			bc_db_query("UPDATE EventsCam SET length=%d WHERE "
				    "id=%d", duration, id);
		}

		pclose(fp);
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


/* Returns 0 if okay, otherwise -1 and outputs an error */
static int check_trial_expired()
{
	struct stat d;
	time_t now = time(NULL);
	FILE *f;

	if (stat("/var/lib/bluecherry", &d) != 0 || d.st_ctime > now ||
		(now - d.st_ctime >= TRIAL_DAYS*60*60*24)) {
		goto fail;
	}

	f = fopen("/var/lib/.bcins", "r");
	if (f) {
		char buf[16];
		int64_t ftime = 0;

		size_t len = fread(&buf, 1, sizeof(buf)-1, f);
		buf[len] = 0;
		fclose(f);

		ftime = atoll(buf);
		if (ftime <= now && (now - ftime < TRIAL_DAYS*60*60*24))
			return 0;
	}

	fail:
	bc_status_component_error("The %d day trial of Bluecherry has expired. You can "
		"buy a license from http://store.bluecherry.net/licenses", TRIAL_DAYS);
	return -1;
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

int main(int argc, char **argv)
{
	int opt;
	int bg = 1;
	const char *config_file = BC_CONFIG_DEFAULT;
	const char *user = 0, *group = 0;

	umask(027);

	while ((opt = getopt(argc, argv, "hsm:r:u:g:l:")) != -1) {
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

	if (av_lockmgr_register(bc_av_lockmgr)) {
		bc_log(Fatal, "libav lock registration failed: %m");
		exit(1);
	}

	signals_setup();

	/* XXX This is not suitable for much of anything, really. */
	srand((unsigned)(getpid() * time(NULL)));

	bc_libav_init();

	if (bg && daemon(0, 0) == -1) {
		bc_log(Fatal, "Fork failed: %m");
		return 1;
	}

	bc_log(Info, "Started bc-server " BC_VERSION " (toolchain "
	       __VERSION__ ")");

	/* Mutex */
	bc_initialize_mutexes();

	rtsp_server *rtsp = new rtsp_server;
	rtsp->setup(7002);

	if (open_db_loop(config_file))
		return 1;

	bc_log(Info, "SQL database connection opened");

	bc_status_component_begin(STATUS_DB_POLLING1);
	{
		int ret = bc_check_globals();
		/* Do some cleanup */
		get_last_run_date();
		bc_check_inprogress();
		bc_status_component_end(STATUS_DB_POLLING1, ret == 0);
	}

	pthread_t rtsp_thread;
	pthread_create(&rtsp_thread, NULL, rtsp_server::runThread, rtsp);

	/* Main loop */
	for (unsigned int loops = 0 ;; sleep(1), loops++) {
		/* Every 16 seconds until initialized, then every 4:16 minutes */
		if ((!solo_ready && !(loops & 15)) || (solo_ready && !(loops & 255))) {
			bc_status_component_begin(STATUS_SOLO_DETECT);
			int ret = bc_check_avail();
			solo_ready = (ret == 0);
			if (ret == -EAGAIN && !loops) {
				/* Only warn if it's not ready at startup; don't trigger an error. */
				bc_log(Warning, "Solo6x10 devices are not initialized yet");
			} else if (ret) {
				/* If it's still not ready after 15sec, error */
				bc_status_component_error(
					"Solo6x10 devices are not initialized: %s",
					(ret == -EAGAIN) ?
					"Driver not ready" : strerror(-ret));
			}
			bc_status_component_end(STATUS_SOLO_DETECT, ret == 0);
		}

		/* Every about 2 minutes */
		if ((loops & 127) == 0) {
			bc_watchdog_check_all();

			bc_status_component_begin(STATUS_LICENSE);
			int old_n_devices = max_threads;
			max_threads = 0;
			int ret = (bc_read_licenses(licenses) < 0);

			if (ret == 0) {
				for (std::vector<bc_license>::iterator it = licenses.begin(); it != licenses.end(); ++it)
					max_threads += it->n_devices;

				if (max_threads == 0) {
					int expired = check_trial_expired();
					if (!expired) {
						max_threads = TRIAL_MAX_DEVICES;
						if ((loops & 2047) == 0)
							bc_log(Warning, "Not licensed; running in trial mode");
					}
				} else if (old_n_devices != max_threads) {
					bc_log(Info, "Licensed for %d devices", max_threads);
				}
			}

			bc_status_component_end(STATUS_LICENSE, ret == 0);

			bc_status_component_begin(STATUS_MEDIA_CHECK);
			/* Check media locations for full */
			int mc_ret = bc_check_media();
			bc_status_component_end(STATUS_MEDIA_CHECK, mc_ret == 0);
	 	}

		/* Every 8 seconds */
		if ((loops & 7) == 0) {
			bc_status_component_begin(STATUS_DB_POLLING1);
			/* Check global vars */
			int error = bc_check_globals();
			/* Check for changes in cameras */
			error |= bc_check_db();
			bc_status_component_end(STATUS_DB_POLLING1, error == 0);
		}

		/* Every second, check for dead threads */
		bc_check_threads();

		if (!(loops & 63))
			bc_update_server_status();
	}

	bc_stop_threads();
	bc_db_close();
	bc_libav_init();

	return 0;
}
