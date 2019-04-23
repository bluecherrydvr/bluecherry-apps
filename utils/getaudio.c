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
#include <string.h>
#include <unistd.h>

#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <alsa/asoundlib.h>

#include "g723-dec.h"


static AVOutputFormat		*fmt_out;
static AVStream			*st;
static AVFormatContext		*oc;
static AVCodec			*codec;

static snd_pcm_t		*pcm;
static snd_pcm_hw_params_t	*params;

static short			pcm_buf[4096];
static int			pcm_buf_size;
static struct g723_state	g723_state;
static unsigned char		g723_data[96];
static short			pcm_in[512];
static unsigned char		mp2_out[1024];

extern char *__progname;

static void err(const char *fmt, ...)
{
	va_list va;

	fprintf(stderr, "ERROR: ");

	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);

	fprintf(stderr, "\n");

	exit(1);
}

static int pcm_dupe(short *in, int in_size, short *out)
{
	int n;

	for (n = 0; n < in_size; n++)
		out[n * 2] = out[(n * 2) + 1] = in[n];

	return in_size * 2;
}


static void usage(void)
{
        fprintf(stderr, "Usage: %s -o <outfile> <alsadev>\n", __progname);
	exit(1);
}

int main(int argc, char **argv)
{
	char *outfile = NULL;
	char *alsadev;
	int opt;

	while ((opt = getopt(argc, argv, "o:h")) != -1) {
		switch (opt) {
		case 'o': outfile = optarg; break;
		case 'h': default: usage();
		}
	}

	if (optind >= argc || outfile == NULL)
		usage();

	alsadev = argv[optind];

	/* Startup the avformat stuff first */
	avcodec_register_all();
	av_register_all();

	fmt_out = av_guess_format(NULL, outfile, NULL);
	if (fmt_out == NULL)
		err("opening fmt_out: %s", outfile);

	if ((oc = avformat_alloc_context()) == NULL)
		err("allocating context");

	oc->oformat = fmt_out;
	snprintf(oc->filename, sizeof(oc->filename), "%s", outfile);

	st = avformat_new_stream(oc, 0);
	if (st == NULL)
		err("opening new stream");

	st->codec->codec_id = AV_CODEC_ID_MP2;
	st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
	st->codec->bit_rate = 32000;
	st->codec->sample_rate = 16000;
	st->codec->sample_fmt = AV_SAMPLE_FMT_S16;
	st->codec->channels = 1;
	st->codec->time_base = (AVRational){1, 16000};

	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	codec = avcodec_find_encoder(st->codec->codec_id);
	if (codec == NULL)
		err("finding encoder");

	if (avcodec_open2(st->codec, codec, NULL) < 0)
		err("opening encoder");

	if (avio_open(&oc->pb, outfile, AVIO_FLAG_WRITE) < 0)
		err("opening out file");

	avformat_write_header(oc, NULL);

	/* Open the alsa device */
	if (snd_pcm_open(&pcm, alsadev, SND_PCM_STREAM_CAPTURE,
			 SND_PCM_NONBLOCK) < 0)
		err("opening alsa device");

	snd_pcm_hw_params_alloca(&params);

	if (snd_pcm_hw_params_any(pcm, params) < 0)
		err("initial params");

	if (snd_pcm_hw_params_set_access(pcm, params,
			SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
		err("set access");

	if (snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_U8) < 0)
		err("set format");

	if (snd_pcm_hw_params_set_channels(pcm, params, 1) < 0)
		err("setting channels");

	if (snd_pcm_hw_params_set_rate(pcm, params, 8000, 0) < 0)
		err("setting rate");

	if (snd_pcm_hw_params(pcm, params) < 0)
		err("setting audio dev params");

	if (snd_pcm_prepare(pcm) < 0)
		err("preparing pcm");

	/* Initialize g723 decoder */
	g723_init(&g723_state);

	while (1) {
		int size = snd_pcm_readi(pcm, g723_data, sizeof(g723_data));
		int frame_size = st->codec->frame_size;
		AVPacket pkt;

		if (size == -EAGAIN)
			continue;

		if (size < 0)
			err("reading from pcm: %s", snd_strerror(size));

		size = g723_decode(&g723_state, g723_data, size, pcm_in);
		pcm_buf_size += pcm_dupe(pcm_in, size, pcm_buf + pcm_buf_size);

		if (pcm_buf_size < frame_size)
			continue;

		av_init_packet(&pkt);

		pkt.size = avcodec_encode_audio(st->codec, mp2_out,
						sizeof(mp2_out), pcm_buf);

		if (pcm_buf_size > frame_size)
			memcpy(pcm_buf, pcm_buf + frame_size,
			       (pcm_buf_size - frame_size) * 2);

		pcm_buf_size -= frame_size;

		if (pkt.size == 0)
			continue;

		if (st->codec->coded_frame->pts != AV_NOPTS_VALUE)
			pkt.pts = av_rescale_q(st->codec->coded_frame->pts,
					       st->codec->time_base,
					       st->time_base);

		pkt.flags |= AV_PKT_FLAG_KEY;
		pkt.stream_index = st->index;
		pkt.data = mp2_out;

		if (av_write_frame(oc, &pkt))
			err("writing frame");
	}

	exit(0);
}
