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

protected:
	std::string file_path;
	AVFormatContext *oc;
	AVStream *video_st, *audio_st;
	int64_t output_pts_base;
};

#endif

