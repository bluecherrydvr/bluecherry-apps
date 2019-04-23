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

#ifndef __LIBBLUECHERRY_H
#define __LIBBLUECHERRY_H

#include <sys/types.h>
#include <stdarg.h>
#include <stddef.h>
#include <termios.h>
#include <libconfig.h>
#include <time.h>
#include <inttypes.h>

extern "C" {
#include <libavcodec/avcodec.h> /* for AVCodecID */
#include <libavformat/avformat.h>
}

#ifndef PRId64
#ifdef __x86_64__
#define PRId64 "ld"
#else
#define PRId64 "lld"
#endif
#endif

#ifndef V4L2_PIX_FMT_H264
#define V4L2_PIX_FMT_H264     v4l2_fourcc('H', '2', '6', '4')
#endif
#ifndef V4L2_PIX_FMT_MPEG4
#define V4L2_PIX_FMT_MPEG4    v4l2_fourcc('M', 'P', 'G', '4')
#endif

#define BC_CONFIG		ETCDIR"/bluecherry.conf"
#define BC_CONFIG_BASE		"bluecherry"
#define BC_CONFIG_DB		BC_CONFIG_BASE ".db"

#define BC_UID_TYPE_BC		"BCUID"
#define BC_UID_TYPE_PCI		"BCPCI"

typedef void * BC_DB_RES;

#include "logging.h"
#include "input_device.h"

typedef enum {
	BC_DEVICE_V4L2_SOLO6010_DKMS,  /* standalone distribution out of sync with kernel upstream */
	BC_DEVICE_V4L2_SOLO6X10,  /* upstream driver */
	BC_DEVICE_V4L2_TW5864,
	BC_DEVICE_V4L2_GENERIC,
	BC_DEVICE_LAVF
} bc_device_type_t;

struct bc_handle {
	char			device[512];
	char			driver[512];

	bc_device_type_t	type;

	input_device            *input;
	class stream_source     *source;

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
	bool rtsp_rtp_prefer_tcp;
	char signal_type[16];
	char motion_map[769];
	int8_t motion_algorithm;
	double motion_frame_downscale_factor;
	int min_motion_area;
	char schedule[7 * 24 + 1];
	int     width, height;
	int     interval;
	int16_t prerecord, postrecord;
	int     motion_analysis_ssw_length;
	int     motion_analysis_percentage;
	int8_t  debug_level;
	bool   aud_disabled;
	bool   schedule_override_global;
	int     hue, saturation, contrast, brightness;
	int     video_quality;
	bool reencode_enabled;
	int reencode_bitrate;
	int reencode_frame_width;
	int reencode_frame_height;
};

struct bc_license {
	char license[32];
	char authorization[32];
	int n_devices;
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

struct bc_media_entry {
	unsigned long table_id;
	char filepath[PATH_MAX];
	unsigned long bytes;
};

struct bc_event_cam {
	int id;
	bc_event_level_t level;
	bc_event_cam_type_t type;
	time_t start_time;
	time_t end_time;
	unsigned long inserted;
	struct bc_media_entry media;
};

struct bc_event_sys {
	bc_event_level_t level;
	bc_event_sys_type_t type;
	time_t time;
};

typedef struct bc_event_cam * bc_event_cam_t;
typedef struct bc_media_entry * bc_media_entry_t;

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

/* Called to open and close a handle for a device. */
struct bc_handle *bc_handle_get(BC_DB_RES dbres);
void bc_handle_free(struct bc_handle *bc);

int bc_device_config_init(struct bc_device_config *cfg, BC_DB_RES dbres);

/* Misc. Utilities */
time_t bc_gettime_monotonic();
int hex_encode(char *out, int out_sz, const char *in, int in_sz);

enum bc_access_type
{
	ACCESS_NONE,
	ACCESS_SETUP,
	ACCESS_REMOTE,
	ACCESS_WEB,
	ACCESS_BACKUP,
	ACCESS_RELAY
};

int bc_user_auth(const char *username, const char *password, int access_type = 0, int device_id = -1);

/* Database functions */
int bc_db_open(const char *bc_config)
	__attribute__((nonnull (1)));
void bc_db_close(void);
BC_DB_RES bc_db_get_table(const char *sql, ...)
	__attribute__ ((format (printf, 1, 2)));
/* Unlocked */
BC_DB_RES __bc_db_get_table(const char *sql, ...)
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
int bc_db_commit_trans(void);
int bc_db_rollback_trans(void);
const char *bc_db_get_field(BC_DB_RES dbres, int nfield);
int bc_db_num_fields(BC_DB_RES dbres);

/* Used to get specific values from a table result */
const char *bc_db_get_val(BC_DB_RES dbres, const char *colname, size_t *length);
int bc_db_get_val_int(BC_DB_RES dbres, const char *colname);
float bc_db_get_val_float(BC_DB_RES dbres, const char *colname);
int bc_db_get_val_bool(BC_DB_RES dbres, const char *colname);
char *bc_db_escape_string(const char *from, size_t len);

/* Validate and process a license key to get values from it */
int bc_license_machine_id(char *out, size_t out_sz);
int bc_read_licenses(std::vector<bc_license> &licenses);
int bc_license_check(const char *license);
int bc_license_check_auth(const char *license, const char *auth);

/* ### Handle events ### */

/* Create a camera event, optionally with a media file associated.
 * Media should be created with bc_event_media_init. */
bc_event_cam_t bc_event_cam_start(int device_id, time_t start_ts,
                                  bc_event_level_t level,
                                  bc_event_cam_type_t type,
                                  const char *media_file);
/* End a camera event returned by bc_event_cam_start */
void bc_event_cam_end(bc_event_cam_t *bce);
/* Insert a system event */
int bc_event_sys(bc_event_level_t level,
		 bc_event_sys_type_t type);

int bc_event_has_media(bc_event_cam_t event) __attribute__((pure));
int bc_event_media_length(bc_event_cam_t event);

/* PTZ commands */
void bc_ptz_check(struct bc_handle *bc, BC_DB_RES dbres);
int bc_ptz_cmd(struct bc_handle *bc, unsigned int cmd, int delay,
	       int pan_speed, int tilt_speed, int pset_id);

#ifdef __cplusplus
template <typename num>
num clamp(num n, num min, num max) {
	return (n < min) ? min : ((n > max) ? max : n);
}
#else
#define clamp(n, min, max) ({					\
	typeof(n) _n = (n);					\
	typeof(min) _min = (min);				\
	typeof(max) _max = (max);				\
	(void)(&_n == &_min);					\
	(void)(&_n == &_max);					\
	(_n < _min) ? _min : ((_n > _max) ? _max : _n); })
#endif

#endif /* __LIBBLUECHERRY_H */
