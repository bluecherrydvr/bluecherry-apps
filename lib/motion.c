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
		AVFrame *rawFrame, *frame;
		AVCodecContext *cctx;
		uint8_t *buf;
		uint32_t *result = 0;
		int bufSize;
		int r;

		if (!bc_buf_is_video_frame(bc))
			return 0;

		rawFrame = avcodec_alloc_frame();
		r = rtp_device_decode_video(&bc->rtp, rawFrame);
		if (r < 0) {
			// XXX report error
			return 0;
		} else if (!r) {
			// no picture decoded from this frame
			return 0;
		}

		cctx = bc->rtp.ctx->streams[bc->rtp.video_stream_index]->codec;

		md->convContext = sws_getCachedContext(md->convContext, cctx->width,
			cctx->height, cctx->pix_fmt, cctx->width, cctx->height, PIX_FMT_GRAY8,
			SWS_BICUBIC, NULL, NULL, NULL);

		frame = avcodec_alloc_frame();
		bufSize = avpicture_get_size(PIX_FMT_GRAY8, cctx->width, cctx->height);
		buf = av_malloc(bufSize);
		if (!buf)
			return -1;
		avpicture_fill((AVPicture*)frame, buf, PIX_FMT_GRAY8, cctx->width, cctx->height);

		sws_scale(md->convContext, rawFrame->data, rawFrame->linesize, 0,
		          cctx->height, frame->data, frame->linesize);

		av_free(rawFrame);

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
			uint8_t *cur = frame->data[0];
			uint32_t *val;
			int w = frame->linesize[0], h = cctx->height;
			int total = w * h;
			int x = 0, y = 0;
			int threshold_cell_w = ceil(w/32.0);
			int threshold_cell_h = ceil(h/24.0);
			int threshold_cell = 0;

			result = malloc(total*sizeof(uint32_t));
			val = result;
			/* Initialize the first pixel. All others initialize from west or north */
			*val = 0;

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
				if (y && w-1 > x) {
					/* northeast; because its value is based on the north,
					 * we look for the difference from that. To avoid degrading
					 * too fast on the edges, we drop negative differences. */
					int r = val[-w+1] - val[-w];
					if (r > 0)
						*val += r;
				}

				if (abs(*ref - *cur) >= md->thresholds[threshold_cell])
					(*val)++;
				else
					*val >>= 1;

				if (*val >= 150) { // XXX magic threshold number
					uint8_t *end = md->refFrame->data[0] + total;
					ret = 1;
#ifndef DEBUG_DUMP_MOTION_DATA
					/* Quickly finish merging the reference frame (see below),
					 * and break */
					for (; ref != end; ++ref, ++cur)
						*ref = ((*ref)*0.9f) + ((*cur)*0.1f);
					break;
#else
					(void)end;
#endif
				}

				/* Merge the current frame into the reference frame. This
				 * makes the reference a background that slowly adapts to
				 * changes in the image, so instant changes will persist
				 * longer and thus result in larger blobs of change. Consider,
				 * for example, the case of a small, fast moving object.
				 *
				 * Currently, we add 10% of the new frame to 90% of the current
				 * reference. */
				*ref = ((*ref)*0.9f) + ((*cur)*0.1f);

				ref++;
				cur++;
				val++;
				if (++x == w) {
					x = 0;
					if (++y == h)
						break;
					threshold_cell = (y/threshold_cell_h) << 5;
					/* Initialize the next value using the value to the north.
					 * This case only comes after wrapping, because we use the
					 * west when x>0 */
					*val = val[-w];
				} else {
					if (!(x % threshold_cell_w))
						threshold_cell++;
					/* Initialize the next value based on this one (to the west) */
					*val = val[-1];
				}
			}
			
			av_free(frame->data[0]);
			av_free(frame);
		} else {
			if (md->refFrame) {
				av_free(md->refFrame->data[0]);
				av_free(md->refFrame);
			}
			md->refFrame = frame;
			md->refFrameHeight = cctx->height;
			md->refFrameWidth  = cctx->width;
		}

#ifdef DEBUG_DUMP_MOTION_DATA
		if (md->dumpfile && result) {
			int i;
			uint8_t *z = (uint8_t*)result;
			for (i = 0; i < bufSize; ++i, ++z) {
				if (result[i] < 45)
					*z = 0;
				else if (result[i] > 300)
					*z = 255;
				else
					*z = result[i] - 45;
			}
			size_t r = fwrite(result, 1, bufSize, md->dumpfile);
			if (r < bufSize)
				bc_log("Write error on motion dump file: %m");
		}
#endif

		free(result);
	}

	return ret;
}
