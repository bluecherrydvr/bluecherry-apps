#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "bt.h"
#include "bc-server.h"

#include "motion_processor.h"
#include "vaapi.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>
}

#include <string.h>
#include <limits.h>     /* PATH_MAX */
#include <sys/stat.h>   /* mkdir(2) */
#include <errno.h>

#include "opencv2/opencv.hpp"
#include <vector>

motion_processor::motion_processor(bc_record *bcRecord)
	: stream_consumer("Motion Detection"), decode_ctx(0), destroy_flag(false), convContext(0), refFrame(0),
	  last_tested_pts(AV_NOPTS_VALUE), skip_count(0), m_alg(BC_DEFAULT), m_refFrameUpdateCounters({0, 0}),
	  m_downscaleFactor(0.5), m_minMotionAreaPercent(3), m_maxMotionAreaPercent(90), m_maxMotionFrames(20),
	  m_minMotionFrames(15), m_motionBlendRatio(0.9), m_motionDebug(true), m_recorder(bcRecord)
{
	output_source = new stream_source("Motion Detection");
	set_motion_thresh_global('3');

	md_frame_pool_index = 0;
	memset(md_frame_pool, 0, 255);

	memset(&m_debugEventTime, 0, sizeof(struct tm));
	m_debugEventTime.tm_sec = -1;

	// m_refFrames[2] is a static array so ctors, dtors are not called automatically on elements
	new(&m_refFrames[0]) cv::Mat();
	new(&m_refFrames[1]) cv::Mat();
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

	// m_refFrames[2] is a static array so ctors, dtors are not called automatically on elements
	m_refFrames[0].~Mat();
	m_refFrames[1].~Mat();
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
	pthread_setname_np(pthread_self(), thread_name);
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

		int ret = avcodec_send_packet(decode_ctx, &avpkt);
		if (ret < 0) {
			char error[512];
			av_strerror(ret, error, sizeof(error));
			bc_log(Warning, "motion processor: avcodec_send_packet failed: %s", error);
		}

		while (true) {
			ret = avcodec_receive_frame(decode_ctx, frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;

			if (ret < 0) {
				char error[512];
				av_strerror(ret, error, sizeof(error));
				bc_log(Warning, "motion processor: avcodec_receive_frame failed: %s", error);
				break;
			}

			have_picture = 1;
			break;
		}

		if (have_picture) {
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
						bc_log(Info, "Motion detection skipped too many consecutive frames "
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

	const AVCodec *codec = avcodec_find_decoder(prop.video.codec_id);
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

	m_debugFrameNum++;
	switch (m_alg)
	{
		case BC_DEFAULT:
    		return detect_bc_original(rawFrame);

		case CV_BASIC:
            return detect_opencv(rawFrame);

        case CV_MULTIFRAME:
            return detect_opencv_advanced(rawFrame);
	}

	return 0;
}

void motion_processor::set_motion_algorithm(int algo)
{
	m_alg = (enum detection_algorithm)algo;

	bc_log(Info, "motion algorithm is set to %i", algo);
}

int motion_processor::set_frame_downscale_factor(double f)
{
	if (f > 1.0 || f < 0.1)
		return 0;

	m_downscaleFactor = f;

	bc_log(Info, "frame downscale factor is set to %f", f);

	return 1;
}

int motion_processor::set_min_motion_area_percent(int p)
{
	if (p > 100 || p < 0 || ( p > m_maxMotionAreaPercent * 100))
	    p = 5;
		// return 0;

    m_minMotionAreaPercent = p / 100.;

	bc_log(Info, "min_motion_area_percent is set to %i", p);

	return 1;
}

int motion_processor::set_max_motion_area_percent(int p)
{
    if (p > 100 || p < 0 || (p < m_minMotionAreaPercent * 100))
        p = 90;
        //return 0;

    m_maxMotionAreaPercent = p / 100.;

    bc_log(Info, "max_motion_area_percent is set to %i", p);

    return 1;
}

int motion_processor::set_max_motion_frames(int max)
{
    if (max < m_minMotionFrames)
        max = max < m_minMotionFrames ? m_minMotionFrames * 1.33339 : 20;
        //return 0;

    if (max > 255 || max < 1)
        max = 20;

    m_maxMotionFrames = max;

    bc_log(Info, "max_motion_frames is set to %i", max);

    return 1;
}

int motion_processor::set_min_motion_frames(int min)
{
    if (min > m_maxMotionFrames)
        min = min > m_maxMotionFrames ? (m_maxMotionFrames * 0.75) : 15;
        //return 0;
    if (min > 255 || min < 1)
        min = 15;

    m_minMotionFrames = min;

    bc_log(Info, "min_motion_frames is set to %i", min);

    return 1;
}

int motion_processor::set_motion_blend_ratio(int ratio)
{
    if (ratio > 100 || ratio < 2)
        ratio = 10;
        //return 0;

    m_motionBlendRatio = 1.0 / ratio;

    bc_log(Info, "motion_blend_radio is set to %i:1", ratio);

    return 1;
}

int motion_processor::set_motion_debug(int debug)
{
    if (debug > 1 || debug < 0)
        return 0;

    m_motionDebug = (debug == 1);

    bc_log(Info, "motion_debug is set to %i", debug);

    return 1;
}


/**
 * Utility function to create a directory path, creating parents as needed.
 * @param path
 * @return 0 on success, -1 on error
 */

int mkdir_p(const char *path, int perms)
{
    /* Adapted from http://stackoverflow.com/a/2336245/119527 */
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p;

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1;
    }
    strcpy(_path, path);

    /* Iterate the string */
    for (p = _path + 1; *p; p++) {
        if (*p == '/') {
            /* Temporarily truncate */
            *p = '\0';

            if (mkdir(_path, perms) != 0) {
                if (errno != EEXIST)
                    return -1;
            }

            *p = '/';
        }
    }

    if (mkdir(_path, perms) != 0) {
        if (errno != EEXIST)
            return -1;
    }

    return 0;
}

/**
 * Checks the number of consecutive motion or non-motion frames.  We look for 30 consecutive frames to try
 * and avoid trigger lots of events with only a couple frames in each.
 *
 * If more than 30 consecutive motion frames, we are 'in event'.
 * If more than 30 consecutive non-motion frames, we clear 'in event', advance the event counter/date time
 *
 * In this function last_count is the number of consecutive frames, + means motion frames, - is non-motion
 * @param true if the current frame had detected motion, otherwise false.
 * @return true if we are i"in event", otherwise false.
 */
bool motion_processor::check_for_new_debug_event(bool md) {
    static int last_count = 0;
    static bool last_md = false;
    static bool in_event = false;
    char date_time[32];

    if (md != last_md)
        last_count = 0;

    int dir = (md ? 1 : -1);
    int cur_count = (last_count + dir > 10 ? 10 : (last_count + dir < -10 ? -10 : last_count + dir));

    if (!in_event && cur_count >= 10) {
        in_event = true;
        strftime(date_time, sizeof(date_time), "%Y/%m/%d - %H:%M:%S", &m_debugEventTime);
        bc_log(Info, "Debug motion event has started: %s", date_time);
    } else if (in_event && cur_count <= -10) {
        time_t now = time(NULL);
        localtime_r(&now, &m_debugEventTime);

        //std::time_t now = std::time(NULL);
        //&m_debugEventTime = std::localtime(&now);
        //std::strftime(&m_debugEventTime, sizeof(&m_debugEventTime), "%Y-%m-%d/%H.%M.%S", ptm);

        m_debugEventNum++;
        m_debugFrameNum = 0;
        in_event = false;
        strftime(date_time, sizeof(date_time), "%Y/%m/%d - %H:%M:%S", &m_debugEventTime);
        bc_log(Info, "Debug motion event has ended: %s", date_time);
    }

    last_md = md;
    last_count = cur_count;
    return in_event;
}

void motion_processor::dump_opencv_frame(cv::Mat &m, const char *name)
{
    char date[24], stime[24], fname[128];

    if (m_debugEventTime.tm_sec < 0) {
        time_t now = time(NULL);
        localtime_r(&now, &m_debugEventTime);
        //std::time_t now = std::time(NULL);
        //m_debugEventTime = std::localtime(&now);
    }

    bc_get_media_loc(fname, sizeof(fname));
    strftime(date, sizeof(date), "%Y/%m/%d", &m_debugEventTime);
    strftime(stime, sizeof(stime), "%H-%M-%S", &m_debugEventTime);

    // append to the end of fname to get the entire pathname
    snprintf(fname + strlen(fname), sizeof(fname)-1, "/%s/%06d/%s.debug", date, m_recorder->id, stime);
    mkdir_p(fname, S_IRWXU | S_IRWXG | S_IRWXO);

    // append again to the end of fname to get the absolute filename
    sprintf(fname + strlen(fname),"/%06d.%s%s.jpg", m_debugFrameNum, name, (m_motionTriggered ? "-t" : "-p"), name);
    if (!imwrite(fname, m))
            bc_log(Error, "Failed to save %s Frame", name);
}

void motion_processor::dump_opencv_composite(cv::Mat red, cv::Mat green, cv::Mat blue, const char *name)
{
    // render all the contours (or perhaps just deltaFrame) in Red, onto the original greay curFrame, and output to file.
    cv::Mat colorized;
    std::vector<cv::Mat> channels = { blue, green, red };
    cv::merge(channels, colorized);
    dump_opencv_frame(colorized, name);
}

int motion_processor::match_ref_frame_opencv(cv::Mat &curFrame, cv::Mat &refFrame, int w, int h)
{
	cv::Mat m, deltaFrame, blobFrame;
	cv::absdiff(refFrame, curFrame, deltaFrame);

    //dump_opencv_frame(deltaFrame, "delta",  &dbg_cnt4);
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 24; j++) {
            cv::Mat cell(deltaFrame, cv::Range(h / 24 * j, h / 24 * (j + 1)), cv::Range(w / 32 * i, w / 32 * (i + 1)));
            //according to OpenCV docs, changes to "cell" subarrays are reflected in deltaFrame, no copy-on-write is performed
            cv::threshold(cell, cell, thresholds[j * 32 + i], 255, cv::THRESH_BINARY);
        }
    }

    //dump_opencv_frame(deltaFrame, "threshold",  &dbg_cnt4);
	int iterations = 5;
	cv::dilate(deltaFrame, blobFrame, cv::Mat(), cv::Point(-1,-1), iterations);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(blobFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    //dump_opencv_frame(deltaFrame, "contour",  &dbg_cnt4);

    // the old way was looking for the largest blob and comparing against thresholds...
    // the new way sums the area of all blobs and compares the total against thresholds...
    double motionArea = 0.;
    double maxArea = w * h;

	for( int i = 0; i < (int)contours.size(); i++) {
        motionArea += cv::contourArea(contours[i]);
	}

    float motionPercent =  motionArea / maxArea;
    if (motionPercent >= m_minMotionAreaPercent) {
        if (m_motionDebug) {
            bc_log(Info, "Motion-detect contour area is %0.1f%% of frame.", motionPercent * 100.);
            cv::Mat bgChannel = curFrame-deltaFrame; // do this op once, but we'll use the result twice
            dump_opencv_composite(curFrame, bgChannel, bgChannel, "motion");
        }
        // don't trigger event if the motion has saturated the majority of the frame.
        return (motionPercent <= m_maxMotionAreaPercent);
    }

	return 0;
}

int motion_processor::convert_AVFrame_to_grayMat(AVFrame *srcFrame, cv::Mat &dstFrame)
{
    int dst_w = dstFrame.size().width;
    int dst_h = dstFrame.size().height;

    if (dst_w < 32 || dst_h < 24)
    {
        bc_log(Error, "Frame size is too small for motion detection! Increase frame downscale factor if it applies");
        return 0;
    }

    convContext = sws_getCachedContext(convContext, srcFrame->width, srcFrame->height,
                                       fix_pix_fmt(srcFrame->format), dst_w, dst_h,
                                       AV_PIX_FMT_GRAY8, SWS_FAST_BILINEAR, NULL, NULL, NULL);

    // allocation a new AV frame of the new size/color type. But, we'll use the cv::Mat as the pixel holder
    AVFrame *frame = av_frame_alloc();
    frame->format = AV_PIX_FMT_GRAY8;
    frame->width = dst_w;
    frame->height = dst_h;

    // setup and pointers, and tmp AVFrame for scaling the src image into
    av_image_fill_arrays(frame->data, frame->linesize, dstFrame.data, AV_PIX_FMT_GRAY8, dst_w, dst_h, 1);

    // do the copy/scale
    sws_scale(convContext, (const uint8_t **)srcFrame->data, srcFrame->linesize, 0, srcFrame->height,
              frame->data, frame->linesize);

    av_frame_free(&frame);

    return 1;
}

int motion_processor::detect_opencv(AVFrame *rawFrame)
{
	int ret = 0;
    int dst_h = rawFrame->height * m_downscaleFactor;
    int dst_w = rawFrame->width * m_downscaleFactor;

    // first things first... get the original frame from the AVFrame and convert it to an OpenCV Grayscale Mat, and the "downscale" size.
    cv::Mat m = cv::Mat(dst_h, dst_w, CV_8UC1);
    if (convert_AVFrame_to_grayMat(rawFrame, m) == 0) {
        return 0; // error converting frame...
    }

	cv::GaussianBlur(m, m, cv::Size(21, 21), 0);

	//match every 2nd reference frame
	if (!m_refFrames[0].empty() && m_refFrames[0].rows == dst_h && m_refFrames[0].cols == dst_w && m_refFrameUpdateCounters[0] != 0)
	{
		ret = this->match_ref_frame_opencv(m, m_refFrames[0], dst_w, dst_h);

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

	if (ret == 0 && !m_refFrames[1].empty() && m_refFrames[1].rows == dst_h && m_refFrames[1].cols == dst_w && m_refFrameUpdateCounters[1] != 0)
	{
		ret = this->match_ref_frame_opencv(m, m_refFrames[1], dst_w, dst_h);

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

	if (m_motionDebug)
        check_for_new_debug_event(ret);
	return ret;
}

int motion_processor::detect_opencv_advanced(AVFrame *rawFrame)
{
    int ret = 0;
    int dst_h = rawFrame->height * m_downscaleFactor;
    int dst_w = rawFrame->width * m_downscaleFactor;

    // first things first... get the original frame from the AVFrame and convert it to an OpenCV Grayscale Mat, and the "downscale" size.
    cv::Mat m = cv::Mat(dst_h, dst_w, CV_8UC1);
    if (convert_AVFrame_to_grayMat(rawFrame, m) == 0) {
        return 0; // error converting frame...
    }

    // do a quick(-ish) blur to reduce noise...
    cv::GaussianBlur(m, m, cv::Size(21, 21), 0);

    if (m_refFrames[0].empty() || m_refFrames[0].rows != dst_h || m_refFrames[0].cols != dst_w) {
        m_refFrames[0] = m;
        bc_log(Debug, "opencv motion detection - setting reference frame");
    } else {
        ret = match_ref_frame_opencv(m, m_refFrames[0], dst_w, dst_h);
        cv::addWeighted(m_refFrames[0], 1.0 - m_motionBlendRatio, m, m_motionBlendRatio, 0.0,  m_refFrames[0]);
    }

    if (m_motionDebug)
        check_for_new_debug_event(ret);

    // every frame, update our temporal counters
    md_frame_pool_index = (md_frame_pool_index + 1) % m_maxMotionFrames;
    md_frame_pool[md_frame_pool_index] = ret;

    int num_md_frames = 0;
    for (int i = 0; i < m_maxMotionFrames; i++)
        num_md_frames += md_frame_pool[i];

    if (num_md_frames >= m_minMotionFrames) {
        if (!m_motionTriggered) {
            bc_log(Info, "OpenCV::Temporal detected %d frames of motion. Motion Triggered.", num_md_frames);
        }
    } else if (m_motionTriggered) {
        // we are between trigger events...
        bc_log(Info, "OpenCV::Temporal motion event has lapsed.");
    }

    m_motionTriggered = (num_md_frames >= m_minMotionFrames);
    return m_motionTriggered;
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
    dump_opencv_frame()
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
