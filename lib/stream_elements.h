#ifndef CHAIN_H
#define CHAIN_H

#include "libbluecherry.h"
#include <set>
#include <mutex>
#include <condition_variable>
#include <string.h>
#include <bsd/string.h>

/* These elements create a primitive chain for distributing packets from one source to
 * many consumers, and potentially for consumers to repackage those packets in a new source.
 *
 * A stream_source maintains a list of connected consumers and, upon calls to send(), will
 * distribute that packet to each consumer. It is threadsafe under limited conditions.
 *
 * stream_consumer receives packets from the connected stream_source, and stores those into
 * a private keyframe-based buffer. Another thread waiting on its condition variable can then
 * be woken to process those packets asynchronously from their source and other consumers.
 *
 * stream_consumer may also have an output, which is an instance of stream_source that is
 * expected to receive packets after processing from that consumer. This can be used to modify,
 * limit, or annotate packets for downstream elements.
 *
 * The threading model used here is very primitive and not ideal; this implementation should
 * be revisited to something more reliable that doesn't require one thread per consumer and
 * very careful deadlock avoidance.
 */

class stream_consumer;

class stream_source
{
public:
	enum SetupMode {
		StartFromLastKeyframe,
		StartFromRealtime
	};

	const char * const name;
	char thread_name[16];

	stream_source(const char *name);
	virtual ~stream_source();

	/* Push a packet downstream from this element to all connected elements;
	 * the push to each consumer will take place synchronously, but consumers are expected
	 * to simply buffer the packet and return immediately. */
	virtual void send(const stream_packet &packet);

	/* Connect consumer to this element, so that all packets sent from this element
	 * will be received by the consumer.
	 *
	 * SetupMode may be StartFromLastKeyframe to prepare the consumer by sending all
	 * packets beginning with the most recent keyframe. If using StartFromRealtime, the
	 * consumer must be able to handle starting from an undecodable position in the stream.
	 *
	 * Threadsafe if the lifetime of stream_source and stream_consumer is guaranteed.
	 */
	void connect(stream_consumer *child, SetupMode mode = StartFromRealtime);
	void disconnect(stream_consumer *child);

	const log_context &logging_context() { return log; }
	void set_logging_context(const log_context &context) { log = context; }
	void set_thread_name(const char *arg) { strlcpy(thread_name, arg, sizeof(thread_name)); }

protected:
	std::set<stream_consumer*> children;
	std::mutex lock;
	stream_keyframe_buffer send_buffer;
	log_context log;
};

class stream_consumer
{
public:
	const char * const name;
	char thread_name[16];
	stream_consumer(const char *name);
	virtual ~stream_consumer();

	/* Called by stream_source to buffer one packet for this consumer. Expected to
	 * return very quickly, as processing happens asynchronously.
	 *
	 * Threadsafe, as long as the consumer is not deleted. */
	virtual void receive(const stream_packet &packet);

	stream_source *output() { return output_source; }

	/* Disconnect from the currently connected source. Threadsafe as long as the source
	 * is guaranteed to still exist. */
	void disconnect();

	/* Check if this consumer is connected to the specified source */
	bool is_connected_to(stream_source *source) const { return connected_source == source; }

	const log_context &logging_context() { return log; }
	void set_logging_context(const log_context &context) { log = context; }
	void set_thread_name(const char *arg) { strlcpy(thread_name, arg, sizeof(thread_name)); }

protected:
	/* Lock protecting buffer and buffer_wait */
	std::mutex lock;
	/* Condition triggered when a packet is received and added to the buffer */
	std::condition_variable buffer_wait;
	/* Keyframe-based buffer for packets received by this element; default behavior
	 * is to start from any frame (including non-keyframes) and keep exactly one keyframe
	 * interval buffered. Implementations may remove packets after processing. */
	stream_keyframe_buffer buffer;
	stream_source *output_source;
	stream_source *connected_source;
	log_context log;

	friend class stream_source;
	virtual void connected(stream_source *source);
	virtual void disconnected(stream_source *source);
};

#endif

