/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <syslog.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "libbluecherry.h"
#include "rtp-session.h"
#include "v4l2device.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
}

int bc_set_motion(struct bc_handle *bc, int on)
{
	if (bc->type == BC_DEVICE_V4L2)
		return reinterpret_cast<v4l2_device*>(bc->input)->set_motion(on);
	bc_log("XXX: bc_set_motion crippled");
	return -ENOSYS;
}

int bc_motion_is_on(struct bc_handle *bc)
{
#if 0
	v4l2_device *d = 0;
	if (bc->type == BC_DEVICE_V4L2) {
		struct v4l2_buffer *vb;

		vb = bc_buf_v4l2(bc);
		if (vb == NULL)
			return 0;

		return vb->flags & V4L2_BUF_FLAG_MOTION_ON ? 1 : 0;
	}
#endif

	return 1;
}

static uint16_t solo_value_map[] = {
	0xffff, 1152, 1024, 768, 512, 3842
};

static uint8_t generic_value_map[] = {
	255, 48, 26, 12, 7, 3
};

int bc_set_motion_thresh_global(struct bc_handle *bc, char value)
{
	int val = value - '0';
	if (val < 0 || val > 5)
		return -1;

	if (bc->type == BC_DEVICE_V4L2) {
		v4l2_device *d = reinterpret_cast<v4l2_device*>(bc->input);
		if (d->caps() & BC_CAM_CAP_V4L2_MOTION) {
			struct v4l2_control vc;
			vc.id = V4L2_CID_MOTION_THRESHOLD;
			/* Upper 16 bits are 0 for the global threshold */
			vc.value = solo_value_map[val];
			return ioctl(d->device_fd(), VIDIOC_S_CTRL, &vc);
		}
	}
#if 0
	else {
		memset(bc->motion_data.thresholds, generic_value_map[val],
		       sizeof(bc->motion_data.thresholds));
	}
#endif

	return 0;
}

int bc_set_motion_thresh(struct bc_handle *bc, const char *map, size_t size)
{
	if (bc->type == BC_DEVICE_V4L2) {
		v4l2_device *d = reinterpret_cast<v4l2_device*>(bc->input);
		if (!(d->caps() & BC_CAM_CAP_V4L2_MOTION))
			return -ENOSYS;

		struct v4l2_control vc;
		int vh = 15;
		int i;
		vc.id = V4L2_CID_MOTION_THRESHOLD;

		if (d->caps() & BC_CAM_CAP_V4L2_PAL)
			vh = 18;

		if (size < 22 * vh)
			return -1;

		/* Our input map is 22xvh, but the device is actually twice that.
		 * Fields are doubled accordingly. */
		for (i = 0; i < (vh*2); i++) {
			int j;
			for (j = 0; j < 44; j++) {
				int pos = ((i/2)*22)+(j/2);
				if (map[pos] < '0' || map[pos] > '5')
					return -1;

				/* One more than the actual block number, because the driver
				 * expects this. 0 sets the global threshold. */
				vc.value = (unsigned)(i*64+j+1) << 16;
				vc.value |= solo_value_map[map[pos] - '0'];

				if (ioctl(d->device_fd(), VIDIOC_S_CTRL, &vc))
					return -1;
			}
		}
	}
#if 0
	else {
		size_t i;
		if (size < 32*24)
			return -1;
		size = 32*24;

		for (i = 0; i < size; ++i) {
			if (map[i] < '0' || map[i] > '5')
				return -1;
			bc->motion_data.thresholds[i] = generic_value_map[map[i] - '0'];
		}
	} 
#endif

	return 0;
}

