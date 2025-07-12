#include "stream_elements.h"

stream_source::stream_source(const char *n)
	: name(n),
	thread_name({0,})
{
}

stream_source::~stream_source()
{
	std::lock_guard<std::mutex> l(lock);
	for (auto it = children.begin(); it != children.end(); it++) {
		bc_log(Bug, "stream_source %s has clients at destruction, may cause crashes", name);
		(*it)->disconnected(this);
	}
}

void stream_source::send(const stream_packet &packet)
{
	std::lock_guard<std::mutex> l(lock);
	send_buffer.add_packet(packet);
	// XXX deadlock potential? receive() locks as well.
	for (auto it = children.begin(); it != children.end(); it++)
		(*it)->receive(packet);
}

void stream_source::connect(stream_consumer *child, SetupMode mode)
{
	child->connected(this);

	std::lock_guard<std::mutex> l(lock);
	auto ok = children.insert(child);
	// Stop if the element already existed in the set
	if (!ok.second)
		return;

	if (mode == StartFromLastKeyframe) {
		for (auto it = send_buffer.begin(); it != send_buffer.end(); it++)
			child->receive(*it);
	}
}

void stream_source::disconnect(stream_consumer *child)
{
	child->disconnected(this);

	std::lock_guard<std::mutex> l(lock);
	children.erase(child);
}

stream_consumer::stream_consumer(const char *n)
	: name(n), output_source(0), connected_source(0),
	thread_name({0,})
{
	buffer.set_enforce_keyframe(false);
}

stream_consumer::~stream_consumer()
{
	disconnect();
}

void stream_consumer::receive(const stream_packet &packet)
{
	std::unique_lock<std::mutex> l(lock);
	buffer.add_packet(packet);
	l.unlock();

	buffer_wait.notify_one();
}

void stream_consumer::disconnect()
{
	// It's assumed that the source will not disappear during this function.
	// That is only safe if the caller is aware and controls source.
	if (connected_source)
		connected_source->disconnect(this);
}

void stream_consumer::connected(stream_source *source)
{
	if (connected_source) {
		// This can happen during rapid schedule changes
		// Disconnect from the old source first
		bc_log(Warning, "Consumer %s was already connected to %s, disconnecting before connecting to %s", 
		       name, connected_source->name, source->name);
		connected_source->disconnect(this);
	}
	connected_source = source;
}

void stream_consumer::disconnected(stream_source *source)
{
	if (source != connected_source) {
		// This can happen during rapid schedule changes or cleanup
		// Log as warning instead of bug since it's a known race condition
		bc_log(Warning, "Disconnected from source %s but was connected to %s - this may indicate a race condition during schedule changes", 
		       source ? source->name : "null", 
		       connected_source ? connected_source->name : "null");
		return;
	}

	connected_source = 0;
}

