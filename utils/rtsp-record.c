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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>

static void usage(void)
{
	fprintf(stderr, "Usage: rtsp-record rtsp://[USERINFO@]"
		"SERVER[:PORT]/ output_file.mkv\n\n");
	exit(EXIT_FAILURE);
}

/* Fake H.264 encoder for libavcodec. We're only muxing video, never reencoding,
 * so a real encoder isn't neeeded, but one must be present for the process to
 * succeed. ffmpeg does not support h264 encoding without libx264, which is GPL.
 */

static int fake_h264_init(AVCodecContext *ctx)
{
	return 0;
}

static int fake_h264_close(AVCodecContext *ctx)
{
	return 0;
}

static int fake_h264_frame(AVCodecContext *ctx, uint8_t *buf, int bufsize, void *data)
{
	return -1;
}

AVCodec fake_h264_encoder = {
	.name           = "fakeh264",
	.type           = AVMEDIA_TYPE_VIDEO,
	.id             = AV_CODEC_ID_H264,
	.priv_data_size = 0,
	.init           = fake_h264_init,
	.encode         = fake_h264_frame,
	.close          = fake_h264_close,
	.capabilities   = CODEC_CAP_DELAY,
	.pix_fmts       = (const enum PixelFormat[]) { AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_NONE },
	.long_name      = "Fake H.264 Encoder for RTP Muxing",
};

AVFormatContext *in_ctx;
AVFormatContext *out_ctx;
AVOutputFormat *fmt_out;
AVStream *vst;
int video_stream_index = -1;

void setup_output(const char *file);

int main(int argc, char* argv[])
{
	int opt;
	char *url;

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
		case 'h':
		default:
			usage();
		}
	}

	if (argc <= (optind+1))
		usage();

	url = argv[optind++];

	avcodec_register(&fake_h264_encoder);
	av_register_all();
	av_log_set_level(AV_LOG_DEBUG);

	if (avformat_open_input(&in_ctx, url, NULL, NULL) != 0) {
		fprintf(stderr, "Could not open URI\n");
		exit(1);
	}

	if (avformat_find_stream_info(in_ctx, NULL) < 0) {
		fprintf(stderr, "Could not find stream info\n");
		exit(1);
	}

	av_dump_format(in_ctx, 0, url, 0);

	setup_output(argv[optind]);

	AVPacket packet, opacket;
	int cnt = 0, re;
	while ((re = av_read_frame(in_ctx, &packet)) >= 0) {
		if (packet.stream_index != video_stream_index)
			continue;
		av_init_packet(&opacket);
		opacket.data = packet.data;
		opacket.size = packet.size;
		/* For all output framerate issues, look here. */
		opacket.pts = av_rescale_q(packet.pts, (AVRational){1, 90000}, vst->time_base);
		opacket.flags = packet.flags;
		opacket.stream_index = vst->index;
		if (av_write_frame(out_ctx, &opacket)) {
			fprintf(stderr, "Error writing video frame\n");
		}
		
		if (++cnt == 500)
			break;
	}
	
	printf("End; last return value %d\n", re);
	av_write_trailer(out_ctx);

	exit(0);
}

void setup_output(const char *file)
{
	AVCodec *codec = 0;

	fmt_out = av_guess_format(NULL, file, NULL);
	if (!fmt_out) {
		fprintf(stderr, "Could not guess output format\n");
		exit(1);
	}

	out_ctx = avformat_alloc_context();
	if (!out_ctx) {
		fprintf(stderr, "Could not alloc context\n");
		exit(1);
	}

	out_ctx->oformat = fmt_out;
	snprintf(out_ctx->filename, sizeof(out_ctx->filename), "%s", file);

	vst = avformat_new_stream(out_ctx, 0);
	if (!vst) {
		fprintf(stderr, "Could not add video stream\n");
		exit(1);
	}

	int i;
	for (i = 0; i < in_ctx->nb_streams; ++i) {
		if (in_ctx->streams[i]->codec->codec_type != AVMEDIA_TYPE_VIDEO)
			continue;
			
		printf("Found video codec!\n");
		video_stream_index = i;

		AVCodecContext *ic = in_ctx->streams[i]->codec;
		codec = avcodec_find_encoder(in_ctx->streams[i]->codec->codec_id);
		vst->codec->codec_id = ic->codec_id;
		vst->codec->codec_type = ic->codec_type;
		vst->codec->pix_fmt = ic->pix_fmt;
		vst->codec->width = ic->width;
		vst->codec->height = ic->height;
		vst->codec->time_base = ic->time_base;

		break;
	}

	if (out_ctx->oformat->flags & AVFMT_GLOBALHEADER)
		vst->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

#if 0
	if (rs->tid_a >= 0 || rs->aud_port >= 0) {
		ast = avformat_new_stream(out_ctx, 1);
		if (!ast) {
			fprintf(stderr, "Could not add audio stream\n");
			exit(1);
		}

		ast->codec->codec_id = rs->aud_codec;
		ast->codec->codec_type = CODEC_TYPE_AUDIO;
		ast->codec->bit_rate = rs->bitrate;
		ast->codec->sample_rate = rs->samplerate;
		ast->codec->channels = rs->channels;
		ast->codec->time_base = (AVRational){1, rs->samplerate};

		if (out_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			ast->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
#endif

	if (av_set_parameters(out_ctx, NULL) < 0) {
		fprintf(stderr, "Could not set params\n");
		exit(1);
	}

	if (codec == NULL || avcodec_open(vst->codec, codec) < 0) {
		fprintf(stderr, "Could not open video encoder\n");
		exit(1);
	}

	if (avio_open(&out_ctx->pb, file, AVIO_FLAG_WRITE) < 0) {
		fprintf(stderr, "Could not open outfile\n");
		exit(1);
	}

	av_write_header(out_ctx);
}
