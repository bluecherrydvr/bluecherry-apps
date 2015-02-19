#ifndef TRIGGER_PROCESSOR_H
#define TRIGGER_PROCESSOR_H

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
class trigger_processor : public stream_consumer
{
public:
	trigger_processor(int camera_id);
	virtual ~trigger_processor();

	void destroy();
	void run();

	virtual void receive(const stream_packet &packet) override;

	void trigger(const char *description);

private:
	bool destroy_flag;
	int camera_id;

	pthread_mutex_t trigger_flag_lock;
	bool trigger_flag;
};

#endif
