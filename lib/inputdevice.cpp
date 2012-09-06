#include "libbluecherry.h"
#include <cstdatomic>

input_device::input_device()
	: _audio_enabled(false), next_packet_seq(0) 
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
	: size(0), flags(0), pts(AV_NOPTS_VALUE), type(-1), ts_clock(0), ts_monotonic(0),
	  seq(0), d(0)
{
}

stream_packet::stream_packet(const uint8_t *data)
	: size(0), flags(0), pts(AV_NOPTS_VALUE), type(-1), ts_clock(0), ts_monotonic(0),
	  seq(0), d(new stream_packet_data(data))
{
}

stream_packet::stream_packet(const stream_packet &o)
	: size(o.size), flags(o.flags), pts(o.pts), type(o.type), ts_clock(o.ts_clock),
	  ts_monotonic(o.ts_monotonic), seq(0), d(o.d)
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
	ts_monotonic = o.ts_monotonic;
	seq = o.seq;

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

void stream_buffer::apply_bound()
{
}

bool stream_buffer::accepts_packet(const stream_packet &packet)
{
	return true;
}

bool stream_buffer::add_packet(const stream_packet &packet)
{
	if (!accepts_packet(packet))
		return false;
	push_back(packet);
	apply_bound();
	return true;
}

stream_keyframe_buffer::stream_keyframe_buffer()
	: mDuration(0)
{
}

void stream_keyframe_buffer::set_duration(unsigned v)
{
	mDuration = v;
	apply_bound();
}

bool stream_keyframe_buffer::accepts_packet(const stream_packet &packet)
{
	if (empty() && (!packet.is_video_frame() || !packet.is_key_frame()))
		return false;
	return true;
}

void stream_keyframe_buffer::apply_bound()
{
	if (empty())
		return;

	time_t last_ts = back().ts_monotonic;
	if (last_ts < mDuration)
		return;
	last_ts -= mDuration;

	/* Find the last video keyframe with a timestamp equal or greater
	 * to last_ts, and erase everything before it.
	 * Intentionally avoids the first packet. */
	for (auto it = end()-1; it > begin(); it--) {
		if (!it->is_video_frame() || !it->is_key_frame())
			continue;

		if (it->ts_monotonic > last_ts)
			continue;

		erase(begin(), it);
		break;
	}
}

