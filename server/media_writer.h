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

	/* Control monotonicity of timestamps we feed to muxer, the same way as in ffmpeg_mux.c in ffmpeg.*/
	int64_t last_mux_dts[2] = {0, 0};
	
	/* Track logging state to prevent spam */
	bool frame_rate_warned[2] = {false, false};
	bool timestamp_gap_warned[2] = {false, false};
	time_t last_timestamp_warning[2] = {0, 0};
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

