#ifndef MOTION_PROCESSOR_H
#define MOTION_PROCESSOR_H

#include "stream_elements.h"

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
	motion_processor(const input_device *input);
	virtual ~motion_processor();

	void destroy();

	void run();

private:
	AVCodecContext *decode_ctx;
	stream_properties input_properties;
	bool destroy_flag;

	struct SwsContext *convContext;
	AVFrame           *refFrame;
	int                refFrameHeight;
	int                refFrameWidth;
	int64_t            last_tested_pts;
	uint8_t            skip_count;
	uint8_t            thresholds[768];

	int detect(AVFrame *frame);
};

#endif

