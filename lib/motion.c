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

#include <libbluecherry.h>
#include <libswscale/swscale.h>

struct v4l2_buffer *bc_buf_v4l2(struct bc_handle *bc);

int bc_set_motion(struct bc_handle *bc, int on)
{
	int ret = 0;

	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;

		vc.id = V4L2_CID_MOTION_ENABLE;
		vc.value = on ? 1 : 0;

		ret = ioctl(bc->v4l2.dev_fd, VIDIOC_S_CTRL, &vc);
	} else if (!on) {
		/* Free resources from generic motion detection */
		if (bc->motion_data.convContext) {
			sws_freeContext(bc->motion_data.convContext);
			bc->motion_data.convContext = 0;
		}
		if (bc->motion_data.refFrame) {
			av_free(bc->motion_data.refFrame->data[0]);
			av_free(bc->motion_data.refFrame);
			bc->motion_data.refFrame = 0;
		}
#ifdef DEBUG_DUMP_MOTION_DATA
		if (bc->motion_data.dumpfile) {
			fclose(bc->motion_data.dumpfile);
			bc->motion_data.dumpfile = 0;
		}
#endif
	}

	if (!ret)
		bc->motion_data.enabled = on;

	return ret;
}

int bc_motion_is_on(struct bc_handle *bc)
{
	if (!bc->motion_data.enabled)
		return 0;

	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_buffer *vb;

		vb = bc_buf_v4l2(bc);
		if (vb == NULL)
			return 0;

		return vb->flags & V4L2_BUF_FLAG_MOTION_ON ? 1 : 0;
	}

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

	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;
		vc.id = V4L2_CID_MOTION_THRESHOLD;

		/* Upper 16 bits are 0 for the global threshold */
		vc.value = solo_value_map[val];

		return ioctl(bc->v4l2.dev_fd, VIDIOC_S_CTRL, &vc);
	} else {
		memset(bc->motion_data.thresholds, generic_value_map[val],
		       sizeof(bc->motion_data.thresholds));
	}

	return 0;
}

int bc_set_motion_thresh(struct bc_handle *bc, const char *map, size_t size)
{
	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;
		int vh = 15;
		int i;
		vc.id = V4L2_CID_MOTION_THRESHOLD;

		if (bc->cam_caps & BC_CAM_CAP_V4L2_PAL)
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

				if (ioctl(bc->v4l2.dev_fd, VIDIOC_S_CTRL, &vc))
					return -1;
			}
		}
	} else {
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

	return 0;
}

/* YUVJ* formats are deprecated in libav, but still used in some places,
 * including h264. Because they're deprecated, swscale doesn't recognize
 * that they are effectively identical to non-J variants, so the grayscale
 * conversion is much slower than it should be. */
static int fix_pix_fmt(int fmt)
{
	switch (fmt) {
	case PIX_FMT_YUVJ420P: return PIX_FMT_YUV420P;
	case PIX_FMT_YUVJ422P: return PIX_FMT_YUV422P;
	case PIX_FMT_YUVJ444P: return PIX_FMT_YUV444P;
	default: return fmt;
	}
}

int bc_motion_is_detected(struct bc_handle *bc)
{
	int ret = 0;

	if (!bc_motion_is_on(bc))
		return 0;

	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_buffer *vb;

		vb = bc_buf_v4l2(bc);
		if (vb == NULL)
			return 0;

		return vb->flags & V4L2_BUF_FLAG_MOTION_DETECTED ? 1 : 0;
	} else if (bc->type == BC_DEVICE_RTP) {
		struct bc_motion_data *md = &bc->motion_data;
		AVFrame rawFrame, frame;
		AVCodecContext *cctx;
		uint8_t *buf;
		int bufSize;
		int r;

		if (!bc_buf_is_video_frame(bc))
			return 0;

		avcodec_get_frame_defaults(&rawFrame);
		avcodec_get_frame_defaults(&frame);

		r = rtp_device_decode_video(&bc->rtp, &rawFrame);
		if (r < 0) {
			// XXX report error
			return 0;
		} else if (!r) {
			// no picture decoded from this frame
			return 0;
		}

		cctx = bc->rtp.ctx->streams[bc->rtp.video_stream_index]->codec;

		md->convContext = sws_getCachedContext(md->convContext, cctx->width,
			cctx->height, fix_pix_fmt(cctx->pix_fmt), cctx->width, cctx->height,
			PIX_FMT_GRAY8, SWS_BICUBIC, NULL, NULL, NULL);

		/* XXX preallocated buffer? */
		bufSize = avpicture_get_size(PIX_FMT_GRAY8, cctx->width, cctx->height);
		buf = av_malloc(bufSize);
		if (!buf)
			return -1;
		avpicture_fill((AVPicture*)&frame, buf, PIX_FMT_GRAY8, cctx->width, cctx->height);

		sws_scale(md->convContext, rawFrame.data, rawFrame.linesize, 0,
		          cctx->height, frame.data, frame.linesize);

#ifdef DEBUG_DUMP_MOTION_DATA
		if (!md->dumpfile) {
			char filename[128];
			snprintf(filename, sizeof(filename), "/tmp/bc.motion.%d", (int)rand());
			md->dumpfile = fopen(filename, "w");
			if (md->dumpfile)
				bc_log("Opened motion data file '%s'", filename);
			else
				bc_log("Failed to open motion data file '%s' (%m)", filename);
		}
#endif

		if (md->refFrame && md->refFrameHeight == cctx->height && md->refFrameWidth == cctx->width)
		{
			uint8_t *ref = md->refFrame->data[0];
			uint8_t *cur = frame.data[0];
			uint8_t *val = 0;
			uint8_t cv = 0; /* current value, for val[0] */
			uint8_t lv = 0;
			int w = frame.linesize[0], h = cctx->height;
			int total = w * h;
			int x = 0, y = 0;
			int threshold_cell_w = ceil(w/32.0);
			int threshold_cell_h = ceil(h/24.0);
			int threshold_cell = 0;
			int threshold_x_ctr = threshold_cell_w;
			int threshold_current;

			/* Would it be good to merge these loops again? My gut feeling is no,
			 * because ref would be taking up cache space, but it might avoid
			 * reading cur in twice if that gets pushed out of the cache. We
			 * could also avoid the write to cur in that case, which might be
			 * worthwhile. */
			for (; x < total; ++x) {
				/* XXX is the compiler smart about multiple loads from ref[x] and
				 * cur[x]? */
				cv = abs((int)ref[x] - (int)cur[x]);
				/* Merge the current frame into the reference frame. This
				 * makes the reference a background that slowly adapts to
				 * changes in the image, so instant changes will persist
				 * longer and thus result in larger blobs of change. Consider,
				 * for example, the case of a small, fast moving object.
				 *
				 * Currently, we add 10% of the new frame to 90% of the current
				 * reference.
				 *
				 * XXX is there a faster algorithm for this? Could we use the
				 * difference already calculated to do something more clever? */
				ref[x] = ((ref[x])*0.9) + ((cur[x])*0.1);
				cur[x] = cv;
			}

			/* XXX preallocated buffer? it still has to be zero'd */
			val = calloc(w+1, sizeof(uint8_t));
			x = 0;
			cv = 0;
			ref = md->thresholds;
			threshold_current = ref[0];

			/* From the northwest corner, proceed east and south over each pixel.
			 * The value for a pixel is the value from the pixels to the north,
			 * northwest, and northeast (with no double counting). Add one to
			 * that value for any pixel with change exceeding the threshold, and
			 * halve it for any pixel that does not.
			 *
			 * The result is that we have high values on the south/east edges of
			 * areas with many changes exceeding the threshold, and small changes
			 * (which are likely irrelevant) are ignored.
			 */
			for (;;) {
				if (!--threshold_x_ctr) {
					threshold_x_ctr = threshold_cell_w;
					threshold_current = ref[++threshold_cell];
				}

				/* XXX this line causes a surprising number of cache misses
				 * in one profile.. */
				cv += val[x+1];
				if (*(cur++) > threshold_current) {
					cv++;
					val[x] = cv - lv;
				} else {
					cv /= 2;
					val[x] = 0;
				}

				lv = cv;
				/* XXX magic threshold. This probably needs to be based on resolution,
				 * or configurable somehow. This defines the minimum blob of change
				 * needed to trigger motion. */
				if (cv > 149) {
					ret = 1;
#ifndef DEBUG_DUMP_MOTION_DATA
					/* Remember to comment this out if doing measurements; it will
					 * confuse results when there is motion. */
					break;
#endif
				}

#ifdef DEBUG_DUMP_MOTION_DATA
				fwrite(&cv, 1, 1, md->dumpfile);
#endif

				if (__builtin_expect(++x == w, 0)) {
					x = 0;
					if (__builtin_expect(++y == h, 0))
						break;
					/* XXX we could avoid this by storing the current y cell and
					 * counting it as we do with x. Would that be helpful? */
					threshold_cell = (y/threshold_cell_h) << 5;
					threshold_current = ref[threshold_cell];
					cv = val[0];
					lv = 0;
				}
			}

			free(val);
			av_free(buf);
		} else {
			if (md->refFrame) {
				av_free(md->refFrame->data[0]);
				av_free(md->refFrame);
			}
			md->refFrame = avcodec_alloc_frame();
			/* Using sizeof(AVFrame) is forbidden, but we link against our own libav. */
			memcpy(md->refFrame, &frame, sizeof(frame));
			md->refFrameHeight = cctx->height;
			md->refFrameWidth  = cctx->width;
		}
	}

	return ret;
}

