#ifndef MEDIA_WRITER_H
#define MEDIA_WRITER_H

#include <string>

#include "libbluecherry.h"

struct AVFormatContext;
struct AVStream;

class media_writer
{
public:
	media_writer();
	~media_writer();

	int open(const std::string &file_path, const stream_properties &properties);
	void close();

	bool write_packet(const stream_packet &pkt);

	/* TODO Move snapshotting & decoding functions to separate class */
	/* Save a JPG snapshot of a frame at file_path. */
	int snapshot_create(const char *outfile, const stream_packet &pkt);
	int snapshot_feed(const stream_packet &pkt);


protected:
	std::string file_path;
	AVFormatContext *oc;
	AVStream *video_st, *audio_st, *subs_st;
	AVCodecContext *snapshot_decoder;
	std::string snapshot_filename;
	int64_t last_video_pts, last_video_dts, last_audio_pts, last_audio_dts;
	/* Base PTS value for the start of the recording. This assumes that all streams
	 * share a time_base of AV_TIME_BASE, and 0 represents the same instant across all
	 * streams. This is set automatically by the first written packet. */

	int snapshot_decoder_init(const stream_packet &pkt);
	int snapshot_decode_frame(const stream_packet &pkt, AVFrame **frame);
	int snapshot_encode_write(AVFrame *frame);
	void snapshot_decoder_cleanup(void);
};

#endif

