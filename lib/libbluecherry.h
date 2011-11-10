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
#include <termios.h>
#include <libconfig.h>
#include <time.h>

#include <linux/videodev2.h>

#include <rtp-session.h>

#define BC_CONFIG		ETCDIR"/bluecherry.conf"
#define BC_CONFIG_BASE		"bluecherry"
#define BC_CONFIG_DB		BC_CONFIG_BASE ".db"

#define BC_BUFFERS		16
#define BC_BUFFERS_JPEG		8

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

typedef void * BC_DB_RES;

/* Camera capability flags */
#define BC_CAM_CAP_V4L2_MOTION	0x00000002
#define BC_CAM_CAP_OSD		0x00000004
#define BC_CAM_CAP_SOLO		0x00000008
#define BC_CAM_CAP_MJPEG_URL	0x00000010

typedef enum {
	BC_DEVICE_V4L2,
	BC_DEVICE_RTP
} bc_device_type_t;

struct v4l2_device {
	int			dev_fd;
	struct v4l2_format	vfmt;
	struct v4l2_capability	vcap;
	struct v4l2_streamparm	vparm;
	/* Userspace buffer accounting */
	struct {
		void			*data;
		size_t			size;
		struct v4l2_buffer	vb;
	}			p_buf[BC_BUFFERS];
	int			local_bufs;
	int			buf_idx;
	int			gop;
	int			buffers;
	int			card_id;
	int			dev_id;
};

struct bc_handle {
	char			device[512];
	char			driver[512];
	char			mjpeg_url[1024];

	bc_device_type_t	type;
	unsigned int		cam_caps;

	struct v4l2_device	v4l2;
	struct rtp_device	rtp;

	int			started;
	int			got_vop;
	time_t			mot_last_ts;

	/* PTZ params. Path is a device for PELCO types and full URI
	 * for IP based PTZ controls. */
	char			ptz_path[1024];
	unsigned char		ptz_addr;
	unsigned int		ptz_proto;
	/* Used for serial based protcols */
	struct termios		ptz_tio;

	/* For private data */
	void			*__data;
};

/* Parameters for a device from the database; use through
 * bc_record */
struct bc_device_config {
	char name[256];
	char dev[256];
	char driver[256];
	char rtsp_username[64];
	char rtsp_password[64];
	char signal_type[16];
	char motion_map[400];
	char schedule[7 * 24];
	int     width, height;
	int     interval;
	int8_t  debug_level;
	int     aud_disabled : 1;
	int     schedule_override_global : 1;
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
	BC_EVENT_CAM_T_CONTINUOUS,
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
#define BC_EVENT_CAM_NULL ((bc_event_cam_t)NULL)

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
#define BC_MEDIA_NULL ((bc_media_entry_t)NULL)

/* These bits are setup in such a way as to avoid conflicting bits being
 * used together. Each nibble should only have one bit set. In addition,
 * You can't use STOP with anything else. */
#define BC_PTZ_CMD_STOP		0x80000000

/* Movement commands */
#define BC_PTZ_CMD_RIGHT	0x00000001
#define BC_PTZ_CMD_LEFT		0x00000002
#define BC_PTZ_CMD_UP		0x00000010
#define BC_PTZ_CMD_DOWN		0x00000020
#define BC_PTZ_CMD_IN		0x00000100
#define BC_PTZ_CMD_OUT		0x00000200
#define BC_PTZ_CMDS_MOVE_MASK	0x00000333

/* Preset commands */
#define BC_PTZ_CMD_SAVE		0x00001000
#define BC_PTZ_CMD_GO		0x00002000
#define BC_PTZ_CMD_CLEAR	0x00004000
#define BC_PTZ_CMDS_PSET_MASK	0x00007000


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
struct bc_handle *bc_handle_get(BC_DB_RES dbres);
void bc_handle_free(struct bc_handle *bc);

/* Called to start and stop the stream */
int bc_handle_start(struct bc_handle *bc, const char **err_msg);
void bc_handle_stop(struct bc_handle *bc);
/* Reset the handle to start a new recording.
 * For solo devices, this is equivalent to stop, but
 * for RTP, it has no effect. */
void bc_handle_reset(struct bc_handle *bc);

int bc_device_config_init(struct bc_device_config *cfg, BC_DB_RES dbres);

/* Standard logging function for all BC services */
void bc_log(const char *msg, ...)
	__attribute__ ((format (printf, 1, 2)));

void bc_vlog(const char *msg, va_list va);

/* Misc. Utilities */
time_t bc_gettime_monotonic();

/* Retrieves the next buffer from the device.
 * For RTSP devices, this buffer may not be video! */
int bc_buf_get(struct bc_handle *bc);

/* Get the data pointer for the current buffer */
void *bc_buf_data(struct bc_handle *bc);

/* Get the size in bytes used by the current buffer */
unsigned int bc_buf_size(struct bc_handle *bc);

/* Is the current buffer a key frame? */
int bc_buf_key_frame(struct bc_handle *bc);

/* Is the current buffer a video frame? */
int bc_buf_is_video_frame(struct bc_handle *bc);

/* Format and parameter settings */
int bc_set_interval(struct bc_handle *bc, u_int8_t interval);
int bc_set_format(struct bc_handle *bc, u_int32_t fmt, u_int16_t width,
		  u_int16_t height);
int bc_set_mjpeg(struct bc_handle *bc);

/* Enable or disable the motion detection */
int bc_set_motion(struct bc_handle *bc, int on);
int bc_set_motion_thresh(struct bc_handle *bc, unsigned short val,
			 unsigned short block);
int bc_set_motion_thresh_global(struct bc_handle *bc, unsigned short val);
/* Checks if the current buffer has motion on/detected */
int bc_motion_is_on(struct bc_handle *bc);
int bc_motion_is_detected(struct bc_handle *bc);

/* Set v4l2 control */
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
const char *bc_db_get_val(BC_DB_RES dbres, const char *colname, int *length);
int bc_db_get_val_int(BC_DB_RES dbres, const char *colname);
float bc_db_get_val_float(BC_DB_RES dbres, const char *colname);
int bc_db_get_val_bool(BC_DB_RES dbres, const char *colname);
char *bc_db_escape_string(const char *from, int len);

/* Validate and process a license key to get values from it */
int bc_key_process(struct bc_key_data *res, char *str);

/* ### Handle events ### */

/* Returns an event handle for later passing to _end */
bc_event_cam_t bc_event_cam_start(int id, bc_event_level_t level,
				  bc_event_cam_type_t type,
				  bc_media_entry_t media);
int bc_event_cam_fire(int id, bc_event_level_t level, bc_event_cam_type_t type);
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
/* Destroy a media object (that not got started) */
void bc_media_destroy(bc_media_entry_t *bcm);
void bc_media_set_snapshot(bc_event_cam_t bce, void *file_data,
			   int file_size);

/* Should be called periodically to ensure events that failed to write
 * to the db are retried. */
void bc_media_event_clear(void);

/* Handlers for motion events */
extern void (*bc_handle_motion_start)(struct bc_handle *bc);
extern void (*bc_handle_motion_end)(struct bc_handle *bc);

/* PTZ commands */
void bc_ptz_check(struct bc_handle *bc, BC_DB_RES dbres);
int bc_ptz_cmd(struct bc_handle *bc, unsigned int cmd, int delay, 
	       int pan_speed, int tilt_speed, int pset_id);

#endif /* __LIBBLUECHERRY_H */
