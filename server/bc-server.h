/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __BC_SERVER_H
#define __BC_SERVER_H

#include <libbluecherry.h>
#include <pthread.h>

extern "C" {
#include <libavformat/avformat.h>
#include "g723-dec.h"
}

#include <alsa/asoundlib.h>

/* Maximum length of recording */
#define BC_MAX_RECORD_TIME 900

struct bc_record {
	struct bc_handle        *bc;
	/* bc_device_config holds configured parameters from the database.
	 * We have two copies of this: cfg, which is almost always what you
	 * want, is writable only by the device's thread and represents the
	 * currently in use configuration.
	 *
	 * cfg_update is accessed only when holding the mutex, as it is written
	 * by the main thread (for updating device config from the database) and
	 * used by the device thread to update the active configuration.
	 *
	 * The cfg_dirty flag may be read without acquiring the mutex first, but
	 * must always be written while holding the mutex. */
	struct bc_device_config cfg;
	struct bc_device_config cfg_update;
	char                    cfg_dirty;
	pthread_mutex_t		    cfg_mutex;

	int id;

	/* Recording */
	AVOutputFormat  *fmt_out;
	AVStream        *video_st;
	AVStream        *audio_st;
	AVFormatContext *oc;
	int64_t         output_pts_base;

	/* Streaming */
	AVFormatContext *stream_ctx;

	time_t			osd_time;
	unsigned int		start_failed;

	struct g723_state	g723_state;
	unsigned char		g723_data[48];
	signed short		pcm_data[384];

	char			outfile[PATH_MAX];
	pthread_t		thread;
	struct bc_list_struct	list;

	/* Audio setup */
	snd_pcm_t		*pcm;
	char			aud_dev[256];
	int			aud_rate;
	int			aud_channels;
	unsigned int		aud_format;

	/* Event/Media handling */
	bc_event_cam_t		event;

	/* Scheduling, 24x7 */
	char			sched_cur, sched_last;
	char			*thread_should_die;
	int			file_started;

	/* Notify thread to restart with new format */
	int         reset_vid;
	int         fmt;

	/* Motion detection */
	time_t mot_last_ts;
	struct bc_output_packet *prerecord_head, *prerecord_tail;
	/* Pointer to the first packet not written after the postrecord time in
	 * the current motion recording. This may not be a valid pointer; the ONLY
	 * correct usage is to check if this pointer is still in the prerecord_head
	 * list. Be aware that if any new packet is added between a removal from the
	 * list and a check against this pointer, a false positive is possible. That
	 * is avoided in the current usage. */
	void *mot_first_buffered_packet;
};

struct bc_output_packet {
	struct bc_output_packet *next;
	void        *data;
	int          size;
	unsigned int flags;
	int64_t      pts;
	int          type; // AVMEDIA_TYPE_VIDEO or AVMEDIA_TYPE_AUDIO
	time_t       ts_monotonic; // XXX used for prerecord; refactor with PTS
	time_t       ts_clock; // Used for prerecord event start time
};

typedef enum {
	STATUS_DB_POLLING1,
	STATUS_MEDIA_CHECK,
	STATUS_LICENSE,
	STATUS_SOLO_DETECT,
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

void bc_get_media_loc(char *stor);

int get_output_audio_packet(struct bc_record *bc_rec, struct bc_output_packet *pkt);
int get_output_video_packet(struct bc_record *bc_rec, struct bc_output_packet *pkt);
int bc_output_packet_write(struct bc_record *bc_rec, struct bc_output_packet *pkt);
int bc_output_packet_copy(struct bc_output_packet *dst, const struct bc_output_packet *src);

/* Streaming */
int bc_streaming_setup(struct bc_record *bc_rec);
void bc_streaming_destroy(struct bc_record *bc_rec);
int bc_streaming_is_active(struct bc_record *bc_rec);
int bc_streaming_packet_write(struct bc_record *bc_rec, struct bc_output_packet *pkt);

/* Relate all libav logging on this thread to a given bc_record */
void bc_av_log_set_handle_thread(struct bc_record *bc_rec);

int bc_av_lockmgr(void **mutex, enum AVLockOp op);
void bc_close_avcodec(struct bc_record *bc_rec);
int bc_open_avcodec(struct bc_record *bc_rec);

struct bc_record *bc_alloc_record(int id, BC_DB_RES dbres);
int bc_record_update_cfg(struct bc_record *bc_rec, BC_DB_RES dbres);

int bc_check_avail(void);

int bc_mkdir_recursive(char *path);

int has_audio(struct bc_record *bc_rec);

#ifdef EBUG
#define bc_debug(fmt, args...) bc_log("D: " fmt, ## args)
#else
#define bc_debug(fmt, args...) do{}while(0)
#endif

#define bc_dev_info(rec, fmt, args...) \
	bc_log("I(%d/%s): " fmt, rec->id, rec->cfg.name, ## args)
#define bc_dev_warn(rec, fmt, args...) \
	bc_log("W(%d/%s): " fmt, rec->id, rec->cfg.name, ## args)
#define bc_dev_err(rec, fmt, args...) \
	bc_log("E(%d/%s): " fmt, rec->id, rec->cfg.name, ## args)

#endif /* __BC_SERVER_H */
