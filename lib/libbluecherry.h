/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __LIBBLUECHERRY_H
#define __LIBBLUECHERRY_H

#include <sys/types.h>
#include <stdarg.h>

#include <linux/videodev2.h>

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
	BC_DB_SQLITE,
};

struct bc_db_ops {
	enum bc_db_type type;
	void *(*open)(void);
	void (*close)(void *handle);
	int (*get_table)(void *handle, int *nrows, int *ncols, char ***res,
			 const char *fmt, va_list ap);
	void (*free_table)(void *handle, char **result);
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
	int			got_vop;
	int			mot_cnt;
	int			gop;
	/* Database tracking and handlers */
	enum bc_db_type		db_type;
	void			*dbh;
	struct bc_db_ops	*db_ops;
};

/* Called to open and close a handle for a device. */
struct bc_handle *bc_handle_get(const char *dev);
void bc_handle_free(struct bc_handle *bc);

/* Called to start the stream */
int bc_handle_start(struct bc_handle *bc);

/* Standard logging function for all BC services */
void bc_log(char *msg, ...);

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
int bc_set_osd(struct bc_handle *bc, char *str);

/* Database functions */
int bc_db_open(struct bc_handle *bc, enum bc_db_type type);
void bc_db_close(struct bc_handle *bc);

#endif /* __LIBBLUECHERRY_H */
