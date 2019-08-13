#include "libbluecherry.h"

input_device::input_device()
	: _audio_enabled(false), _started(false), next_packet_seq(0) 
{
}

input_device::~input_device()
{
}

void input_device::set_audio_enabled(bool v)
{
	_audio_enabled = v;
}

void input_device::set_subtitles_enabled(bool v)
{
	_subs_enabled = v;
}

void input_device::set_subtitles_text(const char *text)
{
	if (!_subs_enabled)
		return;

	_subs_text = std::string(text);
}

bool input_device::has_new_subs()
{
	if (_subs_enabled && subs_ts != time(NULL)) {
		subs_ts = time(NULL);
		return true;
	}

	return false;
}

std::string input_device::timestamped_sub()
{
	time_t t;
        struct tm tm;
	char timebuf[64];

	tzset();

	t = time(NULL);
	//subs_ts = t;

	strftime(timebuf, sizeof(timebuf), " %F %T", localtime_r(&t, &tm));

	return _subs_text + std::string(timebuf);
}

stream_packet::stream_packet()
	: size(0), flags(NoFlags), pts(AV_NOPTS_VALUE), dts(AV_NOPTS_VALUE), type(-1), ts_clock(0), ts_monotonic(0),
	  seq(0), d(0)
{
}

stream_packet::stream_packet(const uint8_t *data, const std::shared_ptr<stream_properties> &properties)
	: size(0), flags(NoFlags), pts(AV_NOPTS_VALUE), dts(AV_NOPTS_VALUE), type(-1), ts_clock(0), ts_monotonic(0),
	  seq(0), d(new stream_packet_data(data, properties))
{
}

stream_packet::stream_packet(const stream_packet &o)
	: size(o.size), flags(o.flags), pts(o.pts), dts(o.dts), type(o.type), ts_clock(o.ts_clock),
	  ts_monotonic(o.ts_monotonic), seq(o.seq), d(o.d)
{
	if (d)
		d->ref();
}

stream_packet &stream_packet::operator=(const stream_packet &o)
{
	size = o.size;
	flags = o.flags;
	pts = o.pts;
	dts = o.dts;
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

stream_packet_data::stream_packet_data(const uint8_t *x, const std::shared_ptr<stream_properties> &p)
	: data(x), properties(p), r(1)
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
	(void)packet;
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
	: mDuration(0), mEnforced(true)
{
}

void stream_keyframe_buffer::set_duration(unsigned v)
{
	mDuration = v;
	apply_bound();
}

void stream_keyframe_buffer::set_enforce_keyframe(bool v)
{
	mEnforced = v;
}

bool stream_keyframe_buffer::accepts_packet(const stream_packet &packet)
{
	if (mEnforced && empty() && (!packet.is_video_frame() || !packet.is_key_frame()))
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

		/* When not in enforced mode, keep a frame prior to the keyframe
		 * if present, to avoid unnecessary dropping of frames. */
		if (!mEnforced && it == begin()+1)
			break;

		erase(begin(), it);
		break;
	}
}

stream_properties::video_properties::video_properties()
	: codec_id(AV_CODEC_ID_NONE), pix_fmt(AV_PIX_FMT_NONE), width(0), height(0),
	  time_base({ 1, 1 }), profile(FF_PROFILE_UNKNOWN)
{
}

bool stream_properties::operator!=(const stream_properties& other) const
{
	if (video.codec_id != other.video.codec_id
		|| video.pix_fmt != other.video.pix_fmt
		|| video.width != other.video.width
		|| video.height != other.video.height
		|| video.profile != other.video.profile)
		return true;

	if (has_audio() != other.has_audio())
		return true;

	if (has_audio()) {
		if (audio.codec_id != other.audio.codec_id
		|| audio.sample_rate != other.audio.sample_rate
		|| audio.sample_fmt != other.audio.sample_fmt
		|| audio.channels != other.audio.channels
		|| audio.bits_per_coded_sample != other.audio.bits_per_coded_sample)
		return true;
	}

	return false;
}

void stream_properties::video_properties::apply(AVCodecContext *cc) const
{
	cc->codec_id = codec_id;
	cc->codec_type = AVMEDIA_TYPE_VIDEO;
	cc->pix_fmt = pix_fmt;
	cc->width = width;
	cc->height = height;
	cc->time_base = time_base;
	cc->profile = profile;
	if (!extradata.empty()) {
		cc->extradata_size = extradata.size();
		cc->extradata = (uint8_t*)av_malloc(extradata.size() + AV_INPUT_BUFFER_PADDING_SIZE);
		memcpy(cc->extradata, &extradata.front(), extradata.size());
	} else {
		cc->extradata_size = 0;
		cc->extradata = 0;
	}
}

stream_properties::audio_properties::audio_properties()
	: codec_id(AV_CODEC_ID_NONE), bit_rate(0), sample_rate(0), sample_fmt(AV_SAMPLE_FMT_NONE),
	  channels(0), time_base({ 1, 1}), profile(FF_PROFILE_UNKNOWN)
{
}

void stream_properties::audio_properties::apply(AVCodecContext *cc) const
{
	cc->codec_id = codec_id;
	cc->codec_type = AVMEDIA_TYPE_AUDIO;
	cc->bit_rate = bit_rate;
	cc->sample_rate = sample_rate;
	cc->sample_fmt = sample_fmt;
	cc->channels = channels;
	cc->time_base = time_base;
	cc->profile = profile;
	cc->bits_per_coded_sample = bits_per_coded_sample;
	if (!extradata.empty()) {
		cc->extradata_size = extradata.size();
		cc->extradata = (uint8_t*)av_malloc(extradata.size() + AV_INPUT_BUFFER_PADDING_SIZE);
		memcpy(cc->extradata, &extradata.front(), extradata.size());
	} else {
		cc->extradata_size = 0;
		cc->extradata = 0;
	}
}

stream_properties::subtitle_properties::subtitle_properties()
	: codec_id(AV_CODEC_ID_TEXT), time_base({ 1, 1000})
{
	subtitle_header = 
             "[Script Info]\r\n"
             "; Script generated by FFmpeg/Lavc\r\n"
             "ScriptType: v4.00+\r\n"
             "PlayResX: 384\r\n"
             "PlayResY: 288\r\n"
             "\r\n"
             "[V4+ Styles]\r\n"

             /* ASSv4 header */
             "Format: Name, "
             "Fontname, Fontsize, "
             "PrimaryColour, SecondaryColour, OutlineColour, BackColour, "
             "Bold, Italic, Underline, StrikeOut, "
             "ScaleX, ScaleY, "
             "Spacing, Angle, "
             "BorderStyle, Outline, Shadow, "
             "Alignment, MarginL, MarginR, MarginV, "
             "Encoding\r\n"

             "Style: "
             "Default,"             /* Name */
             "Arial,16,"               /* Font{name,size} */
             "&Hffffffff,&Hffffffff,&H00000000,&H00000000," /* {Primary,Secondary,Outline,Back}Colour */
             "0,0,0,0,"          /* Bold, Italic, Underline, StrikeOut */
             "100,100,"             /* Scale{X,Y} */
             "0,0,"                 /* Spacing, Angle */
             "1,1,0,"              /* BorderStyle, Outline, Shadow */
             "2,10,10,10,"         /* Alignment, Margin[LRV] */
             "0\r\n"                /* Encoding */

             "\r\n"
             "[Events]\r\n"
             "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text\r\n";
}

void stream_properties::subtitle_properties::apply(AVCodecContext *cc) const
{
	cc->codec_id = codec_id;
	cc->codec_type = AVMEDIA_TYPE_SUBTITLE;
	cc->time_base = time_base;
	cc->subtitle_header = (uint8_t*) subtitle_header;
	cc->subtitle_header_size = strlen((const char*)cc->subtitle_header);
}

