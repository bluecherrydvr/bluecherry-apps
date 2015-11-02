#ifndef MOTION_PROCESSOR_H
#define MOTION_PROCESSOR_H

#include "stream_elements.h"
#include "opencv2/opencv.hpp"

struct AVCodecContext;

/* This element decodes and runs a generic motion detection algorithm on
 * the input stream. It is suitable for detecting motion on any decodable
 * input, particularly generic IP cameras.
 *
 * The output stream from this element contains all processed packets, which
 * due to the buffering rules is guaranteed to be decodable. However, because
 * the process is quite expensive, it's expected that this element may drop
 * a significant portion of packets to cope with load and stay at realtime.
 *
 * As a result, the output stream is not directly suitable for recording or
 * streaming; instead, it should be correlated with a buffered stream and
 * the result of that used instead. This behavior is implemented by
 * motion_handler, which also handles the decisions on what to record in
 * response to motion flags.
 */
class motion_processor : public stream_consumer
{
public:
	enum detection_algorithm
	{
		BC_DEFAULT,
		OPENCV
	};

	motion_processor();
	virtual ~motion_processor();

	int set_motion_thresh(const char *map, size_t size);
	int set_motion_thresh_global(char value);

	void set_motion_algorithm(int algo);
	int set_frame_downscale_factor(double f);
	int set_min_motion_area_percent(int p);

	void destroy();
	void run();

	void start_thread();

private:
	AVCodecContext *decode_ctx;
	bool destroy_flag;

	struct SwsContext *convContext;
	AVFrame           *refFrame;
	int64_t            last_tested_pts;
	uint8_t            skip_count;
	uint8_t            thresholds[768];

	bool decode_create(const stream_properties &prop);
	void decode_destroy();
	int detect(AVFrame *frame);

	int detect_bc_original(AVFrame *frame);
	int detect_opencv(AVFrame *frame);
	detection_algorithm m_alg;

	pthread_t m_thread;
	cv::Mat m_refFrame;
	double m_downscaleFactor;
	int m_minMotionAreaPercent;

	static void thread_cleanup(void *data);
};

#endif
