/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "bc-server.h"

int bc_av_lockmgr(void **mutex, enum AVLockOp op)
{
	switch (op) {
		case AV_LOCK_CREATE:
			*mutex = malloc(sizeof(pthread_mutex_t));
			if (!*mutex)
				return 1;
			return !!pthread_mutex_init(*mutex, NULL);
		
		case AV_LOCK_OBTAIN:
			return !!pthread_mutex_lock(*mutex);
		
		case AV_LOCK_RELEASE:
			return !!pthread_mutex_unlock(*mutex);
			
		case AV_LOCK_DESTROY:
			pthread_mutex_destroy(*mutex);
			free(*mutex);
			return 0;
	}

	return 1;
}

int has_audio(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;

	if (bc_rec->aud_disabled)
		return 0;

	if (bc_rec->pcm != NULL)
		return 1;

	if ((bc->cam_caps & BC_CAM_CAP_RTSP) && bc->rtp_sess.audio_stream_index >= 0)
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
		
		if (c->coded_frame->pts != AV_NOPTS_VALUE)
			pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base,
		                           bc_rec->audio_st->time_base);
	} else {
		struct rtp_session *rs = &bc_rec->bc->rtp_sess;

		if (rs->frame.stream_index != rs->audio_stream_index)
			return 0;

		pkt.data = rs->frame.data;
		pkt.size = rs->frame.size;
/*		pkt.pts = av_rescale_q(rs->frame.pts, (AVRational){1, bc_rec->audio_st->codec->sample_rate},
		                       bc_rec->audio_st->time_base);*/
	}

	pkt.flags |= AV_PKT_FLAG_KEY;
	pkt.stream_index = bc_rec->audio_st->index;

//	bc_dev_info(bc_rec, "audio pts %lld (frame: %lld)", pkt.pts, bc_rec->bc->rtp_sess.frame.pts);

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
	int re;

	if (!bc_rec->oc)
		return 0;

	c = bc_rec->video_st->codec;

	av_init_packet(&pkt);

	if (bc_buf_key_frame(bc))
		pkt.flags |= AV_PKT_FLAG_KEY;

	pkt.data = bc_buf_data(bc);
	pkt.size = bc_buf_size(bc);

	/* See if there is data to send */
	if (!pkt.data || !pkt.size)
		return 0;

	if (bc->cam_caps & BC_CAM_CAP_RTSP) {
		/* RTP is at a constant 90KHz time scale */
		pkt.pts = av_rescale_q(bc->rtp_sess.frame.pts, (AVRational){1, 90000},
				       bc_rec->video_st->time_base);
	} else {
		if (c->coded_frame->pts != AV_NOPTS_VALUE)
			pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base,
					       bc_rec->video_st->time_base);
	}

	pkt.stream_index = bc_rec->video_st->index;
	
//	bc_dev_info(bc_rec, "video pts %lld", pkt.pts);

	if ((re = av_write_frame(bc_rec->oc, &pkt)) != 0) {
		char averror[512] = { 0 };
		av_strerror(re, averror, sizeof(averror));
		bc_dev_err(bc_rec, "cannot write frame to recording (%s)", averror);
		return -1;
	}

	return 0;
}

void bc_close_avcodec(struct bc_record *bc_rec)
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

static void bc_save_jpeg(struct bc_record *bc_rec, AVCodecContext *oc, AVFrame *picture)
{
	AVCodecContext *joc = NULL;
	AVCodec *codec;
	unsigned char *buf = NULL;
	int buf_size;
	int pic_size;

	buf_size = avpicture_get_size(PIX_FMT_YUVJ420P, oc->width, oc->height);

	buf = malloc(buf_size);
	if (buf == NULL)
		return;

	joc = avcodec_alloc_context();
	if (!joc)
		goto save_fail;

	joc->bit_rate = oc->bit_rate;
	joc->width = oc->width;
	joc->height = oc->height;
	joc->pix_fmt = PIX_FMT_YUVJ420P; // XXX 422 or 444?
	joc->codec_id = CODEC_ID_MJPEG;
	joc->codec_type = AVMEDIA_TYPE_VIDEO;
	joc->time_base.num = oc->time_base.num;
	joc->time_base.den = oc->time_base.den;

	codec = avcodec_find_encoder(joc->codec_id);
	if (!codec)
		goto save_fail;

	if (avcodec_open(joc, codec) < 0) {
		codec = NULL;
		goto save_fail;
	}

	joc->mb_lmin = joc->lmin = joc->qmin * FF_QP2LAMBDA;
	joc->mb_lmax = joc->lmax = joc->qmax * FF_QP2LAMBDA;
	joc->flags = CODEC_FLAG_QSCALE;
	joc->global_quality = joc->qmin * FF_QP2LAMBDA;

	picture->pts = 1;
	picture->quality = joc->global_quality;
	pic_size = avcodec_encode_video(joc, buf, buf_size, picture);

	bc_media_set_snapshot(bc_rec->event, buf, pic_size);

save_fail:
	if (joc != NULL) {
		if (codec)
			avcodec_close(joc);
		av_freep(&joc);
	}
	if (buf != NULL)
		free(buf);
}

static int bc_get_frame_info(struct bc_record *bc_rec, int *width, int *height,
			      int *fnum, int *fden)
{
	struct bc_handle *bc = bc_rec->bc;
	AVCodec *codec = NULL;
	AVCodecContext *c;
	int got_picture, len;
	AVFrame *picture;
	AVPacket packet;
	enum CodecID codec_id = CODEC_ID_NONE;
	void *buf = bc_buf_data(bc);
	int size = bc_buf_size(bc);

	if (bc->cam_caps & BC_CAM_CAP_V4L2) {
		*fden = bc->vparm.parm.capture.timeperframe.denominator;
		*fnum = bc->vparm.parm.capture.timeperframe.numerator;
		*width = bc->vfmt.fmt.pix.width;
		*height = bc->vfmt.fmt.pix.height;
		codec_id = bc_rec->codec_id;
	} else if ((bc->cam_caps & BC_CAM_CAP_RTSP) && bc->rtp_sess.video_stream_index >= 0) {
		AVStream *stream = bc->rtp_sess.ctx->streams[bc->rtp_sess.video_stream_index];
		*fnum = stream->time_base.num;
		*fden = stream->time_base.den; /* XXX is this correct? */
		*width = stream->codec->width;
		*height = stream->codec->height;
		codec_id = stream->codec->codec_id;
	} else {
		return -1;
	}

	if (buf == NULL || size <= 0)
		return 0;

	/* Decode the first picture to get frame size */
	codec = avcodec_find_decoder(codec_id);
	if (!codec)
		return 0;

	c = avcodec_alloc_context();
	c->time_base.num = *fnum;
	c->time_base.den = *fden;

	picture = avcodec_alloc_frame();
	av_init_packet(&packet);
	packet.data = buf;
	packet.size = size;
	packet.flags = AV_PKT_FLAG_KEY; // XXX is this correct? necessary?

	if (avcodec_open(c, codec) < 0) {
		codec = NULL;
		goto pic_info_fail;
	}

	len = avcodec_decode_video2(c, picture, &got_picture, &packet);
	
	if (len < 0 || !got_picture)
		goto pic_info_fail;

	*width = c->width;
	*height = c->height;

	bc_save_jpeg(bc_rec, c, picture);

pic_info_fail:
	if (c != NULL) {
		if (codec)
			avcodec_close(c);
		av_freep(&c);
	}
	if (picture != NULL)
		av_freep(&picture);
	av_free_packet(&packet);

	return 0;
}

static int setup_solo_output(struct bc_record *bc_rec, AVFormatContext *oc)
{
	AVStream *st;
	int width = 0, height = 0, fnum, fden;

	if (bc_get_frame_info(bc_rec, &width, &height, &fnum, &fden))
		return -1;

	/* Setup new video stream */
	if ((bc_rec->video_st = av_new_stream(oc, 0)) == NULL)
		return -1;
	st = bc_rec->video_st;

	st->time_base.den = fden;
	st->time_base.num = fnum;

	if (bc_rec->codec_id == CODEC_ID_NONE) {
		bc_dev_warn(bc_rec, "Invalid Video Format, assuming MP4V-ES");
		st->codec->codec_id = CODEC_ID_MPEG4;
	} else
		st->codec->codec_id = bc_rec->codec_id;

	/* h264 requires us to work around libavcodec broken defaults */
	if (st->codec->codec_id == CODEC_ID_H264) {
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

	st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
	st->codec->pix_fmt = PIX_FMT_YUV420P;
	st->codec->width = width;
	st->codec->height = height;
	st->codec->time_base.num = fnum;
	st->codec->time_base.den = fden;

	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	st = NULL;

	/* We don't fail when this happens. Video with no sound is
	 * better than no video at all. */
	if (bc_alsa_open(bc_rec))
		bc_alsa_close(bc_rec);

	/* Setup new audio stream */
	if (has_audio(bc_rec)) {
		enum CodecID codec_id = CODEC_ID_PCM_S16LE;

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
		st->codec->codec_type = AVMEDIA_TYPE_AUDIO;

		st->codec->sample_rate = 8000;
		st->codec->sample_fmt = SAMPLE_FMT_S16;
		st->codec->channels = 1;
		st->codec->time_base = (AVRational){1, 8000};

		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
			st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
no_audio:
		st = NULL;
	}
	
	return 0;
}

int bc_open_avcodec(struct bc_record *bc_rec)
{
	struct bc_handle *bc = bc_rec->bc;
	AVCodec *codec;
	AVStream *st;
	AVFormatContext *oc;
	int i;

	if (bc_rec->oc != NULL)
		return 0;

	bc_start_media_entry(bc_rec);

	if (bc_rec->media == BC_MEDIA_NULL) {
		errno = ENOMEM;
		goto error;
	}

	/* Get the output format */
	bc_rec->fmt_out = av_guess_format(NULL, bc_rec->outfile, NULL);
	if (!bc_rec->fmt_out) {
		errno = EINVAL;
		goto error;
	}

	if ((bc_rec->oc = avformat_alloc_context()) == NULL)
		goto error;
	oc = bc_rec->oc;

	oc->oformat = bc_rec->fmt_out;
	snprintf(oc->filename, sizeof(oc->filename), "%s", bc_rec->outfile);
	/* snprintf(oc->title, sizeof(oc->title), "BC: %s", bc_rec->name); XXX replacement in 0.8? */

	if (bc->cam_caps & BC_CAM_CAP_RTSP) {
		/* We don't need this information for RTSP, but this generates the event JPEG frame, so
		 * it should be called anyway (refactor would be nice). */
		int width = 0, height = 0, fnum, fden;
		if (bc_get_frame_info(bc_rec, &width, &height, &fnum, &fden)) {
			errno = ENOMEM;
			goto error;
		}
		
		/* Don't include audio in the output if it's disabled by the user */
		if (!has_audio(bc_rec))
			bc->rtp_sess.audio_stream_index = -1;
	
		if (rtp_session_setup_output(&bc->rtp_sess, oc) < 0)
			goto error;
		
		bc_rec->audio_st = bc_rec->video_st = NULL;
		for (i = 0; i < oc->nb_streams; ++i) {
			if (oc->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
				bc_rec->video_st = oc->streams[i];
			else if (oc->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
				bc_rec->audio_st = oc->streams[i];
		}
	} else {
		if (setup_solo_output(bc_rec, oc) < 0)
			goto error;
	}

	if (av_set_parameters(oc, NULL) < 0)
		goto error;

	/* Open Video output */
	st = bc_rec->video_st;
	codec = avcodec_find_encoder(st->codec->codec_id);
	if (codec == NULL || avcodec_open(st->codec, codec) < 0) {
		bc_rec->video_st = NULL;
		/* Clear this */
		if (bc_rec->audio_st)
			bc_rec->audio_st = NULL;
		goto error;
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
		goto error;
	}

	av_write_header(oc);

	return 0;
	
error:
	/* XXX I dislike this. */
	bc_media_destroy(&bc_rec->media);
	bc_close_avcodec(bc_rec);
	return -1;
}

