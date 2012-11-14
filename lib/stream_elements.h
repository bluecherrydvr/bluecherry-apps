#ifndef CHAIN_H
#define CHAIN_H

#include "libbluecherry.h"
#include <set>
#include <mutex>
#include <condition_variable>

class stream_consumer;

class stream_source
{
public:
	enum SetupMode {
		StartFromLastKeyframe,
		StartFromRealtime
	};

	const char * const name;

	stream_source(const char *name);
	virtual ~stream_source();

	/* Push a packet downstream from this element to all connected elements */
	virtual void send(const stream_packet &packet);

	/* Connect child element to this element, so that all packets sent from this element
	 * will be received by the child. */
	void connect(stream_consumer *child, SetupMode mode = StartFromRealtime);
	void disconnect(stream_consumer *child);

protected:
	std::set<stream_consumer*> children;
	std::mutex lock;
	stream_keyframe_buffer send_buffer;
};

class stream_consumer
{
public:
	const char * const name;
	stream_consumer(const char *name);
	virtual ~stream_consumer();

	/* Threadsafe. Called by source. */
	virtual void receive(const stream_packet &packet);

	stream_source *output() { return output_source; }

	void disconnect();

protected:
	std::mutex lock;
	std::condition_variable buffer_wait;
	stream_keyframe_buffer buffer;
	stream_source *output_source;
	stream_source *connected_source;

	friend class stream_source;
	virtual void connected(stream_source *source);
	virtual void disconnected(stream_source *source);
};

#endif

