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

#ifndef __BC_SERVER_H
#define __BC_SERVER_H

#include "libbluecherry.h"
#include <pthread.h>

extern "C" {
#include <libavformat/avformat.h>
#include "g723-dec.h"
}

#include <pugixml.hpp>

#include "reencoder.h"

/* Global Mutexes */
extern pthread_mutex_t mutex_global_sched;
extern pthread_mutex_t mutex_streaming_setup;
extern pthread_mutex_t mutex_snapshot_delay_ms;
extern pthread_mutex_t mutex_max_record_time_sec;

/* Default configuration file */
#define BC_CONFIG_DEFAULT      "/etc/bluecherry.conf"

class bc_record
{
public:
	static bc_record *create_from_db(int id, BC_DB_RES dbres);

	~bc_record();

	struct bc_handle        *bc;
	/* bc_device_config holds configured parameters from the database.
	 * We have two copies of this: cfg, which is almost always what you
	 * want, is writable only by the device's thread and represents the
	 * currently in use configuration.
	 *
	 * cfg_update is accessed only when holding the mutex, as it is written
	 * by the main thread (for updating device config from the database) and
	 * used by the device thread to update the active configuration.
	 */
	struct bc_device_config cfg;
	struct bc_device_config cfg_update;
	char                    cfg_dirty;
	bool cfg_just_updated;
	pthread_mutex_t		    cfg_mutex;

	const int id;

	log_context log;

	/* Streaming */
	/* RTP muxing contexts */
	AVFormatContext *stream_ctx[2];

	class rtsp_stream *rtsp_stream;

	time_t			osd_time;
	unsigned int		start_failed;

	pthread_t		thread;

	/* Event/Media handling */
	bc_event_cam_t          event;

	/* Scheduling, 24x7 */
	char			sched_cur, sched_last;
	const char		*thread_should_die;
	int			file_started;

	/* Notify thread to restart with new format */
	int         reset_vid;

	/* Motion detection & recording */
	class motion_processor *m_processor;
	class trigger_processor *t_processor;
	class motion_handler *m_handler;
	class recorder *rec;

	/* Livestream reencoding */
	class reencoder *reenc;

	int cur_pkt_flags;
	int cur_stream_index;
	bool pkt_first_chunk;

	/* Internal */
	void run();

	bool update_motion_thresholds();

	void getStatusXml(pugi::xml_node &xmlnode);
	void notify_device_state(const char *state);
private:
	bc_record(int id);

	void destroy_elements();
};

typedef enum {
	STATUS_DB_POLLING1,
	STATUS_MEDIA_CHECK,
	STATUS_LICENSE,
	STATUS_HWCARD_DETECT,
	STATUS_WATCHDOG,
	NUM_STATUS_COMPONENTS // must be the last entry
} bc_status_component;

/* Types for aud_format */
#define AUD_FMT_PCM_U8		0x00000001
#define AUD_FMT_PCM_S8		0x00000002
#define AUD_FMT_PCM_U16_LE	0x00000004
#define AUD_FMT_PCM_S16_LE	0x00000008
#define AUD_FMT_PCM_U16_BE	0x00000010
#define AUD_FMT_PCM_S16_BE	0x00000020

/* Flags for aud_format */
#define AUD_FMT_FLAG_G723_24	0x01000000

/* Used to report on the status of critical system operations;
 * Operations should be wrapped in calls to _begin and _end,
 * and any errors recorded with _error. _end accepts an 'ok'
 * flag which, if zero, will cause an error even if no calls to
 * _error happened. An 'ok' status will not override specific
 * errors. These functions may ONLY be used on the main startup
 * thread. */
void bc_status_component_begin(bc_status_component component);
void bc_status_component_error(const char *error, ...);
/* Returns the actual value of 'ok', e.g. 0 if any error occurred */
int bc_status_component_end(bc_status_component component, int ok);

extern char global_sched[7 * 24 + 1];
#define SNAPSHOT_DELAY_MS_DEFAULT 1000
extern int snapshot_delay_ms;
#define MAX_RECORD_TIME_SEC_DEFAULT 900
extern int max_record_time_sec;

/* @return 0 on success, negative if no free storage locations */
int bc_get_media_loc(char *dest, size_t size);

/* Decode one video packet into frame; returns 1 if successful, 0 if no frame, -1 on error.
 * If a frame is returned, the caller MUST free the data with av_free(frame->data[0]); */
int decode_one_video_packet(struct bc_record *bc_rec, const stream_packet &packet, struct AVFrame *frame);
int save_event_snapshot(struct bc_record *bc_rec, const stream_packet &packet);

/* Streaming */
int bc_streaming_setup(struct bc_record *bc_rec);
void bc_streaming_destroy(struct bc_record *bc_rec);
int bc_streaming_is_setup(struct bc_record *bc_rec) __attribute__((pure));
int bc_streaming_is_active(struct bc_record *bc_rec) __attribute__((pure));
int bc_streaming_packet_write(struct bc_record *bc_rec, const stream_packet &packet);

void bc_close_avcodec(struct bc_record *bc_rec);
int bc_open_avcodec(struct bc_record *bc_rec);

int bc_record_update_cfg(struct bc_record *bc_rec, BC_DB_RES dbres);
void stop_handle_properly(struct bc_record *bc_rec);

int bc_check_avail(void);

int bc_mkdir_recursive(char *path);

int has_audio(struct bc_record *bc_rec);

extern "C" {
void signals_setup();

void bc_ffmpeg_init();
void bc_ffmpeg_teardown();
}

#endif /* __BC_SERVER_H */
