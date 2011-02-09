/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "bc-server.h"

/* Used to maintain coherency between calls to avcodec open/close. The
 * libavcodec open/close is not thread safe and libavcodec will complain
 * (loudly) if it sees this occuring. */
static pthread_mutex_t av_lock = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

static unsigned int bc_to_alsa_fmt(unsigned int fmt)
{
	/* Prefer S16_LE */
	if (fmt & AUD_FMT_PCM_S16_LE)
		return SND_PCM_FORMAT_S16_LE;
	else if (fmt & AUD_FMT_PCM_U8)
		return SND_PCM_FORMAT_U8;
	else if (fmt & AUD_FMT_PCM_S8)
		return SND_PCM_FORMAT_S8;
	else if (fmt & AUD_FMT_PCM_U16_LE)
		return SND_PCM_FORMAT_U16_LE;
	else if (fmt & AUD_FMT_PCM_U16_BE)
		return SND_PCM_FORMAT_U16_BE;
	else if (fmt & AUD_FMT_PCM_S16_BE)
		return SND_PCM_FORMAT_S16_BE;

	return -1;
}

static void bc_alsa_close(struct bc_record *bc_rec)
{
	if (!bc_rec->pcm)
		return;

	snd_pcm_close(bc_rec->pcm);
	bc_rec->pcm = NULL;
}

static int bc_alsa_open(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	snd_pcm_hw_params_t *params = NULL;
	snd_pcm_t *pcm = NULL;
	int err, fmt;

	if (!(bc->cam_caps & BC_CAM_CAP_V4L2))
		return 0;

	/* No alsa device for this record */
	if (bc_rec->aud_dev[0] == '\0' || bc_rec->pcm)
		return 0;

	err = snd_pcm_open(&pcm, bc_rec->aud_dev,
			   SND_PCM_STREAM_CAPTURE,
			   SND_PCM_NONBLOCK);
	snd_config_update_free_global(); /* Make valgrind happy */
	if (err < 0) {
		bc_dev_err(bc_rec, "Opening audio device failed: %s",
			   snd_strerror(err));
		return -1;
	}

	bc_rec->pcm = pcm;

	snd_pcm_hw_params_alloca(&params);

	if ((err = snd_pcm_hw_params_any(pcm, params)) < 0) {
		bc_dev_err(bc_rec, "No audio device configurations available: %s",
			   snd_strerror(err));
		return -1;
	}

	if ((err = snd_pcm_hw_params_set_access(pcm, params,
					 SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		bc_dev_err(bc_rec, "Setting audio device access type failed: %s",
			   snd_strerror(err));
		return -1;
	}

	if ((fmt = bc_to_alsa_fmt(bc_rec->aud_format)) == -1) {
		bc_dev_err(bc_rec, "bc_to_alsa_fmt failed");
		return -1;
	}

	if ((err = snd_pcm_hw_params_set_format(pcm, params, fmt)) < 0) {
		bc_dev_err(bc_rec, "Setting audio device format failed: %s",
			   snd_strerror(err));
		return -1;
	}

	if ((err = snd_pcm_hw_params_set_channels(pcm, params,
					   bc_rec->aud_channels)) < 0) {
		bc_dev_err(bc_rec, "Setting audio device channels failed: %s",
			   snd_strerror(err));
		return -1;
	}

	if ((err = snd_pcm_hw_params_set_rate(pcm, params, bc_rec->aud_rate,
					      0)) < 0) {
		bc_dev_err(bc_rec, "Setting audio device rate failed: %s",
			   snd_strerror(err));
		return -1;
	}

	if ((err = snd_pcm_hw_params(pcm, params)) < 0) {
		bc_dev_err(bc_rec, "Setting audio device parameters failed: %s",
			   snd_strerror(err));
		return -1;
	}

	if ((err = snd_pcm_prepare(pcm)) < 0) {
		bc_dev_err(bc_rec, "Failed to prepare audio device: %s",
			   snd_strerror(err));
		return -1;
	}

	g723_init(&bc_rec->g723_state);

	return 0;
}

/* Convert 8khz PCM to 16khz. Horrid in that we don't do wave smoothing,
 * but it's fast and serves our purpose. MP2 audio doesn't support 8khz
 * audio, which is why we need this. */
static int pcm_dupe(short *in, int in_size, short *out)
{
	int n;

	for (n = 0; n < in_size; n++)
		out[n * 2] = out[(n * 2) + 1] = in[n];

	return in_size * 2;
}

int bc_aud_out(struct bc_record *bc_rec)
{
	AVCodecContext *c = NULL;
	AVPacket pkt;
	unsigned char g723_data[96];
	short pcm_in[512];
	unsigned char mp2_out[1024];
	int size;

	/* pcm can be null due to not being able to open the alsa dev. */
	if (!bc_rec->pcm)
		return -1;

	memset(g723_data, 0, sizeof(g723_data));

	size = snd_pcm_readi(bc_rec->pcm, g723_data, sizeof(g723_data));

	if (size < 0) {
		if (size == -EAGAIN)
			return 1;
		bc_dev_err(bc_rec, "Error reading from sound device: %s",
			   snd_strerror(size));
		bc_alsa_close(bc_rec);
		return -1;
	}

	size = g723_decode(&bc_rec->g723_state, g723_data, size, pcm_in);

	bc_rec->pcm_buf_size += pcm_dupe(pcm_in, size,
					 bc_rec->pcm_buf +
					 bc_rec->pcm_buf_size);

	c = bc_rec->audio_st->codec;

	/* We need enough data to encode first... */
	if (bc_rec->pcm_buf_size < c->frame_size)
		return 0;

	av_init_packet(&pkt);

	pkt.size = avcodec_encode_audio(c, mp2_out, sizeof(mp2_out),
					bc_rec->pcm_buf);

	/* Most likely, we don't align perfectly */
	if (bc_rec->pcm_buf_size > c->frame_size)
		memcpy(bc_rec->pcm_buf, bc_rec->pcm_buf + c->frame_size,
		       (bc_rec->pcm_buf_size - c->frame_size) * 2);

	bc_rec->pcm_buf_size -= c->frame_size;

	/* Not enough to encode a full buffer yet */
	if (pkt.size == 0)
		return 0;

	if (c->coded_frame->pts != AV_NOPTS_VALUE)
		pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base,
				       bc_rec->audio_st->time_base);
	pkt.flags |= PKT_FLAG_KEY;
	pkt.stream_index = bc_rec->audio_st->index;
	pkt.data = mp2_out;

	if (av_write_frame(bc_rec->oc, &pkt)) {
		bc_dev_err(bc_rec, "Error encoding audio frame");
		return -1;
	}

	return 1;
}

int bc_vid_out(struct bc_record *bc_rec)
{
	AVCodecContext *c;
	struct bc_handle *bc = bc_rec->bc;
	AVPacket pkt;

	if (!bc_rec->oc)
		return 0;

	c = bc_rec->video_st->codec;

	av_init_packet(&pkt);

	if (bc_buf_key_frame(bc))
		pkt.flags |= PKT_FLAG_KEY;

	pkt.data = bc_buf_data(bc);
	pkt.size = bc_buf_size(bc);

	/* See if there is data to send */
	if (!pkt.data || !pkt.size)
		return 0;

	if (c->coded_frame->pts != AV_NOPTS_VALUE)
		pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base,
				       bc_rec->video_st->time_base);
	pkt.stream_index = bc_rec->video_st->index;

	if (av_write_frame(bc_rec->oc, &pkt)) {
		errno = EIO;
		return -1;
	}

	return 0;
}

static void __bc_close_avcodec(struct bc_record *bc_rec)
{
	int i;

	bc_alsa_close(bc_rec);

	if (bc_rec->video_st)
		avcodec_close(bc_rec->video_st->codec);
	if (bc_rec->audio_st)
		avcodec_close(bc_rec->audio_st->codec);
	bc_rec->video_st = bc_rec->audio_st = NULL;

	if (bc_rec->oc) {
		av_write_trailer(bc_rec->oc);

		for (i = 0; i < bc_rec->oc->nb_streams; i++) {
			av_freep(&bc_rec->oc->streams[i]->codec);
			av_freep(&bc_rec->oc->streams[i]);
		}

		url_fclose(bc_rec->oc->pb);
		av_free(bc_rec->oc);
		bc_rec->oc = NULL;
	}

	/* Close the media entry in the db */
	bc_event_cam_end(&bc_rec->event);
	bc_media_end(&bc_rec->media);
}

void bc_close_avcodec(struct bc_record *bc_rec)
{
	if (pthread_mutex_lock(&av_lock) == EDEADLK)
		bc_dev_err(bc_rec, "Deadlock detected in av_lock on avcodec close!");
	__bc_close_avcodec(bc_rec);
	pthread_mutex_unlock(&av_lock);
}

static void mkdir_recursive(char *path)
{
	char *t;

	/* If we succeed, sweetness */
	if (!mkdir(path, 0750))
		return;

	/* Try to make the parent directory */
	t = strrchr(path, '/');
	if (t == NULL || t == path)
		return;
	*t = '\0';
	mkdir_recursive(path);
	*t = '/';

	mkdir(path, 0750);
}

static void bc_start_media_entry(struct bc_record *bc_rec)
{
	bc_media_video_type_t video = BC_MEDIA_VIDEO_M4V;
	bc_media_audio_type_t audio = BC_MEDIA_AUDIO_NONE;
	bc_media_cont_type_t cont = BC_MEDIA_CONT_MKV;
	time_t t = time(NULL);
	struct tm tm;
	char date[12], mytime[10], dir[PATH_MAX];

	/* XXX Need some way to reconcile time between media event and
	 * filename. They should match. */
	strftime(date, sizeof(date), "%Y/%m/%d", localtime_r(&t, &tm));
	strftime(mytime, sizeof(mytime), "%T", &tm);
	sprintf(dir, "%s/%s/%06d", media_storage, date, bc_rec->id);
	mkdir_recursive(dir);
	sprintf(bc_rec->outfile, "%s/%s.mkv", dir, mytime);

	if (bc_rec->sched_cur == 'C' && bc_rec->event == BC_EVENT_CAM_NULL)
		bc_rec->event = bc_event_cam_start(bc_rec->id, BC_EVENT_L_INFO,
					BC_EVENT_CAM_T_CONTINUOUS, bc_rec->media);

	/* Now start the next one */
	bc_rec->media = bc_media_start(bc_rec->id, video, audio, cont,
				       bc_rec->outfile, bc_rec->event);
}

static int bc_get_frame_info(struct bc_handle *bc, int *width, int *height,
			      int *fnum, int *fden)
{
	AVCodec *codec;
	AVCodecContext *c;
	int got_picture, len;
	AVFrame *picture;
	void *buf = bc_buf_data(bc);
	int size = bc_buf_size(bc);
	int ret = -1;

	if (bc->cam_caps & BC_CAM_CAP_V4L2) {
		*fden = bc->vparm.parm.capture.timeperframe.denominator;
		*fnum = bc->vparm.parm.capture.timeperframe.numerator;
		*width = bc->vfmt.fmt.pix.width;
		*height = bc->vfmt.fmt.pix.height;
		return 0;
	} else if (bc->cam_caps & BC_CAM_CAP_RTSP) {
		*fnum = 1;
		*fden = bc->rtp_sess.framerate;
	} else {
		return -1;
	}

	if (buf == NULL || size <= 0)
		return -1;

	/* Decode the first picture to get frame size */
	codec = avcodec_find_decoder(CODEC_ID_MPEG4);
	if (!codec)
		return -1;

	c = avcodec_alloc_context();
	picture = avcodec_alloc_frame();

	if (avcodec_open(c, codec) < 0)
		goto pic_info_fail;

	len = avcodec_decode_video(c, picture, &got_picture, buf, size);

        if (len < 0 || !got_picture)
		goto pic_info_fail;

	*width = c->width;
	*height = c->height;

	ret = 0;

pic_info_fail:
	if (c != NULL) {
		avcodec_close(c);
		av_freep(&c);
	}
	if (picture != NULL)
		av_freep(&picture);

	return ret;
}

static int __bc_open_avcodec(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	AVCodec *codec;
	AVStream *st;
	AVFormatContext *oc;
	int width = 0, height = 0, fnum, fden;

	if (bc_rec->oc != NULL)
		return 0;

	bc_start_media_entry(bc_rec);

	if (bc_rec->media == BC_MEDIA_NULL) {
		errno = ENOMEM;
		return -1;
	}

	if (bc_get_frame_info(bc, &width, &height, &fnum, &fden)) {
		errno = ENOMEM;
		return -1;
	}

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

	st->time_base.den = fden;
	st->time_base.num = fnum;
	snprintf(st->language, sizeof(st->language), "eng");

	st->codec->codec_id = bc_rec->codec_id;

	/* h264 requires us to work around libavcodec broken defaults */
	if (bc_rec->codec_id == CODEC_ID_H264) {
		st->codec->crf = 20;
		st->codec->me_range = 16;
		st->codec->me_subpel_quality = 7;
		st->codec->qmin = 10;
		st->codec->qmax = 51;
		st->codec->max_qdiff = 4;
		st->codec->qcompress = 0.6;
		st->codec->i_quant_factor = 0.71;
		st->codec->b_frame_strategy = 1;
	}

	st->codec->codec_type = CODEC_TYPE_VIDEO;
	st->codec->pix_fmt = PIX_FMT_YUV420P;
	st->codec->width = width;
	st->codec->height = height;
	st->codec->time_base = st->time_base;

	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	st = NULL;

	/* We don't fail when this happens. Video with no sound is
	 * better than no video at all. */
	if (bc_alsa_open(bc_rec))
		bc_alsa_close(bc_rec);

	/* Setup new audio stream */
	if (bc_rec->pcm) {
		if ((bc_rec->audio_st = av_new_stream(oc, 1)) == NULL)
			return -1;
		st = bc_rec->audio_st;

		st->codec->codec_id = CODEC_ID_MP2;
		st->codec->codec_type = CODEC_TYPE_AUDIO;
		st->codec->bit_rate = 32000;
		st->codec->sample_rate = 16000;
		st->codec->sample_fmt = SAMPLE_FMT_S16;
		st->codec->channels = 1;
		st->codec->time_base = (AVRational){1, 16000};

		snprintf(st->language, sizeof(st->language), "eng");

		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
			st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

		st = NULL;
	}

	if (av_set_parameters(oc, NULL) < 0)
		return -1;

	/* Open Video output */
	codec = avcodec_find_encoder(bc_rec->video_st->codec->codec_id);
	if (codec == NULL)
		return -1;

	/* If we fail to open the codec's, make sure to clear the
	 * state so we don't try to call avcodec_close() on unopened
	 * codec's. */
	if (avcodec_open(bc_rec->video_st->codec, codec) < 0) {
		bc_rec->video_st = NULL;
		if (bc_rec->audio_st)
			bc_rec->audio_st = NULL;
		return -1;
	}

	/* Open Audio output */
	if (bc_rec->audio_st) {
		codec = avcodec_find_encoder(bc_rec->audio_st->codec->codec_id);
		if (codec == NULL)
			return -1;

		/* Same as above for video */
		if (avcodec_open(bc_rec->audio_st->codec, codec) < 0) {
			bc_rec->audio_st = NULL;
			return -1;
		}
	}

	/* Open output file */
	if (url_fopen(&oc->pb, bc_rec->outfile, URL_WRONLY) < 0)
		return -1;

	av_write_header(oc);

	return 0;
}

int bc_open_avcodec(struct bc_record *bc_rec)
{
	int ret;

	if (pthread_mutex_lock(&av_lock) == EDEADLK)
		bc_dev_err(bc_rec, "Deadlock detected in av_lock on avcodec open!");
	ret = __bc_open_avcodec(bc_rec);
	if (ret)
		__bc_close_avcodec(bc_rec);
	pthread_mutex_unlock(&av_lock);

	return ret;
}
