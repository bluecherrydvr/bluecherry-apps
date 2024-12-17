#ifndef MOTION_PROCESSOR_H
#define MOTION_PROCESSOR_H

#include "stream_elements.h"
#include "opencv2/opencv.hpp"
#include "bc-server.h"

//#define DEBUG_DUMP_MOTION_DATA

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
		CV_BASIC,
		CV_MULTIFRAME
	};

	motion_processor(bc_record *bcRecord);
	virtual ~motion_processor();

	int set_motion_thresh(const char *map, size_t size);
	int set_motion_thresh_global(char value);

	void set_motion_algorithm(int algo);
	int set_frame_downscale_factor(double f);
	int set_min_motion_area_percent(int p);
    int set_max_motion_area_percent(int p);
    int set_max_motion_frames(int max);
    int set_min_motion_frames(int min);
    int set_motion_blend_ratio(int ratio);
    int set_motion_debug(int debug);

	void destroy();
	void run();

	//void start_thread();

private:
	AVCodecContext *decode_ctx;
	bool destroy_flag;

	struct SwsContext *convContext;
	AVFrame           *refFrame;
	int64_t            last_tested_pts;
	uint8_t            skip_count;
	uint8_t            thresholds[768];
	uint8_t            md_frame_pool[255]; //!< This is basically a ring-buffer of the last n frames, and whether that frame detected motion
	uint8_t            md_frame_pool_index; //!< our current place in the pool

	bool decode_create(const stream_properties &prop);
	void decode_destroy();
	int detect(AVFrame *frame);

	int detect_bc_original(AVFrame *frame);
	int detect_opencv(AVFrame *frame);
    int detect_opencv_advanced(AVFrame *frame);

    bool check_for_new_debug_event(bool md);
    void dump_opencv_frame(cv::Mat &m, const char *name);
    void dump_opencv_composite(cv::Mat red, cv::Mat green, cv::Mat blue, const char *name);

    int convert_AVFrame_to_grayMat(AVFrame *srcFrame, cv::Mat &dstFrame);
    int match_ref_frame_opencv(cv::Mat &curFrame, cv::Mat &refFrame, int w, int h);
	detection_algorithm m_alg;

	pthread_t m_thread;
	cv::Mat m_refFrames[2];
	int m_refFrameUpdateCounters[2];
	double m_downscaleFactor;
	float m_minMotionAreaPercent;
    float m_maxMotionAreaPercent;
    int m_maxMotionFrames;
    int m_minMotionFrames;
    float m_motionBlendRatio;
    bool m_motionTriggered = false;

    bc_record *m_recorder;

    bool m_motionDebug = true;
    int m_debugEventNum = 0;
    uint64_t m_debugFrameNum = 0;
    struct tm m_debugEventTime;

	static void thread_cleanup(void *data);
};

#endif
