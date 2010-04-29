/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "bc-server.h"

int bc_aud_out(struct bc_record *bc_rec)
{
	AVCodecContext *c = bc_rec->audio_st->codec;
	AVPacket pkt;
	unsigned char g723_data[48];
	short pcm_data[128];
	int size;

	/* This would be due to not being able to open the alsa dev */
	if (!bc_rec->pcm)
		return 0;

	if ((size = snd_pcm_readi(bc_rec->pcm, g723_data, 48)) != 48) {
		if (!bc_rec->snd_err) {
			bc_alsa_close(bc_rec);
			bc_rec->snd_err = 1;
			if (bc_alsa_open(bc_rec)) {
				bc_log("E(%d): asoundlib failed", bc_rec->id);
				avcodec_close(bc_rec->audio_st->codec);
				bc_rec->audio_st = NULL;
			}
			return bc_aud_out(bc_rec);
		}
		errno = -size;
		return -1;
	}

	av_init_packet(&pkt);

	pkt.size = g723_decode(&bc_rec->g723_state, g723_data, 48,
			       pcm_data) * 2;
	if (c->coded_frame->pts != AV_NOPTS_VALUE)
		pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base,
				       bc_rec->audio_st->time_base);
	pkt.flags |= PKT_FLAG_KEY;
	pkt.stream_index = bc_rec->audio_st->index;
	pkt.data = (void *)pcm_data;

	if (av_interleaved_write_frame(bc_rec->oc, &pkt)) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int bc_vid_out(struct bc_record *bc_rec)
{
	AVCodecContext *c = bc_rec->video_st->codec;
	struct bc_handle *bc = bc_rec->bc;
	AVPacket pkt;

	av_init_packet(&pkt);

	if (bc_buf_key_frame(bc))
		pkt.flags |= PKT_FLAG_KEY;

	pkt.data = bc_buf_data(bc);
	pkt.size = bc_buf_size(bc);
	if (c->coded_frame->pts != AV_NOPTS_VALUE)
		pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base,
				       bc_rec->video_st->time_base);
	pkt.stream_index = bc_rec->video_st->index;

	if (av_interleaved_write_frame(bc_rec->oc, &pkt)) {
		errno = EIO;
		return -1;
	}

	return 0;
}

void bc_close_avcodec(struct bc_record *bc_rec)
{
	int i;

	pthread_mutex_lock(&bc_rec->lock);

	avcodec_close(bc_rec->video_st->codec);
	if (bc_rec->audio_st)
		avcodec_close(bc_rec->audio_st->codec);

	av_write_trailer(bc_rec->oc);

	for (i = 0; i < bc_rec->oc->nb_streams; i++) {
		av_freep(&bc_rec->oc->streams[i]->codec);
		av_freep(&bc_rec->oc->streams[i]);
	}

	url_fclose(bc_rec->oc->pb);
	av_free(bc_rec->oc);

	pthread_mutex_unlock(&bc_rec->lock);
}

static void mkdir_recursive(char *path)
{
	char *t;

	/* If we succeed, sweetness */
	if (!mkdir(path, 0755))
		return;

	/* Try to make the parent directory */
	t = strrchr(path, '/');
	if (t == NULL || t == path)
		return;
	*t = '\0';
	mkdir_recursive(path);
	*t = '/';

	mkdir(path, 0755);
}

int bc_open_avcodec(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	AVCodec *codec;
	AVStream *st;
	AVFormatContext *oc;
	time_t t;
	struct tm tm;
	char date[12], mytime[10], dir[PATH_MAX];

	pthread_mutex_lock(&bc_rec->lock);

	t = time(NULL);
	strftime(date, sizeof(date), "%Y/%m/%d", localtime_r(&t, &tm));
	strftime(mytime, sizeof(mytime), "%T", &tm);
	sprintf(dir, "%s/%s/%06d", BC_FILE_REC_BASE, date, bc_rec->id);
	mkdir_recursive(dir);
	sprintf(bc_rec->outfile, "%s/%s.mkv", dir, mytime);

	/* Initialize avcodec */
	av_register_all();

	/* Get the output format */
	bc_rec->fmt_out = guess_format(NULL, bc_rec->outfile, NULL);
	if (!bc_rec->fmt_out) {
		errno = EINVAL;
		return -1;
	}

	if ((bc_rec->oc = avformat_alloc_context()) == NULL)
		return -1;
	oc = bc_rec->oc;

	oc->oformat = bc_rec->fmt_out;
	snprintf(oc->filename, sizeof(oc->filename), "%s", bc_rec->outfile);
	snprintf(oc->title, sizeof(oc->title), "BC: %s", bc_rec->name);

	/* Setup new video stream */
	if ((bc_rec->video_st = av_new_stream(oc, 0)) == NULL)
		return -1;
	st = bc_rec->video_st;

	st->stream_copy = 1;
	st->time_base.den =
		bc->vparm.parm.capture.timeperframe.denominator;
	st->time_base.num =
		bc->vparm.parm.capture.timeperframe.numerator;
	snprintf(st->language, sizeof(st->language), "eng");

	st->codec->codec_id = CODEC_ID_MPEG4;
	st->codec->codec_type = CODEC_TYPE_VIDEO;
	st->codec->pix_fmt = PIX_FMT_YUV420P;
	st->codec->width = bc->vfmt.fmt.pix.width;
	st->codec->height = bc->vfmt.fmt.pix.height;
	st->codec->time_base.den =
		bc->vparm.parm.capture.timeperframe.denominator;
	st->codec->time_base.num =
		bc->vparm.parm.capture.timeperframe.numerator;

	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	st = NULL;

	/* Setup new audio stream */
	if (bc_rec->pcm) {
		if ((bc_rec->audio_st = av_new_stream(oc, 1)) == NULL)
			return -1;
		st = bc_rec->audio_st;

		st->stream_copy = 1;
		st->codec->codec_id = CODEC_ID_PCM_S16LE;
		st->codec->codec_type = CODEC_TYPE_AUDIO;
		st->codec->bit_rate = 128000;
		st->codec->sample_rate = 8000;
		st->codec->channels = 1;
		snprintf(st->language, sizeof(st->language), "eng");

		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
                	st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

		st = NULL;
	}

	if (av_set_parameters(oc, NULL) < 0)
		return -1;

	/* Open Video output */
	if ((codec = avcodec_find_encoder(bc_rec->video_st->codec->codec_id))
	    == NULL)
		return -1;

	if (avcodec_open(bc_rec->video_st->codec, codec) < 0)
		return -1;

	/* Open Audio output */
	if (bc_rec->audio_st) {
		codec = avcodec_find_encoder(bc_rec->audio_st->codec->codec_id);
		if (codec == NULL)
			return -1;

		if (avcodec_open(bc_rec->audio_st->codec, codec) < 0)
			return -1;
	}

	/* Open output file */
	if (url_fopen(&oc->pb, bc_rec->outfile, URL_WRONLY) < 0)
		return -1;

	av_write_header(oc);

	pthread_mutex_unlock(&bc_rec->lock);

	return 0;
}
