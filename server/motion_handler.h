#ifndef MOTION_HANDLER_H
#define MOTION_HANDLER_H

#include "libbluecherry.h"
#include "stream_elements.h"

/* Motion_handler buffers input packets and uses motion detection results to
 * decide what to forward downstream for recording. This does not perform motion
 * detection; instead, it's responsible for taking raw motion flags on packets and
 * implementing prerecording, postrecording, event continuation, packet-count
 * thresholds, etc.
 *
 * For some devices (e.g. solo), motion flags can be set directly from the input,
 * and the above behaviors are done directly with the input stream. However, in cases
 * where motion detection is performed asynchronously from the input and possibly on
 * a limited subset of packets, this class also accepts a second stream of packets
 * with motion data added. That stream - which is expected to be a subset of the
 * buffer, is interleaved with the buffer to determine what to record, without
 * subjecting the recording to the same packet drop rules as the actual motion
 * detection. The goal is that motion detection can drop packets as needed to stay
 * realtime, but the recording (if any) will include all packets within the given
 * timeframe.
 */
class motion_handler : public stream_source
{
public:
	motion_handler();
	virtual ~motion_handler();

	/* Prerecord is the minimum amount of time in a recording before the frame
	 * that triggered motion, specified in seconds. A buffer is kept of at least
	 * this many seconds (rounded up to a keyframe), and dumped at the start of a
	 * recording.
	 *
	 * Postrecord is how long after the last motion the recording will continue,
	 * also in seconds. Excessively low values will create too many recordings for
	 * intermittent motion.
	 *
	 * In addition, a motion recording is not closed until the last frame from the
	 * recording has fallen out of the prerecord buffer. If more motion is detected
	 * in that time, the intervening frames are dumped from the prerecord buffer and
	 * recording continues. If there is no motion, the recording is ended. This
	 * continuation helps avoid excessive recordings.
	 */
	void set_buffer_time(int prerecord, int postrecord);

	/* This stream should be connected to the raw input stream, and is used to buffer
	 * data for recording. If no flag_consumer is used, it will also be expected for
	 * packets to be flagged with motion. */
	stream_consumer *input_consumer();

	/* Optionally, this stream can be connected to another element which provides a
	 * subset of the input stream with motion flags added. This allows asynchronous
	 * and low-framerate motion processing without affecting the recording itself.
	 * These flags are correlated with the buffer from input_consumer to make recording
	 * decisions. */
	stream_consumer *create_flag_consumer();

	void disconnect();
	void destroy();
	void run();

private:
	class raw_input_consumer *raw_stream;
	stream_consumer *flag_stream;
	bool destroy_flag;
	int prerecord_time;
	int postrecord_time;
};

#endif

