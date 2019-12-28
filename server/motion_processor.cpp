#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "bt.h"

#include "motion_processor.h"
#include "vaapi.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>
}

#include "opencv2/opencv.hpp"
#include <vector>

motion_processor::motion_processor()
	: stream_consumer("Motion Detection"), decode_ctx(0), destroy_flag(false), convContext(0), refFrame(0),
	  last_tested_pts(AV_NOPTS_VALUE), skip_count(0), m_alg(BC_DEFAULT), m_refFrameUpdateCounters({0, 0}), m_downscaleFactor(0.5), m_minMotionAreaPercent(5)
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
static enum AVPixelFormat fix_pix_fmt(int fmt)
{
	switch (fmt) {
	case AV_PIX_FMT_YUVJ420P: return AV_PIX_FMT_YUV420P;
	case AV_PIX_FMT_YUVJ422P: return AV_PIX_FMT_YUV422P;
	case AV_PIX_FMT_YUVJ444P: return AV_PIX_FMT_YUV444P;
	default: return (enum AVPixelFormat)fmt;
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

	std::unique_lock<std::mutex> l(lock);
	while (!destroy_flag)
	{
		if (buffer.empty()) {
			buffer_wait.wait(l);
			continue;
		}

		stream_packet pkt = buffer.front();
		buffer.pop_front();

		if (pkt.type != AVMEDIA_TYPE_VIDEO) {
			output_source->send(pkt);
			continue;
		}

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
		avpkt.dts   = pkt.dts;
		avpkt.data  = const_cast<uint8_t*>(pkt.data());
		avpkt.size  = pkt.size;

		AVFrame *frame = av_frame_alloc();

		int have_picture = 0;
		int re = avcodec_decode_video2(decode_ctx, frame, &have_picture, &avpkt);
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
				int64_t diff = (frame->pts - last_tested_pts) / (AV_TIME_BASE / 1000);
				if (diff > 0 && diff < 200) {
					if (++skip_count > 15) {
						bc_log(Debug, "Motion detection skipped too many consecutive frames "
						       "(diff: %" PRId64 "); Buggy PTS?", diff);
					} else
						skip = true;
				}
			}

			if (!skip) {
				last_tested_pts = frame->pts;
				skip_count = 0;
				int re = detect(frame);
				if (re == 1)
					pkt.flags |= stream_packet::MotionFlag;
			}
		}

		av_frame_free(&frame);
		output_source->send(pkt);

		l.lock();
	}

	l.unlock();
	pthread_cleanup_pop(0);

	bc_log(Debug, "motion_processor destroying");
	delete this;
}

bool motion_processor::decode_create(const stream_properties &prop)
{
	int ret;
	if (decode_ctx)
		return true;

	AVCodec *codec = avcodec_find_decoder(prop.video.codec_id);
	if (!codec || !(decode_ctx = avcodec_alloc_context3(codec)))
		return false;

	prop.video.apply(decode_ctx);

	decode_ctx->get_format = vaapi_hwaccel::get_format;

	// XXX we may want to set some options here, such as disabling threaded decoding
	AVDictionary *decoder_opts = NULL;
	av_dict_set(&decoder_opts, "refcounted_frames", "1", 0);
	ret = avcodec_open2(decode_ctx, codec, &decoder_opts);
	av_dict_free(&decoder_opts);
	if (ret < 0) {
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

	av_frame_free(&refFrame);
}

int motion_processor::detect(AVFrame *rawFrame)
{
	/* Check if frame comes from vaapi hardware decoder */
	if (rawFrame->hw_frames_ctx)
	{
		if (!vaapi_hwaccel::hwdownload_frame(decode_ctx, rawFrame))
			return 0;
	}

	switch (m_alg)
	{
		case BC_DEFAULT:
		return detect_bc_original(rawFrame);

		case OPENCV:
		return detect_opencv(rawFrame);
	}

	return 0;
}

void motion_processor::set_motion_algorithm(int algo)
{
	m_alg = (enum detection_algorithm)algo;

	bc_log(Debug, "motion algorithm is set to %i", algo);
}

int motion_processor::set_frame_downscale_factor(double f)
{
	if (f > 1.0 || f < 0.1)
		return 0;

	m_downscaleFactor = f;

	bc_log(Debug, "frame downscale factor is set to %f", f);

	return 1;
}

int motion_processor::set_min_motion_area_percent(int p)
{
	if (p > 100 || p < 0)
		return 0;

	m_minMotionAreaPercent = p;

	bc_log(Debug, "min_motion_area_percent is set to %i", p);

	return 1;
}

/*
void dump_opencv_frame(cv::Mat &m, const char *name,  int *counter)
                {//debug
                        char fname[128];

                        sprintf(fname,"/tmp/%s_dump%d.bmp", name, *counter);

                        bc_log(Info, "Saving deltaFrame to %s", fname);

                        if (!cv::imwrite(fname, m))
                                bc_log(Error, "Failed to save deltaFrame");
                        (*counter)++;
                }
*/

int motion_processor::match_ref_frame_opencv(cv::Mat &curFrame, cv::Mat &refFrame, double minMotionArea, int w, int h)
{
	cv::Mat m, deltaFrame;

	cv::absdiff(refFrame, curFrame, deltaFrame);

	for (int i = 0; i < 32; i++)
		for (int j = 0; j < 24; j++)
		{
			int threshmap_cell = i + j *32;

			cv::Mat cell(deltaFrame, cv::Range(h/24 * j, h/24 * (j + 1)), cv::Range(w/32 * i, w/32 * (i + 1)));
			//according to OpenCV docs, changes to "cell" subarrays are reflected in deltaFrame, no copy-on-write is performed
			cv::threshold(cell, cell, thresholds[threshmap_cell], 255, cv::THRESH_BINARY);
		}

	int iterations = 2;
	cv::dilate(deltaFrame, deltaFrame, cv::Mat(), cv::Point(-1,-1), iterations);


	std::vector<std::vector<cv::Point>> contours;

	cv::findContours(deltaFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0,0));

	for( int i = 0; i < contours.size(); i++)
	{
		if (cv::contourArea(contours[i]) >= minMotionArea)
		{

			/*{
			static int dbg_cnt4 = 0;

			cv::Mat df(curFrame);

			cv::Rect boundRect = cv::boundingRect(cv::Mat(contours[i]));
			cv::rectangle(df, boundRect.tl(), boundRect.br(), cv::Scalar(255, 0, 0), 2, 8, 0);
			dump_opencv_frame(df, "motion",  &dbg_cnt4);
			}*/

			//motion is detected
			return 1;
		}
	}

	return 0;
}

int motion_processor::detect_opencv(AVFrame *rawFrame)
{
	int ret = 0;
	int dst_h, dst_w;
	double downscaleFactor = m_downscaleFactor;
	double minMotionArea = rawFrame->height * downscaleFactor * rawFrame->width * downscaleFactor / 100 * m_minMotionAreaPercent;

	dst_h = rawFrame->height * downscaleFactor;
	dst_w = rawFrame->width * downscaleFactor;

	cv::Mat m = cv::Mat(dst_h, dst_w, CV_8UC1);

	convContext = sws_getCachedContext(convContext, rawFrame->width, rawFrame->height,
		fix_pix_fmt(rawFrame->format), dst_w, dst_h,
		AV_PIX_FMT_GRAY8, SWS_FAST_BILINEAR, NULL, NULL, NULL);

	AVFrame *frame = av_frame_alloc();
	frame->format = AV_PIX_FMT_GRAY8;
	frame->width = dst_w;
	frame->height = dst_h;

	if (dst_w < 32 || dst_h < 24)
	{
		bc_log(Error, "Frame size is too small for motion detection! Increase frame downscale factor if it applies");
		return 0;
	}

	av_image_fill_arrays(frame->data, frame->linesize, m.data, AV_PIX_FMT_GRAY8, dst_w, dst_h, 1);

	sws_scale(convContext, (const uint8_t **)rawFrame->data, rawFrame->linesize, 0,
		  rawFrame->height, frame->data, frame->linesize);

	//OpenCV stuff goes here...
	cv::GaussianBlur(m, m, cv::Size(21, 21), 0);

	//match every 2nd reference frame
	if (!m_refFrames[0].empty() && m_refFrames[0].rows == frame->height && m_refFrames[0].cols == frame->width && m_refFrameUpdateCounters[0] != 0)
	{

		ret = this->match_ref_frame_opencv(m, m_refFrames[0], minMotionArea, dst_w, dst_h);

		m_refFrameUpdateCounters[0]++;

		if (m_refFrameUpdateCounters[0] % 2 == 0)
			m_refFrameUpdateCounters[0] = 0;
	}
	else
	{
		m_refFrames[0] = m;
		bc_log(Debug, "opencv motion detection - setting reference frame 1");
		m_refFrameUpdateCounters[0] = 1;
	}

	if (ret == 0 && !m_refFrames[1].empty() && m_refFrames[1].rows == frame->height && m_refFrames[1].cols == frame->width && m_refFrameUpdateCounters[1] != 0)
	{
		ret = this->match_ref_frame_opencv(m, m_refFrames[1], minMotionArea, dst_w, dst_h);

                m_refFrameUpdateCounters[1]++;

                if (m_refFrameUpdateCounters[1] % 25 == 0)
                        m_refFrameUpdateCounters[1] = 0;
	}
	else if (ret == 0)
	{
                m_refFrames[1] = m;
                bc_log(Debug, "opencv motion detection - setting reference frame 2");
                m_refFrameUpdateCounters[1] = 1;
	}

	av_frame_free(&frame);
	return ret;
}

int motion_processor::detect_bc_original(AVFrame *rawFrame)
{
	int ret = 0;
	double downscaleFactor = m_downscaleFactor;

	convContext = sws_getCachedContext(convContext, rawFrame->width, rawFrame->height,
		fix_pix_fmt(rawFrame->format), rawFrame->width * downscaleFactor, rawFrame->height * downscaleFactor,
		AV_PIX_FMT_GRAY8, SWS_BICUBIC, NULL, NULL, NULL);

	AVFrame *frame = av_frame_alloc();
	frame->format = AV_PIX_FMT_GRAY8;
	frame->width = rawFrame->width * downscaleFactor;
	frame->height = rawFrame->height * downscaleFactor;
	av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, (AVPixelFormat)frame->format, 4);

	sws_scale(convContext, (const uint8_t **)rawFrame->data, rawFrame->linesize, 0,
		  rawFrame->height, frame->data, frame->linesize);

#ifdef DEBUG_DUMP_MOTION_DATA
	if (!md->dumpfile) {
		char filename[128];
		snprintf(filename, sizeof(filename), "/tmp/bc.motion.%d", (int)rand());
		md->dumpfile = fopen(filename, "w");
		if (md->dumpfile)
			bc_log(Warning, "Opened motion data file '%s'", filename);
		else
			bc_log(Error, "Failed to open motion data file '%s'", filename);
	}
#endif

	if (refFrame && refFrame->height == frame->height && refFrame->width == frame->width)
	{
		uint8_t *ref = refFrame->data[0];
		uint8_t *cur = frame->data[0];
		uint8_t *val = 0;
		uint8_t cv = 0; /* current value, for val[0] */
		uint8_t lv = 0;
		int w = frame->linesize[0], h = frame->height;
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
	} else {
		av_frame_free(&refFrame);
		refFrame = av_frame_clone(frame);
		assert(refFrame);
	}

	av_freep(&frame->data[0]);
	av_frame_free(&frame);
	return ret;
}

static uint8_t generic_value_map[] = {
	255, 48, 26, 12, 7, 3
};

int motion_processor::set_motion_thresh_global(char value)
{
	int val = clamp(value, '0', '5') - '0';

	/* Locking may not be necessary, but done out of an abundance of caution.. */
	std::lock_guard<std::mutex> l(lock);

	memset(thresholds, generic_value_map[val], sizeof(thresholds));

	return 0;
}

int motion_processor::set_motion_thresh(const char *map, size_t size)
{
	if (size < 32*24) {
		bc_log(Error, "Received motion thresholds map of invalid size for IP cam");
		return -1;
	}

	size = 32*24;

	std::lock_guard<std::mutex> l(lock);
	for (size_t i = 0; i < size; ++i) {
		int idx = clamp(map[i], '0', '5') - '0';
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
