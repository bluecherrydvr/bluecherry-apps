/*
 * Copyright (C) 2012 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __LIBBLUECHERRY_INPUTDEVICE_H
#define __LIBBLUECHERRY_INPUTDEVICE_H

#include <string>
#include <vector>
#include <deque>
#include <memory>

/* GCC starting with 4.5.x ships <atomic>, while older versions use <cstdatomic> */
#if __GNUC__ == 4 && __GNUC_MINOR__ < 5
#include <cstdatomic>
#else
#include <atomic>
#endif

class stream_packet;
class stream_properties;

class input_device
{
public:
	input_device();
	virtual ~input_device();

	virtual int start() = 0;
	virtual void stop() = 0;
	virtual bool is_started() { return _started; }

	const char *get_error_message() const { return _error_message.c_str(); }

	virtual int read_packet() = 0;
	virtual const stream_packet &packet() const = 0;

	virtual bool has_audio() const = 0;
	bool audio_enabled() const { return _audio_enabled; }
	virtual void set_audio_enabled(bool enabled);

	/* Get the current stream properties; this is a temporary hack for streaming
	 * and should not be relied on. Properties are included as a field with every
	 * packet, and can be reliably updated that way. */
	std::shared_ptr<const stream_properties> properties() const { return current_properties; }

protected:
	bool _audio_enabled, _started;
	std::string _error_message;
	unsigned next_packet_seq;
	std::shared_ptr<stream_properties> current_properties;

	void set_error_message(const std::string &msg) { _error_message = msg; }
};

/* Properties of the media held by stream_packets. This is roughly equivalent
 * to AVCodecContext for the video and audio (if applicable) streams, and is used
 * for recording and decoding.
 *
 * An instance of this class is associated with each stream_packet, which will
 * usually be shared for all packets from the same input. The contents are immutable.
 * If the input properties change, a new instance will be created and sent with all
 * packets from where it began to apply.
 */
class stream_properties
{
public:
	struct video_properties {
		enum CodecID codec_id;
		enum PixelFormat pix_fmt;
		int width;
		int height;
		/* Note that this is codec time_base; you should NOT use it other than
		 * setting on an AVCodecContext. See stream_packet doc. */
		AVRational time_base;
		int profile;
		std::vector<char> extradata;

		video_properties();
		void apply(AVCodecContext *cc) const;
	} video;

	struct audio_properties {
		enum CodecID codec_id;
		int bit_rate;
		int sample_rate;
		enum AVSampleFormat sample_fmt;
		int channels;
		AVRational time_base;
		int profile;
		std::vector<char> extradata;

		audio_properties();
		void apply(AVCodecContext *cc) const;
	} audio;

	bool has_audio() const { return audio.codec_id != CODEC_ID_NONE; }
};

/* Reference counted container for the data in a stream_packet.
 * Expects a pointer allocated with new[], which will be freed
 * along with this instance when the reference count is zero. */
class stream_packet_data
{
public:
	const uint8_t *data;
	std::shared_ptr<stream_properties> properties;

	stream_packet_data(const uint8_t *data, const std::shared_ptr<stream_properties> &properties);

	void ref();
	void deref();

private:
	std::atomic<int> r;

	~stream_packet_data();
};

/* Represents one packet/frame of media data, in an undefined
 * format. stream_packet is created by the input device and passed
 * to consumers, where it can be buffered and handled. The underlying
 * data is constant and shared to reduce copying.
 *
 * The contents of a stream_packet are entirely situation-dependant.
 * It may contain encoded or decoded video or audio in any format, or
 * any other type as needed. Refer to the source of the packet for
 * usage.
 *
 * A stream_packet instance is not threadsafe, but the underlying data
 * is. You may have stream_packet instances referring to the same data
 * existing on separate threads. The data will be atomically deleted
 * only when no instances refer to it.
 */
class stream_packet
{
public:
	enum Flags {
		NoFlags = 0,
		KeyframeFlag = 0x1,
		MotionFlag = 0x2
	};

	unsigned size;
	unsigned flags;
	/* PTS is normalized to AV_TIME_BASE_Q and synchronized across media. */
	int64_t  pts;
	int      type; // AVMEDIA_TYPE_VIDEO or AVMEDIA_TYPE_AUDIO
	time_t   ts_clock; // Used for prerecord event start time
	/* XXX: Monotonic clock time when this packet was captured. This
	 * is used in place of PTS for buffer management (see
	 * stream_keyframe_buffer), but should be deprecated once we have
	 * reliable PTS values to use. */
	time_t   ts_monotonic;
	/* Sequence number of this packet, unique to the input that created
	 * it. Should not be reset unless the handle is stopped and started
	 * again. Rollover is possible and should be handled. */
	unsigned seq;

	stream_packet();
	stream_packet(const uint8_t *data, const std::shared_ptr<stream_properties> &properties);
	stream_packet(const stream_packet &o);
	stream_packet &operator=(const stream_packet &o);
	~stream_packet();

	const uint8_t *data() const { return d ? d->data : 0; }
	std::shared_ptr<const stream_properties> properties() const { return d ? d->properties : std::shared_ptr<stream_properties>(); }

	bool is_key_frame() const { return flags & KeyframeFlag; }
	bool is_video_frame() const { return type == AVMEDIA_TYPE_VIDEO; }

private:
	stream_packet_data *d;
};

/* Simple FIFO-style queue of stream_packet instances. The buffer is
 * unbounded and should be used directly only with great care.
 * Refer to subclasses for implementations with limits on the size of
 * the buffer. */
class stream_buffer : public std::deque<stream_packet>
{
public:
	/* Returns true if the packet was added to the buffer, or false
	 * when accepts_packet was false. Automatically triggers
	 * apply_bound() as well. */
	bool add_packet(const stream_packet &packet);

	/* Implemented in subclasses to decide whether a packet should
	 * be added to the buffer when given to add_packet. Safe to call
	 * from external code. */
	virtual bool accepts_packet(const stream_packet &packet);

protected:
	virtual void apply_bound();

	using std::deque<stream_packet>::insert;
	using std::deque<stream_packet>::push_back;
	using std::deque<stream_packet>::push_front;
};

/* Implementation of stream_buffer which always begins with a video
 * keyframe, and optionally keeps at least enough frames to cover a
 * given duration. Frames are dropped from the beginning when the
 * next keyframe can satisfy the duration requirement.
 */
class stream_keyframe_buffer : public stream_buffer
{
public:
	stream_keyframe_buffer();

	/* Minimum buffer time in seconds, as counted on a monotonic
	 * clock (stream_packet.ts_monotonic). The buffer will be trimmed
	 * when the duration exceeds this value, but will always start
	 * with a keyframe from at least duration seconds ago.
	 *
	 * XXX This should be refactored to use PTS once we have reliable
	 * and consistent PTS values in stream_packet. */
	unsigned duration() const { return mDuration; }
	void set_duration(unsigned duration);

	/* When the buffer is empty, drop all non-video non-keyframe packets
	 * to ensure that the first packet is always a video keyframe. */
	virtual bool accepts_packet(const stream_packet &packet);

	bool enforce_keyframe() const { return mEnforced; }
	void set_enforce_keyframe(bool enforced);

protected:
	unsigned mDuration;
	bool mEnforced;

	virtual void apply_bound();
};

#endif
