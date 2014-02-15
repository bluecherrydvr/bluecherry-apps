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

	/* Save a JPG snapshot of a frame at file_path. */
	static int snapshot(const char *outfile, const stream_packet &pkt);

protected:
	std::string file_path;
	AVFormatContext *oc;
	AVStream *video_st, *audio_st;
	/* Base PTS value for the start of the recording. This assumes that all streams
	 * share a time_base of AV_TIME_BASE, and 0 represents the same instant across all
	 * streams. This is set automatically by the first written packet. */
	int64_t output_pts_base;

	static int decode_one_packet(const stream_packet &pkt, AVFrame *frame);
};

#endif

