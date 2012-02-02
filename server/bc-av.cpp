/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

extern "C" {
#include "libavutil/mathematics.h"
}

#include "bc-server.h"

int bc_av_lockmgr(void **mutex_p, enum AVLockOp op)
{
	pthread_mutex_t **mutex = (pthread_mutex_t**)mutex_p;
	switch (op) {
		case AV_LOCK_CREATE:
			*mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
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

	if (bc_rec->cfg.aud_disabled)
		return 0;

	if (bc_rec->pcm != NULL)
		return 1;

	if ((bc->type == BC_DEVICE_RTP) && bc->rtp.audio_stream_index >= 0)
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

	if (bc->type != BC_DEVICE_V4L2)
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

	if ((err = snd_pcm_hw_params_set_format(pcm, params, (snd_pcm_format_t)fmt)) < 0) {
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

/* The returned packet is only valid until the next bc_buf_get or the next call
 * to this function. The data must be copied to remain valid.
 *
 * Return value is -1 for error, 0 for no packet, 1 for packet written
 */
int get_output_audio_packet(struct bc_record *bc_rec, struct bc_output_packet *pkt)
{
	if (!has_audio(bc_rec))
		return 0;

	pkt->type     = AVMEDIA_TYPE_AUDIO;
	pkt->ts_clock = time(NULL);

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
			return -1;
		}

		pkt->size = g723_decode(&bc_rec->g723_state, bc_rec->g723_data,
		                        size, bc_rec->pcm_data);
		pkt->data = (void *)bc_rec->pcm_data;

		pkt->flags = AV_PKT_FLAG_KEY;
		pkt->pts   = AV_NOPTS_VALUE;
	} else if (bc_rec->bc->type == BC_DEVICE_RTP) {
		struct rtp_device *rs = &bc_rec->bc->rtp;

		if (rs->frame.stream_index != rs->audio_stream_index)
			return 0;

		pkt->data  = rs->frame.data;
		pkt->size  = rs->frame.size;
		pkt->flags = rs->frame.flags;
		pkt->pts   = rs->frame.pts;
	} else
		return 0;

	return 1;
}

int get_output_video_packet(struct bc_record *bc_rec, struct bc_output_packet *pkt)
{
	struct bc_handle *bc = bc_rec->bc;
	if (!bc_buf_is_video_frame(bc))
		return 0;

	pkt->ts_clock = time(NULL);
	pkt->type     = AVMEDIA_TYPE_VIDEO;
	pkt->pts      = AV_NOPTS_VALUE;
	if (bc_buf_key_frame(bc))
		pkt->flags = AV_PKT_FLAG_KEY;
	else
		pkt->flags = 0;

	pkt->data = bc_buf_data(bc);
	pkt->size = bc_buf_size(bc);

	if (!pkt->data || !pkt->size)
		return 0;

	if (bc->type == BC_DEVICE_RTP)
		pkt->pts = bc->rtp.frame.pts;

	return 1;
}

int bc_output_packet_write(struct bc_record *bc_rec, struct bc_output_packet *pkt)
{
	AVStream *s;
	AVPacket opkt;
	int re;

	if (pkt->type == AVMEDIA_TYPE_AUDIO)
		s = bc_rec->audio_st;
	else
		s = bc_rec->video_st;

	if (!s)
		return 0;

	av_init_packet(&opkt);
	opkt.flags        = pkt->flags;
	opkt.pts          = pkt->pts;
	opkt.data         = (uint8_t*)pkt->data;
	opkt.size         = pkt->size;
	opkt.stream_index = s->index;

	if (bc_rec->bc->type == BC_DEVICE_V4L2 && opkt.pts == AV_NOPTS_VALUE) {
		if (s->codec->coded_frame && s->codec->coded_frame->pts != AV_NOPTS_VALUE)
			opkt.pts = av_rescale_q(s->codec->coded_frame->pts, s->codec->time_base,
			                        s->time_base); 
	}

	if (bc_rec->bc->type == BC_DEVICE_RTP && opkt.pts != AV_NOPTS_VALUE) {
		struct rtp_device *rs = &bc_rec->bc->rtp;
		/* RTP packets must be scaled to the output PTS */
		if (pkt->type == AVMEDIA_TYPE_AUDIO) {
			opkt.pts = av_rescale_q(opkt.pts,
			                        rs->ctx->streams[rs->audio_stream_index]->time_base,
			                        s->time_base);
		} else {
			/* RTP is at a constant 90KHz time scale */
			opkt.pts = av_rescale_q(opkt.pts, (AVRational){1, 90000}, s->time_base);
		}
	}

	if (bc_rec->output_pts_base && opkt.pts != AV_NOPTS_VALUE && bc_rec->bc->type == BC_DEVICE_RTP)
		opkt.pts -= av_rescale_q(bc_rec->output_pts_base, (AVRational){1, 90000}, s->time_base);

	/* Cutoff points can result in a few negative PTS frames, because often
	 * the video will be cut before the audio for that time has been written.
	 * We can drop these; they won't be played back, other than a very trivial
	 * amount of time at the beginning of a recording. */
	if (pkt->pts != AV_NOPTS_VALUE && pkt->pts < 0) {
		av_log(bc_rec->oc, AV_LOG_INFO, "Dropping frame with negative pts %lld, probably "
		       "caused by recent PTS reset", (long long int)pkt->pts);
		return 0;
	}

	re = av_write_frame(bc_rec->oc, &opkt);
	if (re != 0) {
		char err[512] = { 0 };
		av_strerror(re, err, sizeof(err));
		bc_dev_err(bc_rec, "Error when writing frame to recording: %s", err);
		return -1;
	}

	return 1;
}

int bc_output_packet_copy(struct bc_output_packet *dst, const struct bc_output_packet *src)
{
	if (dst != src)
		memcpy(dst, src, sizeof(struct bc_output_packet));

	if (src->size) {
		dst->data = malloc(src->size);
		if (!dst->data)
			return -1;
		memcpy(dst->data, src->data, src->size);
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
	bc_rec->output_pts_base = 0;

	if (bc_rec->oc) {
		if (bc_rec->oc->pb)
			av_write_trailer(bc_rec->oc);

		for (i = 0; i < bc_rec->oc->nb_streams; i++) {
			av_freep(&bc_rec->oc->streams[i]->codec);
			av_freep(&bc_rec->oc->streams[i]);
		}

		if (bc_rec->oc->pb)
			avio_close(bc_rec->oc->pb);
		av_free(bc_rec->oc);
		bc_rec->oc = NULL;
	}
}

int bc_mkdir_recursive(char *path)
{
	char *t;

	if (!mkdir(path, 0750) || errno == EEXIST)
		return 0;
	else if (errno != ENOENT)
		return -1;

	/* Try to make the parent directory */
	t = strrchr(path, '/');
	if (t == NULL || t == path)
		return -1;
	*t = '\0';
	if (bc_mkdir_recursive(path))
		return -1;
	*t = '/';

	return mkdir(path, 0750);
}

static int setup_solo_output(struct bc_record *bc_rec, AVFormatContext *oc)
{
	AVStream *st;
	int fden   = bc_rec->bc->v4l2.vparm.parm.capture.timeperframe.denominator;
	int fnum   = bc_rec->bc->v4l2.vparm.parm.capture.timeperframe.numerator;
	int width  = bc_rec->bc->v4l2.vfmt.fmt.pix.width;
	int height = bc_rec->bc->v4l2.vfmt.fmt.pix.height;

	/* Setup new video stream */
	if ((bc_rec->video_st = avformat_new_stream(oc, NULL)) == NULL)
		return -1;
	st = bc_rec->video_st;

	st->time_base.den = fden;
	st->time_base.num = fnum;

	if (bc_rec->bc->v4l2.codec_id == CODEC_ID_NONE) {
		bc_dev_warn(bc_rec, "Invalid Video Format, assuming MP4V-ES");
		st->codec->codec_id = CODEC_ID_MPEG4;
	} else
		st->codec->codec_id = bc_rec->bc->v4l2.codec_id;

	/* h264 requires us to work around libavcodec broken defaults */
	if (st->codec->codec_id == CODEC_ID_H264) {
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
	if (!bc_rec->cfg.aud_disabled && bc_alsa_open(bc_rec))
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

		if ((bc_rec->audio_st = avformat_new_stream(oc, NULL)) == NULL)
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

	/* Get the output format */
	bc_rec->fmt_out = av_guess_format("matroska", NULL, NULL);
	if (!bc_rec->fmt_out) {
		errno = EINVAL;
		goto error;
	}

	if ((bc_rec->oc = avformat_alloc_context()) == NULL)
		goto error;
	oc = bc_rec->oc;

	oc->oformat = bc_rec->fmt_out;

	if (bc->type == BC_DEVICE_RTP) {
		if (rtp_device_setup_output(&bc->rtp, oc) < 0)
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

	/* Open Video output */
	st = bc_rec->video_st;
	codec = avcodec_find_encoder(st->codec->codec_id);
	if (codec == NULL || avcodec_open2(st->codec, codec, NULL) < 0) {
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
		if (codec == NULL || avcodec_open2(st->codec, codec, NULL) < 0) {
			bc_rec->audio_st = NULL;
			goto error;
		}
	}

	/* Open output file */
	if (avio_open(&oc->pb, bc_rec->outfile, URL_WRONLY) < 0) {
		bc_dev_err(bc_rec, "Failed to open outfile (perms?): %s",
			   bc_rec->outfile);
		goto error;
	}

	avformat_write_header(oc, NULL);

	return 0;
	
error:
	bc_close_avcodec(bc_rec);
	return -1;
}

