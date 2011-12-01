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

int bc_set_motion_thresh_global(struct bc_handle *bc, unsigned short val)
{
	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;

		vc.id = V4L2_CID_MOTION_THRESHOLD;
		vc.value = val;
		/* Upper 16 bits left to 0 for global */

		return ioctl(bc->v4l2.dev_fd, VIDIOC_S_CTRL, &vc);
	}

	return 0;
}

int bc_set_motion_thresh(struct bc_handle *bc, unsigned short val,
			 unsigned short block)
{
	if (bc->cam_caps & BC_CAM_CAP_V4L2_MOTION) {
		struct v4l2_control vc;

		vc.id = V4L2_CID_MOTION_THRESHOLD;
		vc.value = val;
		/* 0 means global; we must add one to the actual block */
		vc.value |= (unsigned int)(block+1) << 16;

		return ioctl(bc->v4l2.dev_fd, VIDIOC_S_CTRL, &vc);
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
			const int threshold = 15;
			uint8_t *ref = md->refFrame->data[0];
			uint8_t *cur = frame->data[0];
			int w = frame->linesize[0], h = cctx->height;
			int p = 0, total = w * h;
			uint32_t rmax = 0;

			result = malloc(total*sizeof(uint32_t));

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
			for (; p < total; ++p) {
				if (p % w) {
					/* west; value includes the northwest and north */
					result[p] = result[p-1];
				} else if (p >= w) {
					/* north; there is nothing to the west or northwest */
					result[p] = result[p-w];
				} else
					result[p] = 0;

				if (p % (w-1) && p >= w) {
					/* northeast; because its value is based on the north,
					 * we look for the difference from that. To avoid degrading
					 * too fast on the edges, we drop negative differences. */
					int r = result[p-w+1] - result[p-w];
					if (r > 0)
						result[p] += r;
				}

				if (abs(ref[p] - cur[p]) >= threshold)
					result[p]++;
				else
					result[p] /= 2;

				if (result[p] > rmax)
					rmax = result[p];

				/* Merge the current frame into the reference frame. This
				 * makes the reference a background that slowly adapts to
				 * changes in the image, so instant changes will persist
				 * longer and thus result in larger blobs of change. Consider,
				 * for example, the case of a small, fast moving object.
				 *
				 * Currently, we add 10% of the new frame to 90% of the current
				 * reference. */
				ref[p] = (ref[p]*0.9f) + (cur[p]*0.1f);
			}

			ret = rmax >= 300; // XXX magic threshold number
			
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
