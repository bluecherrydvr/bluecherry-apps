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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include <bsd/string.h>
#include <curl/curl.h>

#include "libbluecherry.h"
#include "lavf_device.h"
#include "v4l2_device_solo6x10.h"
#include "v4l2_device_solo6010-dkms.h"
#include "v4l2_device_tw5864.h"
#include "stream_elements.h"

#define reset_vbuf(__vb) do {				\
	memset((__vb), 0, sizeof(*(__vb)));		\
	(__vb)->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	\
	(__vb)->memory = V4L2_MEMORY_MMAP;		\
} while(0)

#define RETERR(__msg) ({		\
	if (err_msg)			\
		*err_msg = __msg;	\
	return -1;			\
})

int get_solo_driver_name(char *buf, size_t bufsize)
{
	FILE *fd = popen("lsmod | grep ^solo6 | cut -d ' ' -f1 | tr -d '\\n'", "r");
	if (!fd)
		return -1;
	fgets(buf, bufsize, fd);
	pclose(fd);
	return 0;
}

static inline char *split(char *s, char delim)
{
	s = strchr(s, delim);
	if (s && (*s = 0, !*++s))
		s = NULL;
	return s;
}

static inline void split_pp(char *s, const char **p1, const char **p2)
{
	char *tmp = split(s, '|');
	*p1 = tmp;
	*p2 = split(tmp, '|');
}

static int get_creds(BC_DB_RES dbres, char *creds, size_t size)
{
	const char *user, *pass;

	user = bc_db_get_val(dbres, "rtsp_username", NULL);
	pass = bc_db_get_val(dbres, "rtsp_password", NULL);

	if (!user || !pass)
		return -1;

	if (*user && *pass) {
		char *urlencoded_user = curl_easy_escape(NULL, user, 0);
		if (!urlencoded_user) {
			return -1;
		}

		char *urlencoded_pass = curl_easy_escape(NULL, pass, 0);
		if (!urlencoded_pass) {
			curl_free(urlencoded_user);
			return -1;
		}
		size_t s = snprintf(creds, size, "%s:%s@", urlencoded_user, urlencoded_pass);
		curl_free(urlencoded_user);
		curl_free(urlencoded_pass);
		if (s >= size)
			return -1;
	} else {
		*creds = '\0';
	}

	return 0;
}

/**
 * Parse device path.
 * It is in the questionable format of 'hostname|port|path'.
 *
 * @returns 0 on success, -1 on error.
 */
static int parse_dev_path(char *dst, size_t sz, const char *val,
			  const char *creds, const char *defhost,
			  const char *defport, const char *proto)
{
	char *spec = strdupa(val);
	const char *port, *path;
	unsigned int r;

	split_pp(spec, &port, &path);

	/* XXX: Workaround. Sometimes hostname isn't present.
	 * Should be removed at some point. */
	const char *host = *spec ? spec : defhost;

	if (!path) {
		/* Broken spec. */
		return -1;
	}

	if (!port)
		port = defport;

	/* Create the URL */
	r = snprintf(dst, sz, "%s://%s%s:%s%s", proto, creds, host, port, path);

	if (r >= sz)
		return -1;

	return 0;
}

static int lavf_handle_init(struct bc_handle *bc, BC_DB_RES dbres)
{
	const char *val;
	bc->type = BC_DEVICE_LAVF;

	/* FIXME bc_device_config is already parsed from DB and saved in void*
	 * __data, but completely unused and unreachable (bc_record type belongs to
	 * server and not to lib) */

	char creds[4096];
	if (get_creds(dbres, creds, sizeof(creds)) < 0)
		return -1;

	int rtsp_rtp_prefer_tcp = 0;
	const char *protocol = bc_db_get_val(dbres, "protocol", NULL);
	const char *uri_schema;
	if (protocol && !strncmp(protocol, "IP-MJPEG", 8)) {
		uri_schema = "http";
		val = bc_db_get_val(dbres, "mjpeg_path", NULL);
	} else {
		uri_schema = "rtsp";

		const char *rtsp_rtp_prefer_tcp_raw = bc_db_get_val(dbres, "rtsp_rtp_prefer_tcp", NULL);
		if (rtsp_rtp_prefer_tcp_raw && strlen(rtsp_rtp_prefer_tcp_raw)) {
			rtsp_rtp_prefer_tcp = atoi(rtsp_rtp_prefer_tcp_raw);
		} else {
			bc_log(Error, "rtsp_rtp_prefer_tcp field is absent in Devices table, "
					"DB schema not updated. Using default field value, \"AUTO\"");
		}

		val = bc_db_get_val(dbres, "device", NULL);
	}

	if (val && *val) {
		char url[1024];

		int r = parse_dev_path(url, sizeof(url), val, creds,
				"XXX", /* host should be always present, this default doesn't matter */
				"554", /* port should be always present, this default doesn't matter */
				uri_schema);
		if (r)
			return -1;

		strlcpy(bc->device, url, sizeof(bc->device));
		bc->input = new lavf_device(url, rtsp_rtp_prefer_tcp);

		if (bc->substream_mode) {
			val = bc_db_get_val(dbres, "substream_path", NULL);

			if (val && *val) {
				/* substream_path format should be same as device, 'hostname|port|path' */
				r = parse_dev_path(url, sizeof(url), val, creds,
									"XXX", /* host should be always present, this default doesn't matter */
									"554", /* port should be always present, this default doesn't matter */
									uri_schema);
				if (r)
					return -1;

				bc->substream_input = new lavf_device(url, rtsp_rtp_prefer_tcp);
			}
		}
	}

	/* Check it again because substream_path may damage this variable */
	if (val && *val) {
		/* This `defhost` is a workaround for empty host part in mjpeg_path */
		char *defhost = strdupa(val);
		{
			char *p = strchr(defhost, '|');
			if (p)
				*p = 0;
		}
	}

	return 0;
}

struct bc_handle *bc_handle_get(BC_DB_RES dbres)
{
	const char *device, *driver;
	const char *protocol;
	struct bc_handle *bc;
	int ret;

	if (dbres == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	device = bc_db_get_val(dbres, "device", NULL);
	driver = bc_db_get_val(dbres, "driver", NULL);
	protocol = bc_db_get_val(dbres, "protocol", NULL);
	if (!device || !protocol || !driver) {
		errno = EINVAL;
		return NULL;
	}

	bc = (struct bc_handle*) malloc(sizeof(struct bc_handle));
	if (bc == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	memset(bc, 0, sizeof(struct bc_handle));
	bc->substream_input = NULL;
	bc->input = NULL;

	snprintf(bc->device, sizeof(bc->device), "%s", device);
	snprintf(bc->driver, sizeof(bc->driver), "%s", driver);
	bc_log(Debug, "bc_handle_get(): device: %s, driver: %s, protocol: %s", bc->device, bc->driver, protocol);

	bc_ptz_check(bc, dbres);
	bc_log(Debug, "bc_handle_get(): ptz check done");

	int substream_mode = bc_db_get_val_int(dbres, "substream_mode");
	if (substream_mode < 0) substream_mode = 0;
	bc->substream_mode = (bc_streaming_substream_mode_t)substream_mode;

	if (!strncmp(protocol, "IP", 2)) {
		ret = lavf_handle_init(bc, dbres);
	} else if (!strncmp(device, "TW5864", 6)) {
		bc->type = BC_DEVICE_V4L2_TW5864;
		bc->input = new v4l2_device_tw5864(dbres);
		ret = bc->input->has_error();

		/* TODO Add variant for GENERIC */
	} else {
		char solo_driver_name[32];
		solo_driver_name[0] = '\0';
		get_solo_driver_name(solo_driver_name, sizeof(solo_driver_name));

		if (!strncmp(solo_driver_name, "solo6x10_edge", 13)) {
			bc_log(Debug, "v4l2_device: solo6010_dkms");
			bc->type = BC_DEVICE_V4L2_SOLO6010_DKMS;
			bc->input = new v4l2_device_solo6010_dkms(dbres);
		} else {
			bc_log(Debug, "v4l2_device: solo6x10");
			bc->type = BC_DEVICE_V4L2_SOLO6X10;
			bc->input = new v4l2_device_solo6x10(dbres);
		}

		ret = bc->input->has_error();
	}

	bc->source = new stream_source("Input Source");

	if (ret) {
		bc_handle_free(bc);
		return NULL;
	}

	bc_log(Debug, "bc_handle_get(): done");
	return bc;
}

void bc_handle_free(struct bc_handle *bc)
{
	/* Don't want this call to change errno at all */
	int save_err = errno;

	if (!bc)
		return;

	if (bc->input)
		bc->input->stop();

	if (bc->input)
		delete bc->input;

	delete bc->source;

	if (bc->substream_mode && bc->substream_input) {
		bc->substream_input->stop();
		delete bc->substream_input;
	}


	free(bc);

	errno = save_err;
}

int bc_device_config_init(struct bc_device_config *cfg, BC_DB_RES dbres)
{
	const char *dev = bc_db_get_val(dbres, "device", NULL);
	const char *name = bc_db_get_val(dbres, "device_name", NULL);
	const char *driver = bc_db_get_val(dbres, "driver", NULL);
	const char *schedule = bc_db_get_val(dbres, "schedule", NULL);
	const char *motion_map = bc_db_get_val(dbres, "motion_map", NULL);
	const char *signal_type = bc_db_get_val(dbres, "signal_type", NULL);
	const char *rtsp_username = bc_db_get_val(dbres, "rtsp_username", NULL);
	const char *rtsp_password = bc_db_get_val(dbres, "rtsp_password", NULL);
	const char *rtsp_rtp_prefer_tcp_raw = bc_db_get_val(dbres, "rtsp_rtp_prefer_tcp", NULL);
	const char *substream_path = bc_db_get_val(dbres, "substream_path", NULL);

	if (!dev || !name || !schedule || !motion_map)
		return -1;

	if (rtsp_rtp_prefer_tcp_raw && strlen(rtsp_rtp_prefer_tcp_raw)) {
		cfg->rtsp_rtp_prefer_tcp = atoi(rtsp_rtp_prefer_tcp_raw);
	} else {
		bc_log(Error, "rtsp_rtp_prefer_tcp field is absent in Devices table, "
				"DB schema not updated. Using default field value, \"AUTO\"");
		cfg->rtsp_rtp_prefer_tcp = 0;
	}

	strlcpy(cfg->dev, dev, sizeof(cfg->dev));
	strlcpy(cfg->name, name, sizeof(cfg->name));
	if (driver)
		strlcpy(cfg->driver, driver, sizeof(cfg->driver));
	strlcpy(cfg->schedule, schedule, sizeof(cfg->schedule));
	strlcpy(cfg->motion_map, motion_map, sizeof(cfg->motion_map));

	if (signal_type)
		strlcpy(cfg->signal_type, signal_type, sizeof(cfg->signal_type));
	if (rtsp_username)
		strlcpy(cfg->rtsp_username, rtsp_username, sizeof(cfg->rtsp_username));
	if (rtsp_password)
		strlcpy(cfg->rtsp_password, rtsp_password, sizeof(cfg->rtsp_password));
	if (substream_path)
		strlcpy(cfg->substream_path, substream_path, sizeof(cfg->substream_path));

	cfg->width = bc_db_get_val_int(dbres, "resolutionX");
	cfg->height = bc_db_get_val_int(dbres, "resolutionY");
	cfg->interval = bc_db_get_val_int(dbres, "video_interval");
	cfg->prerecord = (int16_t)bc_db_get_val_int(dbres, "buffer_prerecording");
	cfg->postrecord = (int16_t)bc_db_get_val_int(dbres, "buffer_postrecording");
	cfg->motion_analysis_ssw_length = (int64_t)bc_db_get_val_int(dbres, "motion_analysis_ssw_length");
	if (cfg->motion_analysis_ssw_length == -1) {
		bc_log(Debug, "motion_analysis_ssw_length field is set to -1. Using default value");
		cfg->motion_analysis_ssw_length = 1;  // 1 frame, considering no false positives by default
		if (!strncmp(driver, "tw5864", 6))
			cfg->motion_analysis_ssw_length = 10;  // 10 frames, considering high false-positives rate

	}
	cfg->motion_analysis_percentage = bc_db_get_val_int(dbres, "motion_analysis_percentage");
	if (cfg->motion_analysis_percentage == -1) {
		bc_log(Debug, "motion_analysis_percentage field is set to -1. Using default value");
		cfg->motion_analysis_percentage = 1;  // 1%, considering no false positives by default
		if (!strncmp(driver, "tw5864", 6))
			cfg->motion_analysis_percentage = 25;  // 25% (I-frames are not flagged), considering high false-positives rate
	}
	cfg->debug_level = (int8_t)bc_db_get_val_int(dbres, "debug_level");
	cfg->aud_disabled = bc_db_get_val_int(dbres, "audio_disabled") != 0;
	cfg->schedule_override_global = bc_db_get_val_int(dbres, "schedule_override_global") != 0;
	cfg->hue        = bc_db_get_val_int(dbres, "hue");
	cfg->contrast   = bc_db_get_val_int(dbres, "contrast");
	cfg->brightness = bc_db_get_val_int(dbres, "brightness");
	cfg->saturation = bc_db_get_val_int(dbres, "saturation");
	cfg->video_quality = bc_db_get_val_int(dbres, "video_quality");

	cfg->onvif_events_enabled = bc_db_get_val_int(dbres, "onvif_events_enabled") != 0;
	cfg->onvif_port = bc_db_get_val_int(dbres, "onvif_port");
	bc_db_get_val_str(dbres, "onvif_event_types", cfg->onvif_event_types, sizeof(cfg->onvif_event_types));

	cfg->motion_algorithm = bc_db_get_val_int(dbres, "motion_algorithm");
	cfg->motion_frame_downscale_factor = bc_db_get_val_float(dbres, "frame_downscale_factor");
	cfg->min_motion_area = bc_db_get_val_int(dbres, "min_motion_area");
    cfg->max_motion_area = bc_db_get_val_int(dbres, "max_motion_area");

    cfg->max_motion_frames = bc_db_get_val_int(dbres, "max_motion_frames");
    cfg->min_motion_frames = bc_db_get_val_int(dbres, "min_motion_frames");
    cfg->motion_blend_ratio = bc_db_get_val_int(dbres, "motion_blend_ratio");
    cfg->motion_debug = bc_db_get_val_int(dbres, "motion_debug");

    cfg->reencode_enabled = bc_db_get_val_int(dbres, "reencode_livestream") != 0;
	cfg->reencode_bitrate = bc_db_get_val_int(dbres, "reencode_bitrate");
	cfg->reencode_frame_width = bc_db_get_val_int(dbres, "reencode_frame_width");
	cfg->reencode_frame_height = bc_db_get_val_int(dbres, "reencode_frame_height");

	cfg->substream_mode = (int8_t)bc_db_get_val_int(dbres, "substream_mode");

	cfg->hls_window_size = bc_db_get_val_int(dbres, "hls_window_size");
	cfg->hls_segment_size = bc_db_get_val_int(dbres, "hls_segment_size");
	cfg->hls_segment_duration = bc_db_get_val_float(dbres, "hls_segment_duration");

	return 0;
}

