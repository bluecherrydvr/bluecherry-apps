#include "reencoder.h"
#include "vaapi.h"


reencoder::reencoder(int bitrate, int out_frame_w, int out_frame_h, bool watermarking)
	: dec(0), wmr(0), enc_streaming(0), enc_recording(0), scl(0),
	last_decoded_fw(0), last_decoded_fh(0),
	out_frame_w(out_frame_w), out_frame_h(out_frame_h),
	streaming_bitrate(bitrate), recording_bitrate(0),
	incoming_bitrate_avg(0), avg_window_size(0),
	avg_time(0), incoming_packets_cnt(0),
	watermarking(watermarking)
{
	bc_log(Info, "new reencoder instance created, streaming bitrate %d,\
output streaming frame_width %dx%d, watermarking %s",
		bitrate, out_frame_w, out_frame_h, watermarking ? "enabled" : "disabled");

	if (watermarking && out_frame_w == -1)
		mode = REENC_MODE_WMR_ONLY;
	else if (watermarking && out_frame_w > 0)
		mode = REENC_MODE_WMR_AND_STREAMING;
	else if (!watermarking)
		mode = REENC_MODE_STREAMING_ONLY;

	if (!watermarking && (out_frame_w == -1 || out_frame_h == -1))
		bc_log(Fatal, "reencoder: invalid configuration");
}

reencoder::~reencoder()
{
	if (dec)
		delete dec;
	if (enc_streaming)
		delete enc_streaming;
	if (enc_recording)
		delete enc_recording;
	if (scl)
		delete scl;
}

void reencoder::update_streaming_bitrate(int new_bitrate)
{
	streaming_bitrate = new_bitrate;

	if (enc_streaming)
		enc_streaming->update_bitrate(new_bitrate);
}

bool reencoder::push_packet(const stream_packet &packet, bool reencode_for_streaming)
{

	if (!dec)
		dec = new decoder(AVMEDIA_TYPE_VIDEO);

	dec->push_packet(packet);

	return true;
}

bool reencoder::init_enc_streaming(AVFrame *in)
{
	if (!enc_streaming)
	{
		bc_log(Info, "creating encoder instance for streaming");

		enc_streaming = new encoder();
		/* alloc new hw_frames_ctx if needed??? */
		if (!enc_streaming->init_encoder(AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_H264, streaming_bitrate, in->width, in->height, in->hw_frames_ctx))
		{
			bc_log(Error, "Failed to initialize encoder instance");
			delete enc_streaming;
			enc_streaming = nullptr;
			return false;
		}
	}

	return true;
}

bool reencoder::init_enc_recording(AVFrame *in)
{
	if (!enc_recording)
	{
		bc_log(Info, "creating encoder instance for recording");

		enc_recording = new encoder();
		/* alloc new hw_frames_ctx if needed??? */ /* calculate recording bitrate!!! */
		if (!enc_recording->init_encoder(AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_H264, recording_bitrate, in->width, in->height, in->hw_frames_ctx))
		{
			bc_log(Error, "Failed to initialize encoder instance");
			delete enc_recording;
			enc_recording = nullptr;
			return false;
		}
	}

	return true;
}

bool reencoder::run_loop()
{
	AVFrame *frame;
	AVFrame *scaled_frame = NULL;
	AVFrame *watermarked_frame = NULL;
	bool result = false;

	/* 1) DECODE */
	frame = dec->decoded_frame();

	if (!frame)
		bc_log(Debug, "got no frame from decoder!");

	if (!frame)
		return false;

	bc_log(Debug, "reencoder: got frame %dx%d from decoder", frame->width, frame->height);

	/* 2) ADD WATERMARK */
	if (watermarking)
	{
		if (!wmr)
		{
			bc_log(Info, "creating new watermarker instance");

			wmr = new watermarker();

			const AVCodecContext *ctx = dec->get_ctx();

			if (!wmr->init_watermarker("DVR NAME", ctx))
			{
				bc_log(Error, "Failed to initialize watermarker instance");
				delete wmr;
				wmr = nullptr;
				return false;
			}
		}
	}

	/* 3) SCALE */
	if (!scl && out_frame_w > 0)
	{
		bc_log(Info, "creating new scaler instance for reencoding");

		scl = new scaler();

		const AVCodecContext *ctx = dec->get_ctx();

		if (!scl->init_scaler(out_frame_w, out_frame_h, ctx))
		{
			bc_log(Error, "Failed to initialize scaler instance for reencoding");
			delete scl;
			scl = NULL;
			return false;
		}
	}

	if (last_decoded_fw > 0 && (last_decoded_fw != frame->width || last_decoded_fh != frame->height))
	{
		const AVCodecContext *ctx = dec->get_ctx();

		if (scl)
			scl->reinitialize(ctx);

		if (enc_streaming)
		{
			delete enc_streaming;
			enc_streaming = nullptr;
		}

		if (enc_recording)
		{
			delete enc_recording;
			enc_recording = nullptr;
		}
	}

	last_decoded_fw = frame->width;
	last_decoded_fh = frame->height;

	if (watermarking)
	{
		wmr->push_frame(frame);

		watermarked_frame = wmr->watermarked_frame();

		if (watermarked_frame == NULL)
			return false;

		bc_log(Debug, "got frame size %d x %d from watermarked", watermarked_frame->width, watermarked_frame->height);
	}


	switch (mode)
	{
	case REENC_MODE_WMR_ONLY:

		if (!init_enc_recording(watermarked_frame))
			return false;

		enc_recording->push_frame(watermarked_frame);

		result = enc_recording->encode();

		av_frame_unref(watermarked_frame);

		break;
	case REENC_MODE_WMR_AND_STREAMING:

		if (!init_enc_recording(watermarked_frame))
			return false;

		enc_recording->push_frame(watermarked_frame);

		result = enc_recording->encode();

		scl->push_frame(watermarked_frame);

		scaled_frame = scl->scaled_frame();

		if (scaled_frame == NULL)
			return false;

		bc_log(Debug, "got frame of size %d x %d vaapi surface %#x from scaler", scaled_frame->width, scaled_frame->height, (uintptr_t)scaled_frame->data[3]);

		if (!init_enc_streaming(scaled_frame))
			return false;

		enc_streaming->push_frame(scaled_frame);

		enc_streaming->encode();

		av_frame_unref(scaled_frame);
		av_frame_unref(watermarked_frame);

		break;
	case REENC_MODE_STREAMING_ONLY:

		scl->push_frame(frame);

		scaled_frame = scl->scaled_frame();

		if (scaled_frame == NULL)
			return false;


		bc_log(Debug, "got frame of size %d x %d vaapi surface %#x from scaler", scaled_frame->width, scaled_frame->height, (uintptr_t)scaled_frame->data[3]);

		if (!init_enc_streaming(scaled_frame))
			return false;

		enc_streaming->push_frame(scaled_frame);

		result = enc_streaming->encode();

		av_frame_unref(scaled_frame);

		break;
	}

	return result;
}

const stream_packet &reencoder::streaming_packet() const
{
	return enc_streaming->packet();
}

const stream_packet &reencoder::recording_packet() const
{
	return enc_recording->packet();
}

