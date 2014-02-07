#include <unistd.h>
#include <pthread.h>

#include "bt.h"

#include "motion_processor.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
}

motion_processor::motion_processor()
	: stream_consumer("Motion Detection"), decode_ctx(0), destroy_flag(false), convContext(0), refFrame(0),
	  refFrameHeight(0), refFrameWidth(0), last_tested_pts(AV_NOPTS_VALUE), skip_count(0)
{
	output_source = new stream_source("Motion Detection");
	set_motion_thresh_global('3');
}

motion_processor::~motion_processor()
{
	decode_destroy();

	if (convContext) {
		sws_freeContext(convContext);
		convContext = 0;
	}

#ifdef DEBUG_DUMP_MOTION_DATA
	if (bc->motion_data.dumpfile) {
		fclose(bc->motion_data.dumpfile);
		bc->motion_data.dumpfile = 0;
	}
#endif

	delete output_source;
}

void motion_processor::destroy()
{
	destroy_flag = true;
	buffer_wait.notify_all();
}

/* YUVJ* formats are deprecated in libav, but still used in some places,
 * including h264. Because they're deprecated, swscale doesn't recognize
 * that they are effectively identical to non-J variants, so the grayscale
 * conversion is much slower than it should be. */
static enum PixelFormat fix_pix_fmt(int fmt)
{
	switch (fmt) {
	case AV_PIX_FMT_YUVJ420P: return AV_PIX_FMT_YUV420P;
	case AV_PIX_FMT_YUVJ422P: return AV_PIX_FMT_YUV422P;
	case AV_PIX_FMT_YUVJ444P: return AV_PIX_FMT_YUV444P;
	default: return (enum PixelFormat)fmt;
	}
}

void motion_processor::thread_cleanup(void *data)
{
	motion_processor *mp = (motion_processor *)data;

	mp->lock.unlock();
	bt("monitor_processor unexpectedly cancelled", RET_ADDR);
}

void motion_processor::run()
{
	std::shared_ptr<const stream_properties> saved_properties;

	bc_log_context_push(log);

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_cleanup_push(thread_cleanup, this);

	bc_watchdog_add(&m_watchdog, motion_processor::watchdog_cb);

	std::unique_lock<std::mutex> l(lock);
	while (!destroy_flag)
	{
		bc_watchdog_update(&m_watchdog);

		if (buffer.empty()) {
			buffer_wait.wait(l);
			continue;
		}

		stream_packet pkt = buffer.front();
		buffer.pop_front();

		if (pkt.type != AVMEDIA_TYPE_VIDEO)
			continue;

		l.unlock();

		if (pkt.properties() != saved_properties) {
			saved_properties = pkt.properties();
			decode_destroy();
		}

		if (!decode_ctx && !decode_create(*saved_properties.get())) {
			bc_log(Error, "Cannot create decoder for video stream; motion detection will not work.");
			sleep(10);
			l.lock();
			continue;
		}

		AVPacket avpkt;
		av_init_packet(&avpkt);
		avpkt.flags = pkt.flags;
		avpkt.pts   = pkt.pts;
		avpkt.data  = const_cast<uint8_t*>(pkt.data());
		avpkt.size  = pkt.size;

		AVFrame frame;
		avcodec_get_frame_defaults(&frame);

		int have_picture = 0;
		int re = avcodec_decode_video2(decode_ctx, &frame, &have_picture, &avpkt);
		if (re < 0) {
			bc_log(Info, "Decoding failed for motion processor");
		} else if (have_picture) {
			/* For high framerates, we can achieve the same level of motion detection
			 * with much less CPU by testing at a reduced framerate. This will only run
			 * detection on frames with a PTS more than 45ms after the previous tested
			 * frame. There is a limit of 3 consecutive frame skips, because that shouldn't
			 * happen (other than a >60fps stream). */
			bool skip = false;
			if (refFrame && last_tested_pts != (int64_t)AV_NOPTS_VALUE) {
				int64_t diff = (pkt.pts - last_tested_pts) / (AV_TIME_BASE / 1000);
				if (diff > 0 && diff < 45) {
					if (++skip_count > 3) {
						bc_log(Debug, "Motion detection skipped too many consecutive frames "
						       "(diff: %" PRId64 "); Buggy PTS?", diff);
					} else
						skip = true;
				}
			}

			if (!skip) {
				last_tested_pts = pkt.pts;
				skip_count = 0;
				int re = detect(&frame);
				if (re == 1)
					pkt.flags |= stream_packet::MotionFlag;
			}
		}

		output_source->send(pkt);

		l.lock();
	}

	l.unlock();
	bc_watchdog_rm(&m_watchdog);

	pthread_cleanup_pop(0);

	bc_log(Debug, "motion_processor destroying");
	delete this;
}

bool motion_processor::decode_create(const stream_properties &prop)
{
	if (decode_ctx)
		return true;

	AVCodec *codec = avcodec_find_decoder(prop.video.codec_id);
	if (!codec || !(decode_ctx = avcodec_alloc_context3(codec)))
		return false;

	prop.video.apply(decode_ctx);

	// XXX we may want to set some options here, such as disabling threaded decoding
	if (avcodec_open2(decode_ctx, codec, NULL) < 0) {
		avcodec_close(decode_ctx);
		av_free(decode_ctx);
		decode_ctx = 0;
	}

	return decode_ctx != 0;
}

void motion_processor::decode_destroy()
{
	if (decode_ctx) {
		avcodec_close(decode_ctx);
		av_free(decode_ctx);
		decode_ctx = 0;
	}

	if (refFrame) {
		av_free(refFrame->data[0]);
		av_free(refFrame);
		refFrame = 0;
	}
}

int motion_processor::detect(AVFrame *rawFrame)
{
	int ret = 0;

	convContext = sws_getCachedContext(convContext, rawFrame->width, rawFrame->height,
		fix_pix_fmt(rawFrame->format), rawFrame->width, rawFrame->height,
		AV_PIX_FMT_GRAY8, SWS_BICUBIC, NULL, NULL, NULL);

	/* XXX preallocated buffer? */
	int bufSize = avpicture_get_size(AV_PIX_FMT_GRAY8, rawFrame->width, rawFrame->height);
	uint8_t *buf = (uint8_t*)av_malloc(bufSize);
	if (!buf)
		return -1;

	AVFrame frame;
	avcodec_get_frame_defaults(&frame);
	avpicture_fill((AVPicture*)&frame, buf, AV_PIX_FMT_GRAY8, rawFrame->width, rawFrame->height);

	sws_scale(convContext, (const uint8_t **)rawFrame->data, rawFrame->linesize, 0,
		  rawFrame->height, frame.data, frame.linesize);

#ifdef DEBUG_DUMP_MOTION_DATA
	if (!md->dumpfile) {
		char filename[128];
		snprintf(filename, sizeof(filename), "/tmp/bc.motion.%d", (int)rand());
		md->dumpfile = fopen(filename, "w");
		if (md->dumpfile)
			bc_log(Warning, "Opened motion data file '%s'", filename);
		else
			bc_log(Error, "Failed to open motion data file '%s' (%m)", filename);
	}
#endif

	if (refFrame && refFrameHeight == rawFrame->height && refFrameWidth == rawFrame->width)
	{
		uint8_t *ref = refFrame->data[0];
		uint8_t *cur = frame.data[0];
		uint8_t *val = 0;
		uint8_t cv = 0; /* current value, for val[0] */
		uint8_t lv = 0;
		int w = frame.linesize[0], h = rawFrame->height;
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
		val = (uint8_t*)calloc(w+1, sizeof(uint8_t));
		x = 0;
		cv = 0;
		ref = thresholds;
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
		if (refFrame) {
			av_free(refFrame->data[0]);
			av_free(refFrame);
		}
		refFrame = avcodec_alloc_frame();
		/* Using sizeof(AVFrame) is forbidden, but we link against our own libav. */
		memcpy(refFrame, &frame, sizeof(frame));
		refFrameHeight = rawFrame->height;
		refFrameWidth  = rawFrame->width;
	}

	return ret;
}

static uint8_t generic_value_map[] = {
	255, 48, 26, 12, 7, 3
};

int motion_processor::set_motion_thresh_global(char value)
{
	int val = range(value, '0', '5') - '0';

	/* Locking may not be necessary, but done out of an abundance of caution.. */
	std::lock_guard<std::mutex> l(lock);

	memset(thresholds, generic_value_map[val], sizeof(thresholds));

	return 0;
}

int motion_processor::set_motion_thresh(const char *map, size_t size)
{
	if (size < 32*24)
		return -1;
	size = 32*24;

	std::lock_guard<std::mutex> l(lock);
	for (size_t i = 0; i < size; ++i) {
		int idx = range(map[i], '0', '5') - '0';
		thresholds[i] = generic_value_map[idx];
	}

	return 0;
}

static void *bc_mproc_thread(void *data)
{
	motion_processor *mp = (motion_processor *)data;
	mp->run();
	return NULL;
}

void motion_processor::watchdog_cb(struct watchdog *w)
{
	motion_processor *mp = (motion_processor *)
		((char*)w - __builtin_offsetof(motion_processor, m_watchdog));
	pthread_cancel(mp->m_thread);
	pthread_join(mp->m_thread, NULL);
	mp->start_thread();
}

void motion_processor::start_thread()
{
	int ret;

	ret = pthread_create(&m_thread, NULL, bc_mproc_thread, this);
	if (ret != 0) {
		bc_log(Error, "Error starting motion processor thread: %s",
		       strerror(errno));
		return;
	}
	pthread_detach(m_thread);
}
