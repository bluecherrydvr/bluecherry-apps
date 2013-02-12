/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include "media_writer.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string>

extern "C" {
#include <libavutil/mathematics.h>
#include <libswscale/swscale.h>
}

#include "bc-server.h"
#include "rtp_device.h"

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

media_writer::media_writer()
	: oc(0), video_st(0), audio_st(0), output_pts_base(AV_NOPTS_VALUE)
{
}

media_writer::~media_writer()
{
	close();
}

bool media_writer::write_packet(const stream_packet &pkt)
{
	AVStream *s = 0;
	AVPacket opkt;
	int re;

	if (pkt.type == AVMEDIA_TYPE_AUDIO)
		s = audio_st;
	else if (pkt.type == AVMEDIA_TYPE_VIDEO)
		s = video_st;

	if (!s)
		return false;

	av_init_packet(&opkt);
	opkt.flags        = pkt.flags;
	opkt.pts          = pkt.pts;
	opkt.data         = const_cast<uint8_t*>(pkt.data());
	opkt.size         = pkt.size;
	opkt.stream_index = s->index;

	if (opkt.pts == (int64_t)AV_NOPTS_VALUE) {
		if (s->codec->coded_frame && s->codec->coded_frame->pts != (int64_t)AV_NOPTS_VALUE)
			opkt.pts = av_rescale_q(s->codec->coded_frame->pts, s->codec->time_base,
			                        s->time_base); 
	} else {
		if (output_pts_base == (int64_t)AV_NOPTS_VALUE) {
			output_pts_base = opkt.pts;
			bc_log(Debug, "Setting writer pts base for stream to %"PRId64"", output_pts_base);
		}

		/* Subtract output_pts_base, both in the universal AV_TIME_BASE
		 * and synchronized across all related streams. */
		opkt.pts -= output_pts_base;
		/* Convert to output time_base */
		opkt.pts = av_rescale_q(opkt.pts, AV_TIME_BASE_Q, s->time_base);
	}

	/* Cutoff points can result in a few negative PTS frames, because often
	 * the video will be cut before the audio for that time has been
	 * written.  We can drop these; they won't be played back, other than a
	 * very trivial amount of time at the beginning of a recording. */
	if (opkt.pts != (int64_t)AV_NOPTS_VALUE && opkt.pts < 0) {
		bc_log(Debug, "Dropping frame with negative pts %"PRId64", "
		       "probably caused by recent PTS reset", opkt.pts);
		return true;
	}

	re = av_write_frame(oc, &opkt);
	if (re != 0) {
		char err[512] = { 0 };
		av_strerror(re, err, sizeof(err));
		bc_log(Error, "Error when writing frame to recording: %s", err);
		return false;
	}

	return true;
}

void media_writer::close()
{
	if (video_st)
		avcodec_close(video_st->codec);
	if (audio_st)
		avcodec_close(audio_st->codec);
	video_st = audio_st = NULL;
	output_pts_base = AV_NOPTS_VALUE;

	if (oc) {
		if (oc->pb)
			av_write_trailer(oc);

		for (unsigned i = 0; i < oc->nb_streams; i++) {
			av_freep(&oc->streams[i]->codec);
			av_freep(&oc->streams[i]);
		}

		if (oc->pb)
			avio_close(oc->pb);
		av_free(oc);
		oc = NULL;
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

	bc_log(Error, "mkdir_recursive: path too deep: %s", path);

 error:
	/* Revert path string to it's original state */
	while (depth--)
		*bp[depth] = '/';

	return -1;
}

int media_writer::open(const std::string &path, const stream_properties &properties)
{
	AVCodec *codec;

	if (oc)
		return 0;

	/* Get the output format */
	AVOutputFormat *fmt_out = av_guess_format("matroska", NULL, NULL);
	if (!fmt_out) {
		errno = EINVAL;
		goto error;
	}

	if ((oc = avformat_alloc_context()) == NULL)
		goto error;

	oc->oformat = fmt_out;

	video_st = avformat_new_stream(oc, NULL);
	if (!video_st)
		goto error;
	properties.video.apply(video_st->codec);

	if (properties.has_audio()) {
		audio_st = avformat_new_stream(oc, NULL);
		if (!audio_st)
			goto error;
		properties.audio.apply(audio_st->codec);
	}

	if (oc->oformat->flags & AVFMT_GLOBALHEADER) {
		video_st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
		if (audio_st)
			audio_st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	/* Open Video output */
	codec = avcodec_find_encoder(video_st->codec->codec_id);
	if (!codec || avcodec_open2(video_st->codec, codec, NULL) < 0) 
		goto error;

	/* Open Audio output */
	if (audio_st) {
		codec = avcodec_find_encoder(audio_st->codec->codec_id);
		if (!codec || avcodec_open2(audio_st->codec, codec, NULL) < 0)
			goto error;
	}

	/* Open output file */
	if (avio_open(&oc->pb, path.c_str(), URL_WRONLY) < 0) {
		bc_log(Error, "Cannot open media output file %s",
			   file_path.c_str());
		goto error;
	}

	this->file_path = path;

	avformat_write_header(oc, NULL);

	return 0;
	
error:
	close();
	return -1;
}

int media_writer::decode_one_packet(const stream_packet &pkt, AVFrame *frame)
{
	AVCodecContext *ic = 0;
	std::shared_ptr<const stream_properties> properties = pkt.properties();
	AVCodec *codec = avcodec_find_decoder(properties->video.codec_id);
	AVFrame tmpFrame;
	int re = -1;
	int have_picture = 0;
	if (!codec || !(ic = avcodec_alloc_context3(codec))) {
		bc_log(Warning, "decode_one_packet: cannot allocate decoder context for video");
		return -1;
	}

	properties->video.apply(ic);

	AVPacket packet;
	av_init_packet(&packet);
	packet.flags        = AV_PKT_FLAG_KEY;
	packet.pts          = 0;
	packet.data         = const_cast<uint8_t*>(pkt.data());
	packet.size         = pkt.size;

	if (avcodec_open2(ic, codec, NULL) < 0)
		goto end;

	re = avcodec_decode_video2(ic, &tmpFrame, &have_picture, &packet);
	if (re < 0) {
		char error[512];
		av_strerror(re, error, sizeof(error));
		bc_log(Warning, "decode_one_packet: cannot decode video frame: %s", error);
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

int media_writer::snapshot(const std::string &snapshot_file, const stream_packet &pkt)
{
	AVFrame rawFrame, frame;
	if (decode_one_packet(pkt, &rawFrame) < 1) {
		bc_log(Info, "snapshot: no video frame for snapshot");
		return -1;
	}

	AVCodec *codec = avcodec_find_encoder(CODEC_ID_MJPEG);
	AVCodecContext *oc = 0;
	uint8_t *buf = 0, *swsBuf = 0;
	FILE *file = 0;
	int size, re = -1;

	if (!codec || !(oc = avcodec_alloc_context3(codec))) {
		bc_log(Bug, "snapshot: cannot allocate encoder context for snapshot");
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
			bc_log(Bug, "snapshot: cannot convert pixel format for JPEG (format is %d)", rawFrame.format);
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
		bc_log(Bug, "snapshot: JPEG encoding failed: %s", error);
		goto end;
	}

	file = fopen(snapshot_file.c_str(), "w");
	if (!file) {
		bc_log(Error, "snapshot: cannot create file: %s", strerror(errno));
		goto end;
	}

	if (fwrite(buf, 1, size, file) < (unsigned)size || fclose(file)) {
		bc_log(Error, "snapshot: cannot write snapshot file: %s", strerror(errno));
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
