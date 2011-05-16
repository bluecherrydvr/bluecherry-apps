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

int has_audio(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;

	if (bc_rec->pcm != NULL)
		return 1;

	if (!(bc->cam_caps & BC_CAM_CAP_RTSP))
		return 0;
	if (bc->rtp_sess.tid_a >= 0)
		return 1;
	if (bc->rtp_sess.aud_port >= 0)
		return 1;

	return 0;
}

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
			   SND_PCM_STREAM_CAPTURE, SND_PCM_ASYNC);
//			   SND_PCM_NONBLOCK);
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

/* Returns zero meaning "keep processing", or postive value meaning
 * "wrote one packet to file, can read more" or negative value meaning
 * "error occured or nothing to process". */
int bc_aud_out(struct bc_record *bc_rec)
{
	AVCodecContext *c;
	AVPacket pkt;

	/* pcm can be null due to not being able to open the alsa dev. */
	if (!has_audio(bc_rec) || !bc_rec->audio_st)
		return 0;

	c = bc_rec->audio_st->codec;
	av_init_packet(&pkt);

	if (bc_rec->pcm) {
		int size;

		size = snd_pcm_readi(bc_rec->pcm, bc_rec->g723_data,
				     sizeof(bc_rec->g723_data));

		if (size < 0) {
			if (size == -EAGAIN)
				return 0;
			bc_dev_err(bc_rec, "Error reading from sound device: %s",
				   snd_strerror(size));
			bc_alsa_close(bc_rec);
			avcodec_close(bc_rec->audio_st->codec);
			bc_rec->audio_st = NULL;
			return -1;
		}

		pkt.size = g723_decode(&bc_rec->g723_state,
				       bc_rec->g723_data, size,
				       bc_rec->pcm_data);

		pkt.data = (void *)bc_rec->pcm_data;
	} else {
		struct rtp_session *rs = &bc_rec->bc->rtp_sess;

		if (!rs->aud_valid)
			return 0;

		pkt.size = rs->aud_len;
		pkt.data = rs->aud_buf;
	}

	if (c->coded_frame->pts != AV_NOPTS_VALUE)
		pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base,
				       bc_rec->audio_st->time_base);
	pkt.flags |= PKT_FLAG_KEY;
	pkt.stream_index = bc_rec->audio_st->index;

	if (av_write_frame(bc_rec->oc, &pkt)) {
		bc_dev_err(bc_rec, "Error encoding audio frame");
		return -1;
	}

	return 0;
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

	if (bc->cam_caps & BC_CAM_CAP_RTSP) {
		/* RTP is at a constant 90KHz time scale */
		pkt.pts = av_rescale_q(bc->rtp_sess.vid_ts, (AVRational){1, 90000},
				       bc_rec->video_st->time_base);
	} else {
		if (c->coded_frame->pts != AV_NOPTS_VALUE)
			pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base,
					       bc_rec->video_st->time_base);
	}

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
		if (bc_rec->oc->pb)
			av_write_trailer(bc_rec->oc);

		for (i = 0; i < bc_rec->oc->nb_streams; i++) {
			av_freep(&bc_rec->oc->streams[i]->codec);
			av_freep(&bc_rec->oc->streams[i]);
		}

		if (bc_rec->oc->pb)
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

void bc_mkdir_recursive(char *path)
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
	bc_mkdir_recursive(path);
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
	char stor[256];

	bc_get_media_loc(stor);

	/* XXX Need some way to reconcile time between media event and
	 * filename. They should match. */
	localtime_r(&t, &tm);

	strftime(date, sizeof(date), "%Y/%m/%d", &tm);
	strftime(mytime, sizeof(mytime), "%H-%M-%S", &tm);
	sprintf(dir, "%s/%s/%06d", stor, date, bc_rec->id);
	bc_mkdir_recursive(dir);
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
	codec = avcodec_find_decoder(bc->rtp_sess.vid_codec);
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

	if (bc_rec->codec_id == CODEC_ID_NONE) {
		if (bc->cam_caps & BC_CAM_CAP_RTSP)
			st->codec->codec_id = bc->rtp_sess.vid_codec;

		if (st->codec->codec_id == CODEC_ID_NONE) {
			bc_dev_warn(bc_rec, "Invalid Video Format, assuming MP4V-ES");
			st->codec->codec_id = CODEC_ID_MPEG4;
		}
	} else
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
	if (has_audio(bc_rec)) {
		enum CodecID codec_id;

		if (bc_rec->pcm)
			codec_id = CODEC_ID_PCM_S16LE;
		else
			codec_id = bc->rtp_sess.aud_codec;

		/* If we can't find an encoder, just skip it */
		if (avcodec_find_encoder(codec_id) == NULL) {
			bc_dev_warn(bc_rec, "Failed to find audio codec (%08x) "
				    "so not recording", codec_id);
			goto no_audio;
		}

		if ((bc_rec->audio_st = av_new_stream(oc, 1)) == NULL)
			goto no_audio;
		st = bc_rec->audio_st;
		st->codec->codec_id = codec_id;
		st->codec->codec_type = CODEC_TYPE_AUDIO;

		if (bc_rec->pcm) {
			st->codec->sample_rate = 8000;
			st->codec->sample_fmt = SAMPLE_FMT_S16;
			st->codec->channels = 1;
			st->codec->time_base = (AVRational){1, 8000};
		} else {
			struct rtp_session *rs = &bc->rtp_sess;

			st->codec->bit_rate = rs->bitrate;
			st->codec->sample_rate = rs->samplerate;
			st->codec->channels = rs->channels;
			st->codec->time_base = (AVRational){1, rs->samplerate};
		}

		snprintf(st->language, sizeof(st->language), "eng");

		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
			st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
no_audio:
		st = NULL;
	}

	if (av_set_parameters(oc, NULL) < 0)
		return -1;

	/* Open Video output */
	st = bc_rec->video_st;
	codec = avcodec_find_encoder(st->codec->codec_id);
	if (codec == NULL || avcodec_open(st->codec, codec) < 0) {
		bc_rec->video_st = NULL;
		/* Clear this */
		if (bc_rec->audio_st)
			bc_rec->audio_st = NULL;
		return -1;
	}
	st = NULL;

	/* Open Audio output */
	if (bc_rec->audio_st) {
		st = bc_rec->audio_st;
		codec = avcodec_find_encoder(st->codec->codec_id);
		if (codec == NULL || avcodec_open(st->codec, codec) < 0) {
			bc_rec->audio_st = NULL;
			bc_dev_warn(bc_rec, "Failed to open audio codec (%08x) "
				    "so not recording", st->codec->codec_id);
		}
		st = NULL;
	}

	/* Open output file */
	if (url_fopen(&oc->pb, bc_rec->outfile, URL_WRONLY) < 0) {
		bc_dev_err(bc_rec, "Failed to open outfile (perms?): %s",
			   bc_rec->outfile);
		return -1;
	}

	av_write_header(oc);

	return 0;
}

int bc_open_avcodec(struct bc_record *bc_rec)
{
	int ret;

	if (pthread_mutex_lock(&av_lock) == EDEADLK)
		bc_dev_err(bc_rec, "Deadlock detected in av_lock on avcodec open!");
	ret = __bc_open_avcodec(bc_rec);
	if (ret) {
		bc_media_destroy(&bc_rec->media);
		__bc_close_avcodec(bc_rec);
	}
	pthread_mutex_unlock(&av_lock);

	return ret;
}
