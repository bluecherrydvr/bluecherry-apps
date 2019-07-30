#include "reencoder.h"
#include "vaapi.h"


reencoder::reencoder(int bitrate, int out_frame_w, int out_frame_h, bool watermarking)
	: dec(0), wmr(0), enc_streaming(0), enc_recording(0), scl(0),
	hw_frames_ctx(0),
	last_decoded_fw(0), last_decoded_fh(0),
	out_frame_w(out_frame_w), out_frame_h(out_frame_h),
	streaming_bitrate(bitrate), recording_bitrate(0),
	incoming_bitrate_avg(0), stats_collected(false),
	first_packet_dts(0),
	watermarking(watermarking),
	dvr_name(nullptr), camera_name(nullptr)
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
	if (hw_frames_ctx)
		av_buffer_unref(&hw_frames_ctx);
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

	update_stats(packet);

	return true;
}

bool reencoder::init_enc_streaming(AVFrame *in)
{
	if (!enc_streaming)
	{
		bc_log(Info, "creating encoder instance for streaming");

		enc_streaming = new encoder();

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

void reencoder::update_stats(const stream_packet &packet)
{
	if (stats_collected)
		return;


	if (incoming_bitrate_avg == 0)
		first_packet_dts = packet.dts;

	incoming_bitrate_avg += packet.size * 8;

	if (recording_bitrate == 0)
		recording_bitrate = packet.size * 8 * 15;

	if (packet.seq > 1 && packet.dts - first_packet_dts > AV_TIME_BASE * 3)
	{
		int64_t timediff = packet.dts - first_packet_dts;

		incoming_bitrate_avg = incoming_bitrate_avg *  AV_TIME_BASE / timediff;

		AVRational fps = { packet.seq / 3, 1};

		bc_log(Info, "reencoder: input framerate is %i / %i", fps.num, fps.den);
		if (enc_streaming)
			enc_streaming->update_framerate(fps);

		if (enc_recording)
		{
			enc_recording->update_framerate(fps);
			/* difference is > 25% */
			if ((incoming_bitrate_avg - recording_bitrate) * 100 / recording_bitrate > 25)
			{
				recording_bitrate = incoming_bitrate_avg;
				enc_recording->update_bitrate(recording_bitrate);
				bc_log(Info, "reencoder: updating recording bitrate to %i, fps %i / %i", recording_bitrate, fps.num, fps.den);
			}
		}
		stats_collected = true;
	}
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

	bc_log(Debug, "reencoder: got frame %dx%d from decoder, bitrate %d", frame->width, frame->height,  dec->get_ctx()->bit_rate);

	/* 2) ADD WATERMARK */
	if (watermarking)
	{
		if (!wmr)
		{
			bc_log(Info, "creating new watermarker instance");

			wmr = new watermarker();

			const AVCodecContext *ctx = dec->get_ctx();

			if (!wmr->init_watermarker(dvr_name, camera_name, ctx))
			{
				bc_log(Error, "Failed to initialize watermarker instance");
				delete wmr;
				wmr = nullptr;
				return false;
			}

			hw_frames_ctx = vaapi_hwaccel::alloc_frame_ctx(ctx);

			if (!hw_frames_ctx)
				return false;
		}
	}

	/* 3) SCALE */
	if (!scl && out_frame_w > 0)
	{
		bc_log(Info, "creating new scaler instance for reencoding");

		scl = new scaler();

		const AVCodecContext *ctx = dec->get_ctx();

		AVBufferRef *hwctx = watermarking ? hw_frames_ctx : ctx->hw_frames_ctx;

		if (!scl->init_scaler(out_frame_w, out_frame_h, ctx, hwctx))
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

		AVBufferRef *hwctx = watermarking ? hw_frames_ctx : ctx->hw_frames_ctx;

		if (scl)
			scl->reinitialize(ctx, hwctx);

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

		if (!vaapi_hwaccel::hwupload_frame(hw_frames_ctx, watermarked_frame))
			return false;

		bc_log(Debug, "uploaded watermarked frame to vaapi hardware context");
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

		//if (!init_enc_recording(frame))
		//	return false;

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
	if (mode == REENC_MODE_WMR_ONLY)
		return enc_recording->packet();

	return enc_streaming->packet();
}

const stream_packet &reencoder::recording_packet() const
{
	return enc_recording->packet();
}

