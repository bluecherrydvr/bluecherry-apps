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

#define BC_BUFFERS		32
#define BC_BUFFERS_LOCAL	15
#define BC_BUFFERS_THRESH	((BC_BUFFERS - BC_BUFFERS_LOCAL) / 2)

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

enum bc_db_type {
	BC_DB_SQLITE = 0,
	BC_DB_PSQL = 1,
	BC_DB_MYSQL = 2,
};

struct bc_db_ops {
	enum bc_db_type type;
	void *(*open)(struct config_t *cfg);
	void (*close)(void *handle);
	int (*get_table)(void *handle, int *nrows, int *ncols, char ***res,
			 const char *fmt, va_list ap);
	void (*free_table)(void *handle, char **res);
	int (*query)(void *handle, const char *sql, va_list ap);
};

struct bc_db_handle {
	enum bc_db_type		db_type;
	void			*dbh;
	struct bc_db_ops	*db_ops;
};

struct bc_handle {
	/* Track info about the v4l2 device */
	char			dev_file[256];
	int			dev_fd;
	struct v4l2_format	vfmt;
	struct v4l2_capability	vcap;
	struct v4l2_streamparm	vparm;
	/* Userspace buffer accounting */
	struct {
		void		*data;
		size_t		size;
	}			p_buf[BC_BUFFERS];
	struct v4l2_buffer	q_buf[BC_BUFFERS];
	
	int			rd_idx;
	int			wr_idx;
	int			mot_idx;
	int			got_vop;
	int			mot_cnt;
	int			gop;
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

/* Events */

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


/* Called to open and close a handle for a device. */
struct bc_handle *bc_handle_get(const char *dev);
void bc_handle_free(struct bc_handle *bc);

/* Called to start the stream */
int bc_handle_start(struct bc_handle *bc);

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

/* Set the text of the OSD */
int bc_set_osd(struct bc_handle *bc, char *fmt, ...)
	__attribute__ ((format (printf, 2, 3)));

/* Database functions */
struct bc_db_handle *bc_db_open(void);
void bc_db_close(struct bc_db_handle *bc_db);
int bc_db_get_table(struct bc_db_handle *bc_db, int *nrows, int *ncols,
		    char ***res, const char *fmt, ...)
	__attribute__ ((format (printf, 5, 6)));
void bc_db_free_table(struct bc_db_handle *bc_db, char **res);
int bc_db_query(struct bc_db_handle *bc_db, const char *sql, ...)
	__attribute__ ((format (printf, 2, 3)));

/* Used to get specific values from a table result */
char *bc_db_get_val(char **rows, int ncols, int row, const char *colname);
int bc_db_get_val_int(char **rows, int ncols, int row, const char *colname);
int bc_db_get_val_bool(char **rows, int ncols, int row, const char *colname);

/* Validate and process a license key to get values from it */
int bc_key_process(struct bc_key_data *res, char *str);

/* ### Handle events ### */

/* Returns an event handle for later passing to _end */
bc_event_cam_t bc_event_cam_start(int cam_id, bc_event_level_t level,
				  bc_event_cam_type_t type);
/* Finished the event and inserts it into the database */
void bc_event_cam_end(struct bc_db_handle *bcdb, bc_event_cam_t bce);
/* Insert a cam event in one shot. It will have a 0 length */
int bc_event_cam(struct bc_db_handle *bcdb, int cam_id, bc_event_level_t level,
		 bc_event_cam_type_t type);
/* Insert a system event */
int bc_event_sys(struct bc_db_handle *bcdb, bc_event_level_t level,
		 bc_event_sys_type_t type);
/* Should be called periodically to ensure events that failed to write
 * to the db are retried. */
void bc_event_clear(void);

#endif /* __LIBBLUECHERRY_H */
