/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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

#include <pugixml.hpp>

#define BC_BUFFERS		16
#define BC_BUFFERS_JPEG		8

/* Some things that are driver specific */
#ifndef V4L2_BUF_FLAG_MOTION_ON
#define V4L2_BUF_FLAG_MOTION_ON		0x0400
#define V4L2_BUF_FLAG_MOTION_DETECTED	0x0800
#endif

#ifndef V4L2_CID_MOTION_ENABLE
#define V4L2_CID_MOTION_ENABLE		(V4L2_CID_PRIVATE_BASE+0)
#define V4L2_CID_MOTION_THRESHOLD	(V4L2_CID_PRIVATE_BASE+1)
#define V4L2_CID_MOTION_TRACE		(V4L2_CID_PRIVATE_BASE+2)
#endif

/* Camera capability flags */
#define BC_CAM_CAP_V4L2_MOTION 0x00000002
#define BC_CAM_CAP_OSD         0x00000004
#define BC_CAM_CAP_SOLO        0x00000008
#define BC_CAM_CAP_MJPEG_URL   0x00000010
/* Set for all BC_DEVICE_V4L2 handles operating
 * in PAL. NTSC is assumed if unset. */
#define BC_CAM_CAP_V4L2_PAL    0x00000020

class stream_packet;
class stream_properties;

/* input_device provides an abstract interface implemented by device backends, such
 * as v4l2_device and lavf_device. Generally, each bc_handle has an input_device, which
 * while running will be repeatedly polled with read_packet(), which should block until
 * a packet is available. */
class input_device
{
public:
	input_device();
	virtual ~input_device();

	virtual int start() = 0;
	virtual void stop() = 0;
	virtual bool is_started() { return _started; }

	const char *get_error_message() const { return _error_message.c_str(); }

	/* Read one packet from the device, blocking if necessary. Returns zero on
	 * success, non-zero (possibly errno) on failure. On success, packet() will
	 * retrieve the current packet until the next call to read_packet.
	 *
	 * The device must have successfully returned from start() prior to calling
	 * read_packet(). */
	virtual int read_packet() = 0;
	virtual const stream_packet &packet() const = 0;

	virtual bool has_audio() const = 0;
	bool audio_enabled() const { return _audio_enabled; }
	virtual void set_audio_enabled(bool enabled);

	/* Get the current stream properties; this is a temporary hack for streaming
	 * and should not be relied on. Properties are included as a field with every
	 * packet, and can be reliably updated that way.
	 *
	 * This should be removed once RTSP streaming is converted to a proper
	 * stream_consumer. */
	std::shared_ptr<const stream_properties> properties() const { return current_properties; }

	virtual void getStatusXml(pugi::xml_node& xmlnode) = 0;

	virtual int caps() const { return 0; }
	virtual bool has_error() const { return false; }

	virtual int set_resolution(uint16_t width, uint16_t height, uint8_t interval) { return 0; }
	virtual int set_osd(const char *fmt, ...) __attribute__ ((format (printf, 2, 3))) { return 0; }
	virtual int set_mjpeg() { return 0; }
	virtual int set_control(unsigned int ctrl, int val) { return 0; }

	virtual int set_motion(bool on) { return 0; }
	virtual int set_motion_thresh(const char *map, size_t size) { return 0; }
	virtual int set_motion_thresh_global(char value) { return 0; }

protected:
	bool _audio_enabled, _started;
	std::string _error_message;
	/* Incremental counter for packets emitted by this device, which should increment
	 * by exactly one for each successful return from read_packet(), beginning with
	 * zero. Subclass is expected to handle this behavior in the packet creation logic.
	 */
	unsigned next_packet_seq;
	std::shared_ptr<stream_properties> current_properties;

	void set_error_message(const std::string &msg) { _error_message = msg; }
};

/* Properties of the media held by stream_packets. This is roughly equivalent
 * to AVCodecContext for the video and audio (if applicable) streams, and is used
 * for recording and decoding.
 *
 * An instance of this class is associated with each stream_packet, which is usually
 * shared and reference counted for packets from that input_device. The contents are
 * immutable. If input properties change, a new instance of stream_properties is created
 * and sent along with subsequent packets; users are expected to compare to determine
 * when this happens.
 *
 * This behavior is necessary to ensure that property changes are always processed
 * at the correct time, regardless of packet drops or buffering.
 */
class stream_properties
{
public:
	struct video_properties {
		enum AVCodecID codec_id;
		unsigned int codec_tag;

		int bits_per_coded_sample;
		int bits_per_raw_sample;
		int level;
		enum AVFieldOrder field_order;
		enum AVColorRange color_range;
		enum AVColorPrimaries color_primaries;
		enum AVColorTransferCharacteristic color_trc;
		enum AVColorSpace colorspace;
		enum AVChromaLocation chroma_sample_location;
		AVRational sample_aspect_ratio;

		enum AVPixelFormat pix_fmt;
		int width;
		int height;
		/* Note that this is codec time_base; you should NOT use it other than
		 * setting on an AVCodecContext. See stream_packet doc. */
		AVRational time_base;
		int profile;
		std::vector<char> extradata;

		video_properties();
		/* Apply these properties to an allocated AVCodecContext instance,
		 * suitable for creating encoders, decoders, etc. */
		void apply(AVCodecContext *cc) const;
	} video;

	struct audio_properties {
		enum AVCodecID codec_id;
		int bit_rate;
		int sample_rate;
		enum AVSampleFormat sample_fmt;
		int channels;
		AVRational time_base;
		int profile;
		std::vector<char> extradata;
		int bits_per_coded_sample;

		audio_properties();
		/* Apply these properties to an allocated AVCodecContext instance,
		 * suitable for creating encoders, decoders, etc. */
		void apply(AVCodecContext *cc) const;
	} audio;

	bool has_audio() const { return audio.codec_id != AV_CODEC_ID_NONE; }
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
 * any other type as needed. Refer to the source of the packet (probably
 * an input_device) for usage.
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
		MotionFlag = 0x2,
		TriggerFlag = 0x4,
	};

	/* These fields are not shared between stream_packet instances, and
	 * thus are safe to modify, but only will apply to packets copied from
	 * this instance after modification. */

	/* Size in bytes of the data field */
	unsigned size;
	/* Flags, as a bitmask of values from Flags */
	mutable unsigned flags;
	/* Presentation timestamp of this packet, which is normalized during
	 * creation of the packet to AV_TIME_BASE_Q, and synchronized across
	 * related media streams. */
	int64_t  pts;
	/* Decoding timestamp in AV_TIME_BASE_Q. Increases monotonically, except
	 * for AV_NOPTS_VALUE and wrapping. */
	int64_t  dts;
	/* AVMEDIA_TYPE_VIDEO or AVMEDIA_TYPE_AUDIO */
	int      type;
	/* Wallclock time at packet capture. Do not compare this value between packets;
	 * the wall clock is not reliable. Used for event recording times. */
	// TODO Use int64_t and av_gettime_relative() (but check av_gettime_relative_is_monotonic()) or drop
	time_t   ts_clock;
	/* Monotonic clock time at packet capture. This is a reliable value for comparison
	 * between capture of packets, but does not represent actual playback timing.
	 *
	 * Avoid use. Currently used in place of PTS for buffer management to avoid problems
	 * with unreliable PTS values, but capture time does not in any way represent packet
	 * time. */
	// TODO Use int64_t and av_gettime_relative() (but check av_gettime_relative_is_monotonic()) or drop
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
 * unbounded and should only be used directly with great care.
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
	virtual bool accepts_packet(const stream_packet &packet)  __attribute__((const));

protected:
	virtual void apply_bound() __attribute__((const));

	/* Prevent use of direct modification functions */
	using std::deque<stream_packet>::insert;
	using std::deque<stream_packet>::push_back;
	using std::deque<stream_packet>::push_front;
};

/* Implementation of stream_buffer which automatically drops packets
 * with regard to keyframes, to avoid corruption of the stream.
 *
 * Normally, the buffer will begin with one keyframe and contain any
 * subsequent packets, and be overwritten with the next keyframe.
 * If duration is set, the buffer will hold at least that many seconds
 * of packets (by capture time), but still only drop before a keyframe.
 *
 * If enforce_keyframe is true (default), the first packet will always
 * be a keyframe, and thus a safe place to start using the stream. Unset
 * this if taking packets out of the buffer as they're processed.
 *
 * This may be woefully unsuitable for audio.
 */
class stream_keyframe_buffer : public stream_buffer
{
public:
	stream_keyframe_buffer();

	/* Minimum buffer time in seconds, as counted on a monotonic
	 * clock (stream_packet.ts_monotonic). The buffer will be trimmed
	 * when the duration exceeds this value, but will always start
	 * with a keyframe from at least duration seconds ago. */
	unsigned duration() const { return mDuration; }
	void set_duration(unsigned duration);

	/* When the buffer is empty, drop all non-video non-keyframe packets
	 * to ensure that the first packet is always a video keyframe. */
	virtual bool accepts_packet(const stream_packet &packet) __attribute__((pure));

	bool enforce_keyframe() const { return mEnforced; }
	void set_enforce_keyframe(bool enforced);

protected:
	unsigned mDuration;
	bool mEnforced;

	virtual void apply_bound();
};

#endif
