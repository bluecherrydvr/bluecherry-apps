/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include "media_writer.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <string>

extern "C" {
#include <libavutil/mathematics.h>
#include <libavutil/error.h>
#include <libswscale/swscale.h>
}

#include "bc-server.h"
#include "rtp_device.h"

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
			bc_log(Debug, "Setting writer pts base for stream to %" PRId64, output_pts_base);
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
		bc_log(Debug, "Dropping frame with negative pts %" PRId64 ", "
		       "probably caused by recent PTS reset", opkt.pts);
		return true;
	}

	re = av_write_frame(oc, &opkt);
	if (re < 0) {
		char err[512] = { 0 };
		av_strerror(re, err, sizeof(err));
		bc_log(Error, "Error writing frame to recording: %s", err);
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

#if 0 /* XXX unused */
static int setup_solo_output(struct bc_record *bc_rec, AVFormatContext *oc)
{
	AVStream *st;
	int fden   = bc_rec->bc->v4l2.vparm.parm.capture.timeperframe.denominator;
	int fnum   = bc_rec->bc->v4l2.vparm.parm.capture.timeperframe.numerator;
	int width  = bc_rec->bc->v4l2.vfmt.fmt.pix.width;
	int height = bc_rec->bc->v4l2.vfmt.fmt.pix.height;

	/* Setup new video stream */
	if ((st = avformat_new_stream(oc, NULL)) == NULL)
		return -1;

	st->time_base.den = fden;
	st->time_base.num = fnum;

	if (bc_rec->bc->v4l2.codec_id == AV_CODEC_ID_NONE) {
		bc_dev_warn(bc_rec, "Invalid CODEC, assuming H264");
		st->codec->codec_id = AV_CODEC_ID_H264;
	} else
		st->codec->codec_id = bc_rec->bc->v4l2.codec_id;

	/* h264 requires us to work around libavcodec broken defaults */
	if (st->codec->codec_id == AV_CODEC_ID_H264) {
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
	st->codec->pix_fmt = AV_PIX_FMT_YUV420P;
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
		enum AVCodecID codec_id = AV_CODEC_ID_PCM_S16LE;

		/* If we can't find an encoder, just skip it */
		if (avcodec_find_encoder(codec_id) == NULL) {
			bc_dev_warn(bc_rec, "Failed to find audio codec (%08x) "
				    "so not recording", codec_id);
			goto no_audio;
		}

		if ((st = avformat_new_stream(oc, NULL)) == NULL)
			goto no_audio;
		st->codec->codec_id = codec_id;
		st->codec->codec_type = AVMEDIA_TYPE_AUDIO;

		st->codec->sample_rate = 8000;
		st->codec->sample_fmt = AV_SAMPLE_FMT_S16;
		st->codec->channels = 1;
		st->codec->time_base = (AVRational){1, 8000};

		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
			st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
no_audio:
		st = NULL;
	}

	return 0;
}

int setup_output_context(struct bc_record *bc_rec, struct AVFormatContext *oc)
{
	struct bc_handle *bc = bc_rec->bc;
	if (bc->type == BC_DEVICE_RTP)
		return bc->input->setup_output(oc);
	else if (bc->type == BC_DEVICE_V4L2 && (bc->cam_caps & BC_CAM_CAP_SOLO))
		return setup_solo_output(bc_rec, oc);
	else
		return -1;
}
#endif

int media_writer::open(const std::string &path, const stream_properties &properties)
{
	int ret;
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
	if (avio_open(&oc->pb, path.c_str(), AVIO_FLAG_WRITE) < 0) {
		bc_log(Error, "Cannot open media output file %s",
			   file_path.c_str());
		goto error;
	}

	this->file_path = path;

	ret = avformat_write_header(oc, NULL);
	if (ret) {
		char error[512];
		av_strerror(ret, error, sizeof(error));
		bc_log(Error, "Failed to init muxer for output file %s: %s (%d)",
				file_path.c_str(), error, ret);
		goto error;
	}

	return 0;

error:
	close();
	return -1;
}

int media_writer::decode_one_packet(const stream_packet &pkt, AVFrame *frame)
{
	AVCodecContext *ic = 0;
	AVDictionary *decoder_opts = NULL;
	std::shared_ptr<const stream_properties> properties = pkt.properties();
	AVCodec *codec = avcodec_find_decoder(properties->video.codec_id);
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

	av_dict_set(&decoder_opts, "refcounted_frames", "1", 0);  // TODO Fix AVDictionary leak
	if (avcodec_open2(ic, codec, &decoder_opts) < 0)
		goto end;

	re = avcodec_decode_video2(ic, frame, &have_picture, &packet);
	if (re < 0) {
		char error[512];
		av_strerror(re, error, sizeof(error));
		bc_log(Warning, "decode_one_packet: cannot decode video frame: %s", error);
		goto end;
	}

end:
	avcodec_close(ic);
	av_free(ic);
	if (re < 0)
		return re;
	return have_picture;
}

int media_writer::snapshot(const char *outfile, const stream_packet &pkt)
{
	int ret;
	AVFrame *rawFrame = av_frame_alloc();
	AVFrame *frame = av_frame_alloc();
	AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
	AVCodecContext *oc = NULL;
	FILE *file = fopen(outfile, "w");
	int re = -1;
	bool avpicture_allocated = false;
	int got_pkt;
	AVPacket avpkt;

	av_init_packet(&avpkt);
	avpkt.data = NULL;
	avpkt.size = 0;

	if (decode_one_packet(pkt, rawFrame) < 1) {
		bc_log(Info, "snapshot: no video frame for snapshot");
		goto end;
	}

	if (!file) {
		bc_log(Error, "Failed to open file '%s' to save snapshot", outfile);
		goto end;
	}

	if (!codec || !(oc = avcodec_alloc_context3(codec))) {
		bc_log(Bug, "snapshot: cannot allocate encoder context for snapshot");
		goto end;
	}

	oc->width   = rawFrame->width;
	oc->height  = rawFrame->height;
	oc->pix_fmt = AV_PIX_FMT_YUVJ420P;
	oc->mb_lmin = oc->lmin = oc->qmin * FF_QP2LAMBDA;
	oc->mb_lmax = oc->lmax = oc->qmax * FF_QP2LAMBDA;
	oc->flags  |= CODEC_FLAG_QSCALE;
	oc->global_quality = oc->qmin * FF_QP2LAMBDA;
	oc->time_base.num = 1;
	oc->time_base.den = 30000;

	if (avcodec_open2(oc, codec, NULL) < 0)
		goto end;

	if (rawFrame->format != AV_PIX_FMT_YUVJ420P) {
		SwsContext *sws = 0;
		sws = sws_getCachedContext(0, rawFrame->width, rawFrame->height, (PixelFormat)rawFrame->format,
		                           rawFrame->width, rawFrame->height, AV_PIX_FMT_YUVJ420P,
		                           SWS_BICUBIC, NULL, NULL, NULL);
		if (!sws) {
			bc_log(Bug, "snapshot: cannot convert pixel format for JPEG (format is %d)", rawFrame->format);
			goto end;
		}

		ret = avpicture_alloc((AVPicture*)frame, AV_PIX_FMT_YUVJ420P, rawFrame->width, rawFrame->height);
		if (ret) {
			bc_log(Error, "Failed to allocate picture for encoding");
			goto end;
		}
		avpicture_allocated = true;
		sws_scale(sws, (const uint8_t**)rawFrame->data, rawFrame->linesize, 0, rawFrame->height,
		          frame->data, frame->linesize);
		sws_freeContext(sws);
	} else {
		av_frame_move_ref(frame, rawFrame);
	}

	ret = avcodec_encode_video2(oc, &avpkt, frame, &got_pkt);
	if (avpicture_allocated)
		avpicture_free((AVPicture*)frame);
	if (ret < 0) {
		char error[512];
		av_strerror(ret, error, sizeof(error));
		bc_log(Bug, "snapshot: JPEG encoding failed: %s", error);
		goto end;
	}

	if (!got_pkt) {
		bc_log(Bug, "snapshot: encoded frame, but got no packet on output");
		goto end;
	}

	assert(avpkt.size && avpkt.data);

	// TODO Use libavformat muxer (AVFormatContext) instead
	if (fwrite(avpkt.data, 1, avpkt.size, file) < (unsigned)avpkt.size) {
		bc_log(Error, "snapshot: cannot write snapshot file: %s", strerror(errno));
		goto end;
	}

	re = 0;
end:
	if (file)
		fclose(file);
	av_free_packet(&avpkt);
	av_frame_free(&rawFrame);
	av_frame_free(&frame);
	avcodec_close(oc);
	av_free(oc);
	return re;
}
