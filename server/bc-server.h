/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __BC_SERVER_H
#define __BC_SERVER_H

#include <pthread.h>

#include <libbluecherry.h>
#include <libavformat/avformat.h>
#include <alsa/asoundlib.h>

#include "g723-dec.h"

/* Maximum length of recording */
#define BC_MAX_RECORD_TIME 900

struct bc_record {
	struct bc_handle	*bc;

	AVOutputFormat		*fmt_out;
	AVStream		*video_st;
	AVStream		*audio_st;
	AVFormatContext		*oc;
	enum CodecID		codec_id;

	time_t			osd_time;
	unsigned int		start_failed;

	struct g723_state	g723_state;
	unsigned char		g723_data[48];
	signed short		pcm_data[384];

	char			outfile[PATH_MAX];
	int			id;
	char			name[256];
	char			dev[256];
	char			driver[256];
	pthread_t		thread;
	struct bc_list_struct	list;

	/* Audio setup */
	snd_pcm_t		*pcm;
	char			aud_dev[256];
	int			aud_rate;
	int			aud_channels;
	unsigned int		aud_format;
	int			aud_disabled;

	/* Event/Media handling */
	bc_media_entry_t	media;
	bc_event_cam_t		event;

	/* Motion, max 22 * 18 */
	char			motion_map[400];

	/* Scheduling, 24x7 */
	char			sched_cur, sched_last;
	char			*thread_should_die;
	pthread_mutex_t		sched_mutex;
	int			file_started;

	/* Notify thread to restart with new format */
	int			reset_vid;
	int			width, height, fmt;
	int			interval;
};

/* Types for aud_format */
#define AUD_FMT_PCM_U8		0x00000001
#define AUD_FMT_PCM_S8		0x00000002
#define AUD_FMT_PCM_U16_LE	0x00000004
#define AUD_FMT_PCM_S16_LE	0x00000008
#define AUD_FMT_PCM_U16_BE	0x00000010
#define AUD_FMT_PCM_S16_BE	0x00000020

/* Flags for aud_format */
#define AUD_FMT_FLAG_G723_24	0x01000000

extern char global_sched[7 * 24];

void bc_get_media_loc(char *stor);

int bc_vid_out(struct bc_record *bc_rec);
int bc_aud_out(struct bc_record *bc_rec);

int bc_av_lockmgr(void **mutex, enum AVLockOp op);
void bc_close_avcodec(struct bc_record *bc_rec);
int bc_open_avcodec(struct bc_record *bc_rec);

struct bc_record *bc_alloc_record(int id, BC_DB_RES dbres);
void bc_update_record(struct bc_record *bc_rec, BC_DB_RES dbres);

typedef enum {
	BC_MOTION_TYPE_SOLO = 0,
} bc_motion_type_t;

int bc_motion_val(bc_motion_type_t type, const char v);

void bc_check_avail(void);

void bc_mkdir_recursive(char *path);

int has_audio(struct bc_record *bc_rec);

#ifdef EBUG
#define bc_debug(fmt, args...) bc_log("D: " fmt, ## args)
#else
#define bc_debug(fmt, args...) do{}while(0)
#endif

#define bc_dev_info(rec, fmt, args...) \
	bc_log("I(%d/%s): " fmt, rec->id, rec->name, ## args)
#define bc_dev_warn(rec, fmt, args...) \
	bc_log("W(%d/%s): " fmt, rec->id, rec->name, ## args)
#define bc_dev_err(rec, fmt, args...) \
	bc_log("E(%d/%s): " fmt, rec->id, rec->name, ## args)

#endif /* __BC_SERVER_H */
