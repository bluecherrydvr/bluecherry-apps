/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "bc-server.h"

int bc_mux_out(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	AVPacket pkt;

	av_init_packet(&pkt);

	if (bc_buf_key_frame(bc))
		pkt.flags |= PKT_FLAG_KEY;

	pkt.data = bc_buf_data(bc);
	pkt.size = bc_buf_size(bc);

	if (av_write_frame(bc_rec->oc, &pkt)) {
		errno = EIO;
		return -1;
	}

	return 0;
}

void bc_close_avcodec(struct bc_record *bc_rec)
{
	int i;

	avcodec_close(bc_rec->video_st->codec);
	av_write_trailer(bc_rec->oc);

	for (i = 0; i < bc_rec->oc->nb_streams; i++) {
		av_freep(&bc_rec->oc->streams[i]->codec);
		av_freep(&bc_rec->oc->streams[i]);
	}

	if (!(bc_rec->fmt_out->flags & AVFMT_NOFILE))
		url_fclose(bc_rec->oc->pb);

	av_free(bc_rec->oc);
}

static int mkdir_recursive(char *path)
{
	char *t;

	/* If we succeed, sweetness */
	if (!mkdir(path, 0755))
		return 0;

	/* Try to make the parent directory */
	t = strrchr(path, '/');
	if (t == NULL || t == path)
		return -1;
	*t = '\0';
	mkdir_recursive(path);
	*t = '/';

	return mkdir(path, 0755);
}

int bc_open_avcodec(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	AVCodec *codec;
	AVInputFormat *fmt_in;
	AVStream *video_st;
	AVFormatContext *oc;
	time_t t;
	struct tm tm;
	char date[12], mytime[10];

	t = time(NULL);
	gmtime_r(&t, &tm);
	strftime(date, sizeof(date), "%Y/%m/%d", &tm);
	strftime(mytime, sizeof(mytime), "%T", &tm);
	sprintf(bc_rec->outfile, "%s/%s/%06d", BC_FILE_REC_BASE, date,
		bc_rec->id);
	mkdir_recursive(bc_rec->outfile);
	sprintf(bc_rec->outfile, "%s/%s.mkv", bc_rec->outfile, mytime);

	/* Initialize avcodec */
	avcodec_init();
	avcodec_register_all();
	av_register_all();

	/* Get the input and output formats */
	bc_rec->fmt_out = guess_format(NULL, bc_rec->outfile, NULL);
	if (!bc_rec->fmt_out) {
		errno = EINVAL;
		return -1;
	}
	fmt_in = av_find_input_format("mpeg");
	if (!fmt_in) {
		errno = EINVAL;
		return -1;
	}

	if ((bc_rec->oc = avformat_alloc_context()) == NULL)
		return -1;
	oc = bc_rec->oc;

	oc->oformat = bc_rec->fmt_out;
	oc->iformat = fmt_in;
	snprintf(oc->filename, sizeof(oc->filename), "%s", bc_rec->outfile);
	snprintf(oc->title, sizeof(oc->title), "BC: %s", bc->dev_file);

	/* Setup new video stream */
	bc_rec->video_st = av_new_stream(oc, 0);
	if (!bc_rec->video_st)
		return -1;
	video_st = bc_rec->video_st;

	video_st->stream_copy = 1;
	video_st->time_base.den =
		bc->vparm.parm.capture.timeperframe.denominator;
	video_st->time_base.num =
		bc->vparm.parm.capture.timeperframe.numerator;
	snprintf(video_st->language, sizeof(video_st->language), "eng");

	video_st->codec->codec_id = CODEC_ID_MPEG4;
	video_st->codec->codec_type = CODEC_TYPE_VIDEO;
	video_st->codec->pix_fmt = PIX_FMT_YUV420P;
	video_st->codec->width = bc->vfmt.fmt.pix.width;
	video_st->codec->height = bc->vfmt.fmt.pix.height;
	video_st->codec->time_base.den =
		bc->vparm.parm.capture.timeperframe.denominator;
	video_st->codec->time_base.num =
		bc->vparm.parm.capture.timeperframe.numerator;

	if(oc->oformat->flags & AVFMT_GLOBALHEADER)
		video_st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	if (av_set_parameters(oc, NULL) < 0)
		return -1;

	/* Open Video output */
	if ((codec = avcodec_find_encoder(video_st->codec->codec_id)) == NULL)
		return -1;

	if (avcodec_open(video_st->codec, codec) < 0)
		return -1;

	/* Open output file, if needed */
	if (!(bc_rec->fmt_out->flags & AVFMT_NOFILE))
		if (url_fopen(&oc->pb, bc_rec->outfile, URL_WRONLY) < 0)
			return -1;

	av_write_header(oc);

	return 0;
}
