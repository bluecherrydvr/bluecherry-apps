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
#include <inttypes.h>
#include <string>
#include <vector>
#include <deque>
#include <cstdatomic>
#include <memory>

#ifndef PRId64
#define PRId64 "lld"
#endif

#define BC_CONFIG		ETCDIR"/bluecherry.conf"
#define BC_CONFIG_BASE		"bluecherry"
#define BC_CONFIG_DB		BC_CONFIG_BASE ".db"

#define BC_UID_TYPE_BC		"BCUID"
#define BC_UID_TYPE_PCI		"BCPCI"

typedef void * BC_DB_RES;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

typedef enum {
	BC_DEVICE_V4L2,
	BC_DEVICE_RTP
} bc_device_type_t;

class stream_packet;
class stream_properties;

class input_device
{
public:
	input_device();
	virtual ~input_device();

	virtual int start() = 0;
	virtual void stop() = 0;
	virtual bool is_started() { return _started; }

	const char *get_error_message() const { return _error_message.c_str(); }

	virtual int read_packet() = 0;
	virtual const stream_packet &packet() const = 0;

	virtual bool has_audio() const = 0;
	bool audio_enabled() const { return _audio_enabled; }
	virtual void set_audio_enabled(bool enabled);

	/* Get the current stream properties; this is a temporary hack for streaming
	 * and should not be relied on. Properties are included as a field with every
	 * packet, and can be reliably updated that way. */
	std::shared_ptr<const stream_properties> properties() const { return current_properties; }

protected:
	bool _audio_enabled, _started;
	std::string _error_message;
	unsigned next_packet_seq;
	std::shared_ptr<stream_properties> current_properties;

	void set_error_message(const std::string &msg) { _error_message = msg; }
};

/* Properties of the media held by stream_packets. This is roughly equivalent
 * to AVCodecContext for the video and audio (if applicable) streams, and is used
 * for recording and decoding.
 *
 * An instance of this class is associated with each stream_packet, which will
 * usually be shared for all packets from the same input. The contents are immutable.
 * If the input properties change, a new instance will be created and sent with all
 * packets from where it began to apply.
 */
class stream_properties
{
public:
	struct video_properties {
		enum CodecID codec_id;
		enum PixelFormat pix_fmt;
		int width;
		int height;
		/* Note that this is codec time_base; you should NOT use it other than
		 * setting on an AVCodecContext. See stream_packet doc. */
		AVRational time_base;
		int profile;
		std::vector<char> extradata;

		video_properties();
		void apply(AVCodecContext *cc) const;
	} video;

	struct audio_properties {
		enum CodecID codec_id;
		int bit_rate;
		int sample_rate;
		enum AVSampleFormat sample_fmt;
		int channels;
		AVRational time_base;
		int profile;
		std::vector<char> extradata;

		audio_properties();
		void apply(AVCodecContext *cc) const;
	} audio;

	bool has_audio() const { return audio.codec_id != CODEC_ID_NONE; }
};

/* Reference counted container for the data in a stream_packet.
 * Expects a pointer allocated with new[], which will be freed
 * along with this instance when the reference count is zero. */
class stream_packet_data
{
public:
	const uint8_t *data;
	std::shared_ptr<stream_properties> properties;

	stream_packet_data(const uint8_t *data, const std::shared_ptr<stream_properties> &properties);

	void ref();
	void deref();

private:
	std::atomic<int> r;

	~stream_packet_data();
};

/* Represents one packet/frame of media data, in an undefined
 * format. stream_packet is created by the input device and passed
 * to consumers, where it can be buffered and handled. The underlying
 * data is constant and shared to reduce copying.
 *
 * The contents of a stream_packet are entirely situation-dependant.
 * It may contain encoded or decoded video or audio in any format, or
 * any other type as needed. Refer to the source of the packet for
 * usage.
 *
 * A stream_packet instance is not threadsafe, but the underlying data
 * is. You may have stream_packet instances referring to the same data
 * existing on separate threads. The data will be atomically deleted
 * only when no instances refer to it.
 */
class stream_packet
{
public:
	enum Flags {
		NoFlags = 0,
		KeyframeFlag = 0x1,
		MotionFlag = 0x2
	};

	unsigned size;
	unsigned flags;
	/* PTS is normalized to AV_TIME_BASE_Q and synchronized across media. */
	int64_t  pts;
	int      type; // AVMEDIA_TYPE_VIDEO or AVMEDIA_TYPE_AUDIO
	time_t   ts_clock; // Used for prerecord event start time
	/* XXX: Monotonic clock time when this packet was captured. This
	 * is used in place of PTS for buffer management (see
	 * stream_keyframe_buffer), but should be deprecated once we have
	 * reliable PTS values to use. */
	time_t   ts_monotonic;
	/* Sequence number of this packet, unique to the input that created
	 * it. Should not be reset unless the handle is stopped and started
	 * again. Rollover is possible and should be handled. */
	unsigned seq;

	stream_packet();
	stream_packet(const uint8_t *data, const std::shared_ptr<stream_properties> &properties);
	stream_packet(const stream_packet &o);
	stream_packet &operator=(const stream_packet &o);
	~stream_packet();

	const uint8_t *data() const { return d ? d->data : 0; }
	std::shared_ptr<const stream_properties> properties() const { return d ? d->properties : std::shared_ptr<stream_properties>(); }

	bool is_key_frame() const { return flags & KeyframeFlag; }
	bool is_video_frame() const { return type == AVMEDIA_TYPE_VIDEO; }

private:
	stream_packet_data *d;
};

/* Simple FIFO-style queue of stream_packet instances. The buffer is
 * unbounded and should be used directly only with great care.
 * Refer to subclasses for implementations with limits on the size of
 * the buffer. */
class stream_buffer : public std::deque<stream_packet>
{
public:
	/* Returns true if the packet was added to the buffer, or false
	 * when accepts_packet was false. Automatically triggers
	 * apply_bound() as well. */
	bool add_packet(const stream_packet &packet);

	/* Implemented in subclasses to decide whether a packet should
	 * be added to the buffer when given to add_packet. Safe to call
	 * from external code. */
	virtual bool accepts_packet(const stream_packet &packet);

protected:
	virtual void apply_bound();

	using std::deque<stream_packet>::insert;
	using std::deque<stream_packet>::push_back;
	using std::deque<stream_packet>::push_front;
};

/* Implementation of stream_buffer which always begins with a video
 * keyframe, and optionally keeps at least enough frames to cover a
 * given duration. Frames are dropped from the beginning when the
 * next keyframe can satisfy the duration requirement.
 */
class stream_keyframe_buffer : public stream_buffer
{
public:
	stream_keyframe_buffer();

	/* Minimum buffer time in seconds, as counted on a monotonic
	 * clock (stream_packet.ts_monotonic). The buffer will be trimmed
	 * when the duration exceeds this value, but will always start
	 * with a keyframe from at least duration seconds ago.
	 *
	 * XXX This should be refactored to use PTS once we have reliable
	 * and consistent PTS values in stream_packet. */
	unsigned duration() const { return mDuration; }
	void set_duration(unsigned duration);

	/* When the buffer is empty, drop all non-video non-keyframe packets
	 * to ensure that the first packet is always a video keyframe. */
	virtual bool accepts_packet(const stream_packet &packet);

	bool enforce_keyframe() const { return mEnforced; }
	void set_enforce_keyframe(bool enforced);

protected:
	unsigned mDuration;
	bool mEnforced;

	virtual void apply_bound();
};

struct bc_handle {
	char			device[512];
	char			driver[512];
	char			mjpeg_url[1024];

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
	char signal_type[16];
	char motion_map[769];
	char schedule[7 * 24 + 1];
	int     width, height;
	int     interval;
	int16_t prerecord, postrecord;
	int8_t  debug_level;
	int     aud_disabled : 1;
	int     schedule_override_global : 1;
	int     hue, saturation, contrast, brightness;
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

/* Standard logging function for all BC services */
void bc_log(const char *msg, ...)
	__attribute__ ((format (printf, 1, 2)));

void bc_vlog(const char *msg, va_list va);

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
int bc_db_open(void);
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
const char *bc_db_get_val(BC_DB_RES dbres, const char *colname, int *length);
int bc_db_get_val_int(BC_DB_RES dbres, const char *colname);
float bc_db_get_val_float(BC_DB_RES dbres, const char *colname);
int bc_db_get_val_bool(BC_DB_RES dbres, const char *colname);
char *bc_db_escape_string(const char *from, int len);

/* Validate and process a license key to get values from it */
int bc_license_machine_id(char *out, int out_sz);
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

int bc_event_has_media(bc_event_cam_t event);
int bc_event_media_length(bc_event_cam_t event);

/* PTZ commands */
void bc_ptz_check(struct bc_handle *bc, BC_DB_RES dbres);
int bc_ptz_cmd(struct bc_handle *bc, unsigned int cmd, int delay, 
	       int pan_speed, int tilt_speed, int pset_id);

#endif /* __LIBBLUECHERRY_H */
