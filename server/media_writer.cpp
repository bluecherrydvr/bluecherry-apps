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

#include "media_writer.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <string>

extern "C" {
#include <libavutil/mathematics.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include "bc-server.h"
#include "lavf_device.h"

media_writer::media_writer()
	: oc(0), video_st(0), audio_st(0),
	last_video_pts(0), last_video_dts(0),
	last_audio_pts(0), last_audio_dts(0)
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
	else
		bc_log(Debug, "write_packet: unexpected packet (to be ignored): pkt.type %d, dts %" PRId64 ", pts %" PRId64 ", size %d", pkt.type, pkt.dts, pkt.pts, pkt.size);

	if (!s)
		return true;

	av_init_packet(&opkt);
	opkt.flags        = pkt.flags;
	opkt.pts          = av_rescale_q_rnd(pkt.pts, AV_TIME_BASE_Q, s->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
	opkt.dts          = av_rescale_q_rnd(pkt.dts, AV_TIME_BASE_Q, s->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
	opkt.data         = const_cast<uint8_t*>(pkt.data());
	opkt.size         = pkt.size;
	opkt.stream_index = s->index;

	/* Fix non-increasing timestamps */
	if (pkt.type == AVMEDIA_TYPE_AUDIO) {
		if (opkt.pts < last_audio_pts || opkt.dts < last_audio_dts) {
			opkt.pts = last_audio_pts + 1;
			opkt.dts = last_audio_dts + 1;
			bc_log(Debug, "fixing timestamps in audio packet");
		}

		last_audio_pts = opkt.pts;
		last_audio_dts = opkt.dts;
	}
	if (pkt.type == AVMEDIA_TYPE_VIDEO) {
		if (opkt.pts < last_video_pts || opkt.dts < last_video_dts) {
			opkt.pts = last_video_pts + 1;
			opkt.dts = last_video_dts + 1;
			bc_log(Debug, "fixing timestamps in video packet");
		}

		last_video_pts = opkt.pts;
		last_video_dts = opkt.dts;
	}


	bc_log(Debug, "av_interleaved_write_frame: dts=%" PRId64 " pts=%" PRId64 " tb=%d/%d s_i=%d k=%d", opkt.dts, opkt.pts, oc->streams[opkt.stream_index]->time_base.num, oc->streams[opkt.stream_index]->time_base.den, opkt.stream_index, !!(opkt.flags & AV_PKT_FLAG_KEY));
	re = av_interleaved_write_frame(oc, &opkt);
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
	video_st = audio_st = NULL;

	if (oc) {
		if (oc->pb)
			av_write_trailer(oc);

		if (oc->pb)
			avio_close(oc->pb);
		avformat_free_context(oc);
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
	if (bc->type == BC_DEVICE_LAVF)
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
	AVDictionary *muxer_opts = NULL;
	AVRational bkp_ts;

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
	bkp_ts = video_st->codec->time_base;
	properties.video.apply(video_st->codec);
	video_st->codec->time_base = bkp_ts;

	if (properties.has_audio()) {
		audio_st = avformat_new_stream(oc, NULL);
		if (!audio_st)
			goto error;
		bkp_ts = audio_st->codec->time_base;
		properties.audio.apply(audio_st->codec);
		audio_st->codec->time_base = bkp_ts;
	}

	if (oc->oformat->flags & AVFMT_GLOBALHEADER) {
		video_st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
		if (audio_st)
			audio_st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	/* Open output file */
	if ((ret = avio_open(&oc->pb, path.c_str(), AVIO_FLAG_WRITE)) < 0) {
		char error[512];
		av_strerror(ret, error, sizeof(error));
		bc_log(Error, "Cannot open media output file %s: %s (%d)",
			   path.c_str(), error, ret);
		goto error;
	}

	this->file_path = path;

	av_dict_set(&muxer_opts, "avoid_negative_ts", "+make_zero", 0);

	ret = avformat_write_header(oc, &muxer_opts);
	av_dict_free(&muxer_opts);
	if (ret) {
		char error[512];
		av_strerror(ret, error, sizeof(error));
		bc_log(Error, "Failed to init muxer for output file %s: %s (%d)",
				file_path.c_str(), error, ret);
		avio_closep(&oc->pb);
		goto error;
	}

	return 0;

error:
	close();
	return -1;
}

int media_writer::snapshot_create(const char *outfile, const stream_packet &pkt)
{
	int ret;

	snapshot_filename = outfile;
	// Initialize decoder
	ret = snapshot_decoder_init(pkt);
	if (ret)
		return ret;

	// Feed decoder
	return snapshot_feed(pkt);
}

/*
 * This function automatically cleans up decoder in any case when its usage is
 * not needed/possible anymore
 */
int media_writer::snapshot_feed(const stream_packet &pkt)
{
	int ret;
	AVFrame *frame = NULL;

	// Feed decoder
	ret = snapshot_decode_frame(pkt, &frame);
	if (!ret) {
		bc_log(Debug, "Fed snapshot decoder with a frame and got picture");
		assert (frame);
	} else {
		if (ret < 0) {
			bc_log(Error, "Feeding snapshot decoder failed");
			snapshot_decoder_cleanup();
		} else if (ret > 0) {
			bc_log(Debug, "Fed this frame to snapshot decoder, but it gave no picture - it needs more");
		}
		return ret;
	}

	// If decoder gave picture, snapshot_encode_write()
	ret = snapshot_encode_write(frame);

	// Unref the decoded frame
	av_frame_free(&frame);

	if (ret) {
		bc_log(Error, "Encoding and writing snapshot file failed");
	}
	snapshot_decoder_cleanup();
	return ret;
}

int media_writer::snapshot_decoder_init(const stream_packet &pkt)
{
	AVDictionary *decoder_opts = NULL;
	std::shared_ptr<const stream_properties> properties = pkt.properties();
	AVCodec *codec = avcodec_find_decoder(properties->video.codec_id);
	int ret;
	if (!codec || !(snapshot_decoder = avcodec_alloc_context3(codec))) {
		bc_log(Error, "Failed to allocate snapshot decoder context");
		return -1;
	}

	properties->video.apply(snapshot_decoder);

	av_dict_set(&decoder_opts, "refcounted_frames", "1", 0);
	ret = avcodec_open2(snapshot_decoder, codec, &decoder_opts);
	av_dict_free(&decoder_opts);
	if (ret < 0) {
		bc_log(Error, "Failed to initialize snapshot decoder context");
		av_free(snapshot_decoder);
	}
	return ret;
}

/*
 * @return negative on failure, 0 if we have picture, positive if no picture so far
 */
int media_writer::snapshot_decode_frame(const stream_packet &pkt, AVFrame **frame_arg)
{
	int ret;
	int have_picture;
	AVPacket packet;
	AVFrame *frame = av_frame_alloc();
	av_init_packet(&packet);
	packet.flags        = pkt.is_key_frame() ? AV_PKT_FLAG_KEY : 0;
	/* Setting DTS from PTS is silly, but in curent scheme DTS is lost.
	 * Should work for decoding one frame right after keyframe (most of time?) */
	packet.dts          = pkt.pts;
	packet.pts          = pkt.pts;
	packet.data         = const_cast<uint8_t*>(pkt.data());
	packet.size         = pkt.size;

	if (!frame)
		return AVERROR(ENOMEM);

	ret = avcodec_decode_video2(snapshot_decoder, frame, &have_picture, &packet);
	if (ret < 0) {
		char error[512];
		av_strerror(ret, error, sizeof(error));
		av_frame_free(&frame);
		bc_log(Warning, "snapshot_decode_frame: decode video frame failed: %s", error);
		return ret;
	}

	if (have_picture) {
		*frame_arg = frame;
		return 0;
	} else {
		av_frame_free(&frame);
		return 1;
	}
}

/*
 * @return negative on error, 0 otherwise
 */
int media_writer::snapshot_encode_write(AVFrame *rawFrame)
{
	AVFrame *frame = av_frame_alloc();
	AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
	AVCodecContext *oc = NULL;
	FILE *file = fopen(snapshot_filename.c_str(), "w");
	int re = -1;
	int ret;
	bool avpicture_allocated = false;
	int got_pkt;
	AVPacket avpkt;

	av_init_packet(&avpkt);
	avpkt.data = NULL;
	avpkt.size = 0;

	if (!file) {
		bc_log(Error, "Failed to open file '%s' to save snapshot", snapshot_filename.c_str());
		goto end;
	}

	if (!codec || !(oc = avcodec_alloc_context3(codec))) {
		bc_log(Bug, "Failed to allocate encoder context for snapshot");
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
		sws = sws_getCachedContext(0, rawFrame->width, rawFrame->height, (AVPixelFormat)rawFrame->format,
		                           rawFrame->width, rawFrame->height, AV_PIX_FMT_YUVJ420P,
		                           SWS_BICUBIC, NULL, NULL, NULL);
		if (!sws) {
			bc_log(Bug, "Failed to convert pixel format for JPEG (format is %d)", rawFrame->format);
			goto end;
		}

		ret = av_image_alloc(frame->data, frame->linesize, rawFrame->width, rawFrame->height, AV_PIX_FMT_YUVJ420P, 4);
		if (ret < 0) {
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
		av_freep(&frame->data[0]);
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
		bc_log(Error, "Failed to write snapshot file: %s", strerror(errno));
		goto end;
	}

	re = 0;
end:
	if (file)
		fclose(file);
	av_free_packet(&avpkt);
	av_frame_free(&frame);
	if (oc) {
		// Flush encoder
		while (1) {
			av_init_packet(&avpkt);
			ret = avcodec_encode_video2(oc, &avpkt, NULL, &got_pkt);
			if (!got_pkt || ret < 0)
				break;
			av_free_packet(&avpkt);
		}
		avcodec_close(oc);
		av_free(oc);
	}
	return re;
}

void media_writer::snapshot_decoder_cleanup(void) {
	av_freep(&snapshot_decoder->extradata);
	snapshot_decoder->extradata_size = 0;
	avcodec_close(snapshot_decoder);
	av_freep(&snapshot_decoder);
}
