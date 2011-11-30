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

		char *buf2 = av_malloc(bufSize);
		char *buf2p = buf2;
		memset(buf2, 0, bufSize);
#endif

		if (md->refFrame && md->refFrameHeight == cctx->height && md->refFrameWidth == cctx->width)
		{
			const int threshold = 15;
			uint8_t *ref = md->refFrame->data[0];
			uint8_t *cur = frame->data[0];
			int w = frame->linesize[0], h = cctx->height;
			int p = 0, total = w * h;
			int changed = 0;
			for (; p < total; ++p) {
				if (abs(ref[p] - cur[p]) > threshold) { // XXX magic sensitivity number
					int nearby_hits = 0;
					if (p % w) { // west
						nearby_hits += abs(ref[p-1] - cur[p-1]) > threshold;
						if (p >= w)
							nearby_hits += abs(ref[p-w-1] - cur[p-w-1]) > threshold; // northwest
						if (p+w < total)
							nearby_hits += abs(ref[p+w-1] - cur[p+w-1]) > threshold; // southwest
					}
					if (p % (w-1)) { // east
						nearby_hits += abs(ref[p+1] - cur[p+1]) > threshold;
						if (p >= w)
							nearby_hits += abs(ref[p-w+1] - cur[p-w+1]) > threshold; // northeast
						if (p+w < total)
							nearby_hits += abs(ref[p+w+1] - cur[p+w+1]) > threshold; // southeast
					}
					if (p >= w)
						nearby_hits += abs(ref[p-w] - cur[p-w]) > threshold; // north
					if (p+w < total)
						nearby_hits += abs(ref[p+w] - cur[p+w]) > threshold; // south

					if (nearby_hits >= 6) {
						++changed;
#ifdef DEBUG_DUMP_MOTION_DATA
						*buf2p = 255;
#endif
					}
				}
#ifdef DEBUG_DUMP_MOTION_DATA
				buf2p++;
#endif
			}

			ret = changed >= (total/128); // XXX magic threshold number

			av_free(md->refFrame->data[0]);
			av_free(md->refFrame);
		}
		md->refFrame = frame;
		md->refFrameHeight = cctx->height;
		md->refFrameWidth  = cctx->width;

#ifdef DEBUG_DUMP_MOTION_DATA
		if (md->dumpfile) {
			size_t r = fwrite(buf2, 1, bufSize, md->dumpfile);
			if (r < bufSize)
				bc_log("Write error on motion dump file: %m");
		}
		av_free(buf2);
#endif
	}

	return ret;
}
