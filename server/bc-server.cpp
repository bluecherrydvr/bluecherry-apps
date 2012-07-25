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

static BC_DECLARE_LIST(bc_rec_list);

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

extern char *__progname;

/* Prerelease */
#define DONT_ENFORCE_LICENSING 0
#define TRIAL_DAYS 30
#define TRIAL_MAX_DEVICES 32
static std::vector<bc_license> licenses;

/* Fake H.264 encoder for libavcodec. We're only muxing video, never reencoding,
 * so a real encoder isn't neeeded, but one must be present for the process to
 * succeed. ffmpeg does not support h264 encoding without libx264, which is GPL.
 */
static int fake_h264_init(AVCodecContext *ctx)
{
	return 0;
}

static int fake_h264_close(AVCodecContext *ctx)
{
	return 0;
}

static int fake_h264_frame(AVCodecContext *ctx, uint8_t *buf, int bufsize, void *data)
{
	return -1;
}

AVCodec fake_h264_encoder = {
	"fakeh264", /* name */
	AVMEDIA_TYPE_VIDEO, /* type */
	CODEC_ID_H264, /* id */
	0, /* priv_data_size */ 
	fake_h264_init, /* init */
	fake_h264_frame, /* encode */
	fake_h264_close, /* close */
	0, /* decode */
	CODEC_CAP_DELAY, /* capabilities */
	0, /* next */
	0, /* flush */
	0, /* suipported_framerates */
	(const enum PixelFormat[]) { PIX_FMT_YUV420P, PIX_FMT_YUVJ420P, PIX_FMT_NONE }, /* pix_fmts */
	"Fake H.264 Encoder for RTP Muxing", /* long_name */
};

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
	int i;
	int ok = 1;
	char *full_error = NULL;
	int full_error_sz = 0;
	time_t ts;
	BC_DB_RES dbres;

	for (i = 0; i < NUM_STATUS_COMPONENTS; ++i) {
		const char *component_str = component_string((bc_status_component)i);
		if (!component_error[i])
			continue;

		ok = 0;

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

		bc_log("E: [%s] %s", component_str, component_error[i]);
	}

	if (bc_db_start_trans())
		goto error;

	dbres = __bc_db_get_table("SELECT * FROM ServerStatus");
	if (!dbres)
		goto rollback;

	ts = time(NULL);

	if (!bc_db_fetch_row(dbres)) {
		if (!ok)
			ts = bc_db_get_val_int(dbres, "timestamp");
		i = __bc_db_query("UPDATE ServerStatus SET pid=%d, timestamp=%lu, "
		                  "message=%s%s%s", (int)getpid(), ts,
		                  ok ? "" : "'", ok ? "NULL" : full_error, ok ? "" : "'");
	} else {
		i = __bc_db_query("INSERT INTO ServerStatus (pid, timestamp, message) "
		                  "VALUES (%d,%lu,%s%s%s)", (int)getpid(), ts,
		                  ok ? "" : "'", ok ? "NULL" : full_error, ok ? "" : "'");
	}

	bc_db_free_table(dbres);

	if (i)
		goto rollback;

	if (bc_db_commit_trans()) {
	rollback:
		bc_db_rollback_trans();
	error:
		bc_log("E: Unable to update server status");
	}

	free(full_error);
}

/* XXX Create a function here so that we don't have to do so many
 * SELECT's in bc_check_globals() */

/* Update our global settings */
static int bc_check_globals(void)
{
	BC_DB_RES dbres;
	int i = 0;

	/* Get global schedule, default to continuous */
	dbres = bc_db_get_table("SELECT * from GlobalSettings WHERE "
				"parameter='G_DEV_SCED'");

	if (!dbres)
		bc_status_component_error("Database failure for global schedule");

	if (dbres && !bc_db_fetch_row(dbres)) {
		const char *sched = bc_db_get_val(dbres, "value", NULL);
		if (sched)
			strlcpy(global_sched, sched, sizeof(global_sched));
	} else {
		/* Default to continuous record */
		memset(global_sched, 'C', sizeof(global_sched));
		global_sched[sizeof(global_sched)-1] = 0;
	}
	bc_db_free_table(dbres);

	/* Get path to media storage locations, or use default */
	dbres = bc_db_get_table("SELECT * from Storage ORDER BY "
				"priority ASC");

	if (!dbres) {
		bc_status_component_error("Database failure for storage paths");
		return -1;
	}

	if (pthread_mutex_lock(&media_lock) == EDEADLK)
		bc_log("E: Deadlock detected in media_lock on db_check!");

	memset(media_stor, 0, sizeof(media_stor));

	while (!bc_db_fetch_row(dbres) && i < MAX_STOR_LOCS) {
		const char *path = bc_db_get_val(dbres, "path", NULL);
		float max_thresh = bc_db_get_val_float(dbres, "max_thresh");
		float min_thresh = bc_db_get_val_float(dbres, "min_thresh");

		if (!path || !*path || max_thresh <= 0 || min_thresh <= 0)
			continue;

		strcpy(media_stor[i].path, path);
		media_stor[i].max_thresh = max_thresh;
		media_stor[i].min_thresh = min_thresh;

		/* We should probably properly stat the directory */
		if (bc_mkdir_recursive(media_stor[i].path)) {
			bc_status_component_error("Cannot create storage path %s: %m",
			                          media_stor[i].path);
		} else
			i++;
	}

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

static float storage_used(const struct bc_storage *stor)
{
	struct statvfs st;

	if (statvfs(stor->path, &st))
		return -1.00;

	return 100.0 - ((float)((float)st.f_bavail / (float)st.f_blocks) * 100);
}

/* Returns -1 on error, 0 for non-full, 1 for full */
static int storage_full(const struct bc_storage *stor)
{
	float used = storage_used(stor);
	if (used < 0)
		return -1;
	else
		return (used >= stor->max_thresh);
}

void bc_get_media_loc(char *stor)
{
	int i;

	stor[0] = 0;

	if (pthread_mutex_lock(&media_lock) == EDEADLK)
		bc_log("E: Deadlock detected in media_lock on get_loc!");

	for (i = 0; i < MAX_STOR_LOCS && media_stor[i].max_thresh; i++) {
		if (!storage_full(&media_stor[i])) {
			strcpy(stor, media_stor[i].path);
			break;
		}
	}

	/* XXX this is a bad fallback. We might not even want a fallback. */
	if (stor[0] == 0)
		strcpy(stor, media_stor[0].path);

	pthread_mutex_unlock(&media_lock);
}

static int bc_cleanup_media()
{
	BC_DB_RES dbres;
	int removed = 0, error_count = 0;
	int i;

	dbres = bc_db_get_table("SELECT * from Media WHERE archive=0 AND filepath!='' ORDER BY start ASC");

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
			bc_log("W: Cannot remove file %s for cleanup: %s",
			       filepath, strerror(errno));
			error_count++;
			continue;
		}

		std::string sidecar = filepath;
		if (sidecar.size() > 3)
			sidecar.replace(sidecar.size()-3, 3, "jpg");
		if (unlink(sidecar.c_str()) < 0 && errno != ENOENT) {
			bc_log("W: Cannot remove sidecar file %s for cleanup: %s",
			       sidecar.c_str(), strerror(errno));
			error_count++;
			continue;
		}

		removed++;
		if (__bc_db_query("UPDATE Media SET filepath='',size=0 "
		                  "WHERE id=%d", id)) {
			bc_status_component_error("Database error during media cleanup");
		}

		if (!(removed % 5)) {
			for (i = 0; i < MAX_STOR_LOCS && media_stor[i].min_thresh; i++) {
				float used = storage_used(&media_stor[i]);
				if (used >= 0 && used <= media_stor[i].min_thresh)
					goto done;
			}
		}
        }

done:
	bc_db_free_table(dbres);

	if (error_count)
		bc_log("W: Cleaned up %d files with %d errors", removed, error_count);
	else
		bc_log("I: Cleaned up %d files", removed);

	return 0;
}

static int bc_check_media(void)
{
	int i, free = 0;
	int ret = 0;

	if (pthread_mutex_lock(&media_lock) == EDEADLK)
		bc_log("E: Deadlock detected in media_lock on check_media!");

	for (i = 0; i < MAX_STOR_LOCS && media_stor[i].max_thresh; i++) {
		if (!storage_full(&media_stor[i])) {
			free = 1;
			break;
		}
	}

	if (!free)
		ret = bc_cleanup_media();

	pthread_mutex_unlock(&media_lock);
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

		/* Maximum threads, used for licensing control */
		if (!DONT_ENFORCE_LICENSING && max_threads >= 0 && cur_threads >= max_threads) {
			bc_status_component_error("Device disabled due to licensing restrictions");
			re |= -1;
			continue;
		}

		/* If this is a V4L2 device, it needs to be detected */
		if (!strcasecmp(proto, "V4L2")) {
			int card_id = bc_db_get_val_int(dbres, "card_id");
			if (!solo_ready || card_id < 0)
				continue;
		}

		bc_rec = bc_alloc_record(id, dbres);
		if (!bc_rec) {
			re |= -1;
			continue;
		}

		cur_threads++;
		bc_list_add(&bc_rec->list, &bc_rec_list);
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
			bc_log("Event %d left in-progress at shutdown; setting duration", event_id);
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

			bc_log("Media %s has zero time so deleting",
			       filepath);

			bc_db_query("DELETE FROM EventsCam WHERE id=%u", e_id);
			bc_db_query("DELETE FROM Media WHERE id=%u", m_id);

			unlink(filepath);
		} else {
			unsigned int id = bc_db_get_val_int(dbres, "id");

			bc_log("Media %s left in-progress so updating length "
			       "to %d", filepath, duration);
			bc_db_query("UPDATE EventsCam SET length=%d WHERE "
				    "id=%d", duration, id);
		}

		pclose(fp);
        }

	bc_db_free_table(dbres);
}

static void usage(void)
{
	fprintf(stderr, "Usage: %s [-s]\n", __progname);
	fprintf(stderr, "  -s\tDo not background\n");
	fprintf(stderr, "  -u\tDrop privileges to user\n");
	fprintf(stderr, "  -g\tDrop privileges to group\n");
	fprintf(stderr, "  -r\tRecord a specific ID only\n");

	exit(1);
}

static pthread_key_t av_log_current_handle_key;
static const int av_log_without_handle = AV_LOG_INFO;

void bc_av_log_set_handle_thread(struct bc_record *bc_rec)
{
	pthread_setspecific(av_log_current_handle_key, bc_rec);
}

/* Warning: Must be reentrant; this may be called from many device threads at once */
static void av_log_cb(void *avcl, int level, const char *fmt, va_list ap)
{
	char msg[strlen(fmt) + 200];
	const char *levelstr;
	struct bc_record *bc_rec = (struct bc_record*)pthread_getspecific(av_log_current_handle_key);

	switch (level) {
		case AV_LOG_PANIC: levelstr = "PANIC"; break;
		case AV_LOG_FATAL: levelstr = "fatal"; break;
		case AV_LOG_ERROR: levelstr = "error"; break;
		case AV_LOG_WARNING: levelstr = "warning"; break;
		case AV_LOG_INFO: levelstr = "info"; break;
		case AV_LOG_VERBOSE: levelstr = "verbose"; break;
		case AV_LOG_DEBUG: levelstr = "debug"; break;
		default: levelstr = "???"; break;
	}

	if (!bc_rec) {
		if (level <= av_log_without_handle) {
			sprintf(msg, "[avlib %s]: %s", levelstr, fmt);
			bc_vlog(msg, ap);
		}
		return;
	}

	if ((bc_rec->cfg.debug_level < 0 && level > AV_LOG_FATAL) ||
	    (bc_rec->cfg.debug_level == 0 && level > AV_LOG_ERROR) ||
	    (bc_rec->cfg.debug_level == 1 && level > AV_LOG_INFO))
		return;

	sprintf(msg, "I(%d/%s): avlib %s: %s", bc_rec->id, bc_rec->cfg.name, levelstr, fmt);
	bc_vlog(msg, ap);
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
		"buy a license from http://www.bluecherrydvr.com/", TRIAL_DAYS);
	return -1;
}

int main(int argc, char **argv)
{
	int opt;
	unsigned int loops;
	int bg = 1;
	int count;
	const char *user = 0, *group = 0;
	int error;

	while ((opt = getopt(argc, argv, "hsm:r:u:g:")) != -1) {
		switch (opt) {
		case 's': bg = 0; break;
		case 'r': record_id = atoi(optarg); break;
		case 'u': user = optarg; break;
		case 'g': group = optarg; break;
		case 'h': default: usage();
		}
	}

	if (user || group) {
		struct passwd *u   = 0;
		gid_t          gid = 0;

		if (group) {
			struct group *g = 0;
			if (!(g = getgrnam(group))) {
				bc_log("E: Group '%s' does not exist", group);
				exit(1);
			}
			gid = g->gr_gid;
		}

		if (user) {
			if (!(u = getpwnam(user))) {
				bc_log("E: User '%s' does not exist", user);
				exit(1);
			}
			if (!group)
				gid = u->pw_gid;
			if (initgroups(user, gid) < 0) {
				bc_log("E: Setting supplemental groups failed");
				exit(1);
			}
		}

		if (setregid(gid, gid) < 0) {
			bc_log("E: Setting group failed");
			exit(1);
		}

		if (u && setreuid(u->pw_uid, u->pw_uid) < 0) {
			bc_log("E: Setting user failed");
			exit(1);
		}
	}	

	if (av_lockmgr_register(bc_av_lockmgr)) {
		bc_log("E: AV lock registration failed: %m");
		exit(1);
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	/* XXX This is not suitable for much of anything, really. */
	srand((unsigned)(getpid() * time(NULL)));

	avcodec_register(&fake_h264_encoder);
	av_register_all();
	avformat_network_init();

	pthread_key_create(&av_log_current_handle_key, NULL);
	av_log_set_callback(av_log_cb);

	if (bg && daemon(0, 0) == -1) {
		bc_log("E: Could not fork to background: %m");
		exit(1);
	}

	bc_log("I: Started Bluecherry daemon");

	rtsp_server *rtsp = new rtsp_server;
	rtsp->setup(7002);

	for (count = 1; bc_db_open(); count++) {
		sleep(1);
		if (count % 30)
			continue;
		bc_log("E: Could not open SQL database after 30 seconds...");
	}

	bc_log("I: SQL database connection opened");

	bc_status_component_begin(STATUS_DB_POLLING1);
	error = bc_check_globals();
	/* Do some cleanup */
	get_last_run_date();
	bc_check_inprogress();
	bc_status_component_end(STATUS_DB_POLLING1, error == 0);

	pthread_t rtsp_thread;
	pthread_create(&rtsp_thread, NULL, rtsp_server::runThread, rtsp);

	/* Main loop */
	for (loops = 0 ;; loops++) {
		/* Every 15 seconds until initialized, then every 5 minutes */
		if ((!solo_ready && !(loops % 15)) || (solo_ready && !(loops % 300))) {
			bc_status_component_begin(STATUS_SOLO_DETECT);
			error = bc_check_avail();
			solo_ready = (error == 0);
			if (error == -EAGAIN && !loops) {
				/* Only warn if it's not ready at startup; don't trigger an error. */
				bc_log("W: Solo6x10 devices are not initialized yet");
				error = 0;
			} else if (error) {
				/* If it's still not ready after 15sec, error */
				bc_status_component_error("Solo6x10 devices are not initialized: %s",
				                          (error == -EAGAIN) ? "Driver not ready" :
				                                               strerror(-error));
			}
			bc_status_component_end(STATUS_SOLO_DETECT, error == 0);
		}

		/* Every 2 minutes */
		if (!(loops % 120)) {
			bc_status_component_begin(STATUS_LICENSE);
			int old_n_devices = max_threads;
			max_threads = 0;
			error = (bc_read_licenses(licenses) < 0);

			if (!error) {
				for (std::vector<bc_license>::iterator it = licenses.begin(); it != licenses.end(); ++it)
					max_threads += it->n_devices;
				if (old_n_devices != max_threads)
					bc_log("I: Licensed for %d devices", max_threads);
			}

			if (!error && max_threads == 0) {
				error |= check_trial_expired();
				if (!error) {
					max_threads = TRIAL_MAX_DEVICES;
					if (!(loops % 1800))
						bc_log("I: Not licensed; running in trial mode");
				}
			}

			bc_status_component_end(STATUS_LICENSE, error == 0);

			bc_status_component_begin(STATUS_MEDIA_CHECK);
			/* Check media locations for full */
			error = bc_check_media();
			bc_status_component_end(STATUS_MEDIA_CHECK, error == 0);
	 	}

		/* Every 10 seconds */
		if (!(loops % 10)) {
			bc_status_component_begin(STATUS_DB_POLLING1);
			/* Check global vars */
			error = bc_check_globals();
			/* Check for changes in cameras */
			error |= bc_check_db();
			bc_status_component_end(STATUS_DB_POLLING1, error == 0);
		}

		/* Every second, check for dead threads */
		bc_check_threads();

		if (!(loops % 60))
			bc_update_server_status();

		sleep(1);
	}

	bc_stop_threads();
	bc_db_close();
	av_lockmgr_register(NULL);

	exit(0);
}
