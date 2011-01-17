/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __LIBBLUECHERRY_H
#define __LIBBLUECHERRY_H

#include <sys/types.h>
#include <stdarg.h>
#include <stddef.h>
#include <libconfig.h>

#include <linux/videodev2.h>

#define BC_CONFIG		ETCDIR"/bluecherry.conf"
#define BC_CONFIG_BASE		"bluecherry"
#define BC_CONFIG_DB		BC_CONFIG_BASE ".db"

#define BC_BUFFERS		16
#define BC_BUFFERS_JPEG		2

#define BC_UID_TYPE_BC		"BCUID"
#define BC_UID_TYPE_PCI		"BCPCI"

/* Some things that are driver specific */
#ifndef V4L2_BUF_FLAG_MOTION_ON
#define V4L2_BUF_FLAG_MOTION_ON		0x0400
#define V4L2_BUF_FLAG_MOTION_DETECTED	0x0800
#endif

#ifndef V4L2_CID_MOTION_ENABLE
#define V4L2_CID_MOTION_ENABLE		(V4L2_CID_PRIVATE_BASE+0)
#define V4L2_CID_MOTION_THRESHOLD	(V4L2_CID_PRIVATE_BASE+1)
#define V4L2_CID_MOTION_TRACE		(V4L2_CID_PRIVATE_BASE+2)
#endif

enum bc_vb_status {
	BC_VB_STATUS_LOCAL = 0,
	BC_VB_STATUS_USING,
	BC_VB_STATUS_QUEUED,
};

typedef void * BC_DB_RES;

struct bc_handle {
	/* Track info about the v4l2 device */
	char			dev_file[256];
	int			dev_fd;
	struct v4l2_format	vfmt;
	struct v4l2_capability	vcap;
	struct v4l2_streamparm	vparm;
	/* Userspace buffer accounting */
	struct {
		void			*data;
		size_t			size;
		struct v4l2_buffer	vb;
		enum bc_vb_status	status;
	}			p_buf[BC_BUFFERS];

	int			started;
	int			buf_idx;
	int			got_vop;
	int			mot_cnt;
	int			gop;
	int			buffers;
	int			card_id;
	int			dev_id;

	/* For private data */
	void			*__data;
};

/* Bluecherry License Key */

enum bc_key_type {
	BC_KEY_TYPE_CAMERA = 0,
	BC_KEY_TYPE_CAMERA_EVAL,
};

struct bc_key_data {
	enum bc_key_type	type;
	u_int8_t		major;
	u_int8_t		minor;
	u_int8_t		count;
	u_int8_t		eval_period;
	u_int32_t		id;
};

/* Events and Media */

typedef enum {
	BC_EVENT_L_INFO = 0,
	BC_EVENT_L_WARN,
	BC_EVENT_L_ALRM,
	BC_EVENT_L_CRIT,
} bc_event_level_t;

typedef enum {
	BC_EVENT_CAM_T_MOTION = 0,
	BC_EVENT_CAM_T_NOT_FOUND,
	BC_EVENT_CAM_T_VLOSS,
	BC_EVENT_CAM_T_ALOSS,
} bc_event_cam_type_t;

typedef enum {
	BC_EVENT_SYS_T_DISK = 0,
	BC_EVENT_SYS_T_CRASH,
	BC_EVENT_SYS_T_BOOT,
	BC_EVENT_SYS_T_SHUTDOWN,
	BC_EVENT_SYS_T_REBOOT,
	BC_EVENT_SYS_T_POWER_OUTAGE,
} bc_event_sys_type_t;

typedef struct bc_event_cam * bc_event_cam_t;
#define BC_EVENT_CAM_FAIL ((bc_event_cam_t)NULL)

typedef enum {
	BC_MEDIA_VIDEO_H264 = 0,
	BC_MEDIA_VIDEO_M4V,
	BC_MEDIA_VIDEO_M2V,
} bc_media_video_type_t;

typedef enum {
	BC_MEDIA_AUDIO_NONE = 0,
	BC_MEDIA_AUDIO_MP2,
	BC_MEDIA_AUDIO_MP3,
	BC_MEDIA_AUDIO_WAV,
} bc_media_audio_type_t;

typedef enum {
	BC_MEDIA_CONT_NONE = 0, // Means no audio, and video format raw
	BC_MEDIA_CONT_MKV,
	BC_MEDIA_CONT_MP4,
	BC_MEDIA_CONT_MPEG2_PS,
	BC_MEDIA_CONT_MPEG2_TS,
	BC_MEDIA_CONT_AVI,
} bc_media_cont_type_t;

typedef struct bc_media_entry * bc_media_entry_t;
#define BC_MEDIA_FAIL ((bc_media_entry_t)NULL)

/* Doubly linked lists */

struct bc_list_struct {
	struct bc_list_struct *next, *prev;
};

#define BC_DECLARE_LIST(name) \
	struct bc_list_struct name = { &(name), &(name) }

#define bc_list_add(item, head) do {		\
	(head)->next->prev = (item);		\
	(item)->next = (head)->next;		\
	(item)->prev = (head);			\
	(head)->next = (item);			\
}while(0)

#define bc_list_del(item) do {			\
	(item)->next->prev = (item)->prev;	\
	(item)->prev->next = (item)->next;	\
	(item)->next = NULL;			\
	(item)->prev = NULL;			\
}while(0)

#define bc_list_empty(head) ((head)->next == (head))

#define bc_list_entry(item, type, member) ({ \
	const typeof( ((type *)0)->member ) *__item = (item);    \
	(type *)((char *)__item - offsetof(type,member)); })

#define bc_list_for_each(item, head) \
	for (item = (head)->next; item != (head); item = item->next)

#define bc_list_for_each_entry_safe(pos, n, head, member)			\
        for (pos = bc_list_entry((head)->next, typeof(*pos), member),		\
                n = bc_list_entry(pos->member.next, typeof(*pos), member);	\
             &pos->member != (head);						\
             pos = n, n = bc_list_entry(n->member.next, typeof(*n), member))


/* Called to open and close a handle for a device. */
struct bc_handle *bc_handle_get(const char *dev, int card_id);
void bc_handle_free(struct bc_handle *bc);

/* Called to start and stop the stream */
int bc_handle_start(struct bc_handle *bc);
void bc_handle_stop(struct bc_handle *bc);

/* Standard logging function for all BC services */
void bc_log(const char *msg, ...)
	__attribute__ ((format (printf, 1, 2)));

void bc_vlog(const char *msg, va_list va);

/* Retrieves the next buffer from the device */
int bc_buf_get(struct bc_handle *bc);

/* Get the data pointer for the current buffer */
void *bc_buf_data(struct bc_handle *bc);

/* Get the size in bytes used by the current buffer */
unsigned int bc_buf_size(struct bc_handle *bc);

/* Get the v4l2_buffer struct for the current buffer */
struct v4l2_buffer *bc_buf_v4l2(struct bc_handle *bc);

/* Is the current buffer a key frame? */
int bc_buf_key_frame(struct bc_handle *bc);

/* Format and parameter settings */
int bc_set_interval(struct bc_handle *bc, u_int8_t interval);
int bc_set_format(struct bc_handle *bc, u_int32_t fmt, u_int16_t width,
		  u_int16_t height);

/* Enable or disable the motion detection */
int bc_set_motion(struct bc_handle *bc, int on);
int bc_set_motion_thresh(struct bc_handle *bc, unsigned short val,
			 unsigned short block);
int bc_set_control(struct bc_handle *bc, unsigned int ctrl, int val);

/* Set the text of the OSD */
int bc_set_osd(struct bc_handle *bc, char *fmt, ...)
	__attribute__ ((format (printf, 2, 3)));

/* Database functions */
int bc_db_open(void);
void bc_db_close(void);
BC_DB_RES bc_db_get_table(const char *sql, ...)
	__attribute__ ((format (printf, 1, 2)));
void bc_db_free_table(BC_DB_RES dbres);
int bc_db_fetch_row(BC_DB_RES dbres);
int bc_db_query(const char *sql, ...)
	__attribute__ ((format (printf, 1, 2)));
/* Unlocked version */
int __bc_db_query(const char *sql, ...)
	__attribute__ ((format (printf, 1, 2)));
unsigned long bc_db_last_insert_rowid(void);
int bc_db_start_trans(void);
void bc_db_commit_trans(void);
void bc_db_rollback_trans(void);
const char *bc_db_get_field(BC_DB_RES dbres, int nfield);
int bc_db_num_fields(BC_DB_RES dbres);

/* Used to get specific values from a table result */
const char *bc_db_get_val(BC_DB_RES dbres, const char *colname);
int bc_db_get_val_int(BC_DB_RES dbres, const char *colname);
int bc_db_get_val_bool(BC_DB_RES dbres, const char *colname);
char *bc_db_escape_string(const char *from);

/* Validate and process a license key to get values from it */
int bc_key_process(struct bc_key_data *res, char *str);

/* ### Handle events ### */

/* Returns an event handle for later passing to _end */
bc_event_cam_t bc_event_cam_start(int id, bc_event_level_t level,
				  bc_event_cam_type_t type,
				  bc_media_entry_t media);
/* Finish the event and inserts it into the database */
void bc_event_cam_end(bc_event_cam_t *bce);
/* Insert a cam event in one shot. It will have a 0 length */
int bc_event_cam(int id, bc_event_level_t level,
		 bc_event_cam_type_t type);
/* Insert a system event */
int bc_event_sys(bc_event_level_t level,
		 bc_event_sys_type_t type);

/* ### Handle media entries ### */

/* Returns a media entry handle for later passing to _end */
bc_media_entry_t bc_media_start(int id, bc_media_video_type_t video,
				bc_media_audio_type_t audio,
				bc_media_cont_type_t cont,
				const char *filepath,
				bc_event_cam_t bce);
/* Called at the end of the media to mark end time. If this returns
 * non-zero, you cannot end the media. */
int bc_media_end(bc_media_entry_t *bcm);
/* Get the length of the current media object */
time_t bc_media_length(bc_media_entry_t *bcm);

/* Should be called periodically to ensure events that failed to write
 * to the db are retried. */
void bc_media_event_clear(void);

/* Handlers for motion events */
extern void (*bc_handle_motion_start)(struct bc_handle *bc);
extern void (*bc_handle_motion_end)(struct bc_handle *bc);

#endif /* __LIBBLUECHERRY_H */
