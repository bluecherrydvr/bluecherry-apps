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
	AVFormatContext *out_ctx = NULL;
	AVStream *video_st = NULL;
	AVStream *audio_st = NULL;
	std::string recording_path;

	/* Base PTS value for the start of the recording. This assumes that all streams
	 * share a time_base of AV_TIME_BASE, and 0 represents the same instant across all
	 * streams. This is set automatically by the first written packet. */
	int64_t last_video_pts = 0;
	int64_t last_video_dts = 0;
	int64_t last_audio_pts = 0;
	int64_t last_audio_dts = 0;
};

class snapshot_writer
{
public:
	snapshot_writer(const char *outfile);
	~snapshot_writer();

	int feed(const stream_packet &pkt);

protected:
	AVFormatContext *out_ctx = NULL;
	AVCodecContext *decoder = NULL;
	AVCodecContext *encoder = NULL;

	FILE *output_file = NULL;
	std::string output_path;

	int init_encoder(int width, int height);
	int init_decoder(const stream_packet &pkt);
	int push_packet(const stream_packet &pkt);

	AVFrame* scale_frame(AVFrame *rawFrame, bool &allocated);
	int write_frame(AVFrame *frame);

	void cleanup(bool flush = false);
	void destroy_encoder(bool flush = false);
	void destroy_decoder(void);
	void destroy_muxer(void);
};

#endif

