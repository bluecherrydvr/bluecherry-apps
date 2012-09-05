/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string>

extern "C" {
#include "libavutil/mathematics.h"
#include "libswscale/swscale.h"
}

#include "bc-server.h"
#include "rtp-session.h"

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

	if (bc->type == BC_DEVICE_RTP)
		return bc->input->has_audio();

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

int bc_output_packet_write(struct bc_record *bc_rec, const stream_packet &pkt)
{
	AVStream *s;
	AVPacket opkt;
	int re;

	if (pkt.type == AVMEDIA_TYPE_AUDIO)
		s = bc_rec->audio_st;
	else if (pkt.type == AVMEDIA_TYPE_VIDEO)
		s = bc_rec->video_st;
	else
		return -1;

	if (!s)
		return 0;

	av_init_packet(&opkt);
	opkt.flags        = pkt.flags;
	opkt.pts          = pkt.pts;
	opkt.data         = const_cast<uint8_t*>(pkt.data());
	opkt.size         = pkt.size;
	opkt.stream_index = s->index;

	if (bc_rec->bc->type == BC_DEVICE_V4L2 && opkt.pts == AV_NOPTS_VALUE) {
		if (s->codec->coded_frame && s->codec->coded_frame->pts != AV_NOPTS_VALUE)
			opkt.pts = av_rescale_q(s->codec->coded_frame->pts, s->codec->time_base,
			                        s->time_base); 
	}

	// XXX This is the wrong place for this logic.
	if (bc_rec->bc->type == BC_DEVICE_RTP && opkt.pts != AV_NOPTS_VALUE) {
		struct rtp_device *rs = reinterpret_cast<rtp_device*>(bc_rec->bc->input);
		/* RTP packets must be scaled to the output PTS */
		if (pkt.type == AVMEDIA_TYPE_AUDIO) {
			opkt.pts = av_rescale_q(opkt.pts,
			                        rs->audio_stream()->time_base,
			                        s->time_base);
		} else {
			opkt.pts = av_rescale_q(opkt.pts,
			                        rs->video_stream()->time_base,
			                        s->time_base);
		}

		// XXX This probably isn't correct when audio streams are present.
		if (bc_rec->output_pts_base) {
			opkt.pts -= av_rescale_q(bc_rec->output_pts_base,
			                         rs->video_stream()->time_base,
			                         s->time_base);
		}
	}

	/* Cutoff points can result in a few negative PTS frames, because often
	 * the video will be cut before the audio for that time has been written.
	 * We can drop these; they won't be played back, other than a very trivial
	 * amount of time at the beginning of a recording. */
	if (opkt.pts != AV_NOPTS_VALUE && opkt.pts < 0) {
		av_log(bc_rec->oc, AV_LOG_INFO, "Dropping frame with negative pts %lld, probably "
		       "caused by recent PTS reset", (long long int)opkt.pts);
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

#define MKDIR_RECURSIVE_DEPTH 25

/* XXX: this function doesn't belong here */
int bc_mkdir_recursive(char *path)
{
	unsigned int depth = 0;
	char *bp[MKDIR_RECURSIVE_DEPTH];

	while (depth <= MKDIR_RECURSIVE_DEPTH) {
		if (!mkdir(path, 0750) || errno == EEXIST) {
			if (!depth)
				return 0;

			/* Continue with next child */
			*bp[--depth] = '/';
			continue;
		}

		if (errno != ENOENT)
			goto error;

		/* Missing parent, try to make it */
		bp[depth] = strrchr(path, '/');
		if (!bp[depth] || bp[depth] == path) {
			errno = EINVAL;
			goto error;
		}

		*bp[depth++] = 0;
	}

	bc_log("W(mkdir): Path too deep: %s", path);

 error:
	/* Revert path string to it's original state */
	while (depth--)
		*bp[depth] = '/';

	return -1;
}

int bc_open_avcodec(struct bc_record *bc_rec)
{
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

	if (bc_rec->bc->input->setup_output(oc) < 0)
		goto error;

	bc_rec->audio_st = bc_rec->video_st = NULL;
	for (i = 0; i < oc->nb_streams; ++i) {
		if (oc->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			bc_rec->video_st = oc->streams[i];
		else if (oc->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			bc_rec->audio_st = oc->streams[i];
	}

	/* Open Video output */
	st = bc_rec->video_st;
	if (!st)
		goto error;
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

int decode_one_video_packet(struct bc_record *bc_rec, const stream_packet &pkt, AVFrame *frame)
{
	AVCodecContext *ic = 0;
	AVCodecContext *recctx = bc_rec->video_st->codec;
	AVCodec *codec = avcodec_find_decoder(recctx->codec_id);
	AVFrame tmpFrame;
	int re = -1;
	int have_picture = 0;
	if (!codec || !(ic = avcodec_alloc_context3(codec))) {
		bc_dev_err(bc_rec, "Cannot allocate decoder context for video");
		return -1;
	}

	ic->width     = recctx->width;
	ic->height    = recctx->height;
	ic->pix_fmt   = recctx->pix_fmt;
	ic->time_base = recctx->time_base;
	if (recctx->extradata && recctx->extradata_size) {
		ic->extradata_size = recctx->extradata_size;
		ic->extradata = (uint8_t*)av_malloc(recctx->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
		memcpy(ic->extradata, recctx->extradata, recctx->extradata_size);
	}

	AVPacket packet;
	av_init_packet(&packet);
	packet.flags        = pkt.flags;
	packet.pts          = pkt.pts;
	packet.data         = const_cast<uint8_t*>(pkt.data());
	packet.size         = pkt.size;

	if (avcodec_open2(ic, codec, NULL) < 0)
		goto end;

	re = avcodec_decode_video2(ic, &tmpFrame, &have_picture, &packet);
	if (re < 0) {
		char error[512];
		av_strerror(re, error, sizeof(error));
		bc_dev_err(bc_rec, "Cannot decode video frame: %s", error);
		goto end;
	}

	if (have_picture) {
		memcpy(frame, &tmpFrame, sizeof(AVFrame));
		int size = avpicture_get_size(ic->pix_fmt, ic->width, ic->height);
		uint8_t *buf = (uint8_t*) av_malloc(size);
		avpicture_fill((AVPicture*)frame, buf, ic->pix_fmt, ic->width, ic->height);
		av_picture_copy((AVPicture*)frame, (const AVPicture*)&tmpFrame, ic->pix_fmt,
		                ic->width, ic->height);
	}

	re = 0;
end:
	avcodec_close(ic);
	av_free(ic);
	if (re < 0)
		return re;
	return have_picture;
}

int save_event_snapshot(struct bc_record *bc_rec, const stream_packet &pkt)
{
	std::string filename = bc_rec->event->media.filepath;
	if (filename.empty()) {
		bc_dev_err(bc_rec, "No filename for snapshot");
		return -1;
	}
	filename.replace(filename.size()-3, 3, "jpg");

	AVFrame rawFrame, frame;
	if (decode_one_video_packet(bc_rec, pkt, &rawFrame) < 1) {
		bc_dev_err(bc_rec, "No video frame for snapshot");
		return -1;
	}

	AVCodec *codec = avcodec_find_encoder(CODEC_ID_MJPEG);
	AVCodecContext *oc = 0;
	uint8_t *buf = 0, *swsBuf = 0;
	FILE *file = 0;
	int size, re = -1;

	if (!codec || !(oc = avcodec_alloc_context3(codec))) {
		bc_dev_err(bc_rec, "Cannot allocate encoder context for snapshot");
		goto end;
	}

	oc->width   = rawFrame.width;
	oc->height  = rawFrame.height;
	oc->pix_fmt = PIX_FMT_YUVJ420P;
	oc->mb_lmin = oc->lmin = oc->qmin * FF_QP2LAMBDA;
	oc->mb_lmax = oc->lmax = oc->qmax * FF_QP2LAMBDA;
	oc->flags  |= CODEC_FLAG_QSCALE;
	oc->global_quality = oc->qmin * FF_QP2LAMBDA;
	oc->time_base.num = 1;
	oc->time_base.den = 30000;

	if (avcodec_open2(oc, codec, NULL) < 0)
		goto end;

	size = avpicture_get_size(PIX_FMT_YUVJ420P, oc->width, oc->height);
	memcpy(&frame, &rawFrame, sizeof(AVFrame));
	if (rawFrame.format != PIX_FMT_YUVJ420P) {
		SwsContext *sws = 0;
		sws = sws_getCachedContext(0, rawFrame.width, rawFrame.height, (PixelFormat)rawFrame.format,
		                           rawFrame.width, rawFrame.height, PIX_FMT_YUVJ420P,
		                           SWS_BICUBIC, NULL, NULL, NULL);
		if (!sws) {
			bc_dev_err(bc_rec, "Cannot convert pixel format for JPEG snapshot (format is %d)", rawFrame.format);
			goto end;
		}

		swsBuf = new uint8_t[size];
		avpicture_fill((AVPicture*)&frame, swsBuf, PIX_FMT_YUVJ420P, rawFrame.width, rawFrame.height);
		sws_scale(sws, (const uint8_t**)rawFrame.data, rawFrame.linesize, 0, rawFrame.height,
		          frame.data, frame.linesize);
		sws_freeContext(sws);
	}

	buf  = new uint8_t[size];
	size = avcodec_encode_video(oc, buf, size, &frame);
	if (size < 1) {
		char error[512];
		av_strerror(size, error, sizeof(error));
		bc_dev_err(bc_rec, "JPEG encoding failed: %s", error);
		goto end;
	}

	file = fopen(filename.c_str(), "w");
	if (!file) {
		bc_dev_err(bc_rec, "Cannot create snapshot file: %s", strerror(errno));
		goto end;
	}

	if (fwrite(buf, 1, size, file) < (unsigned)size || fclose(file)) {
		bc_dev_err(bc_rec, "Cannot write snapshot file: %s", strerror(errno));
		goto end;
	}

	file = 0;
	re   = 0;
end:
	if (file)
		fclose(file);
	delete[] buf;
	delete[] swsBuf;
	av_free(rawFrame.data[0]);
	avcodec_close(oc);
	av_free(oc);
	return re;
}

