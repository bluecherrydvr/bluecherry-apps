#include "libbluecherry.h"
#include <cstdatomic>

input_device::input_device()
	: _audio_enabled(false)
{
}

input_device::~input_device()
{
}

void input_device::set_audio_enabled(bool v)
{
	_audio_enabled = v;
}

stream_packet::stream_packet()
	: size(0), flags(0), pts(AV_NOPTS_VALUE), type(-1), ts_clock(0), d(0)
{
}

stream_packet::stream_packet(const uint8_t *data)
	: size(0), flags(0), pts(AV_NOPTS_VALUE), type(-1), ts_clock(0), d(new stream_packet_data(data))
{
}

stream_packet::stream_packet(const stream_packet &o)
	: size(o.size), flags(o.flags), pts(o.pts), type(o.type), ts_clock(o.ts_clock), d(o.d)
{
	if (d)
		d->ref();
}

stream_packet &stream_packet::operator=(const stream_packet &o)
{
	size = o.size;
	flags = o.flags;
	pts = o.pts;
	type = o.type;
	ts_clock = o.ts_clock;

	if (d)
		d->deref();
	d = o.d;
	if (d)
		d->ref();

	return *this;
}

stream_packet::~stream_packet()
{
	if (d)
		d->deref();
}

stream_packet_data::stream_packet_data(const uint8_t *x)
	: data(x), r(1)
{
}

stream_packet_data::~stream_packet_data()
{
	delete[] data;
}

void stream_packet_data::ref()
{
	std::atomic_fetch_add(&r, 1);
}

void stream_packet_data::deref()
{
	if (std::atomic_fetch_sub(&r, 1) == 1)
		delete this;
}

