#include "reencoder.h"
#include "vaapi.h"


reencoder::reencoder(int bitrate, int out_frame_w, int out_frame_h, bool watermarking)
	: dec(0), wmr(0),
	enc_streaming(0), enc_streaming_todelete(0),
	enc_recording(0), enc_recording_todelete(0),
	scl(0),	hw_frames_ctx(0),
	last_decoded_fw(0), last_decoded_fh(0),
	last_decoded_pixfmt(AV_PIX_FMT_NONE),
	out_frame_w(out_frame_w), out_frame_h(out_frame_h),
	streaming_bitrate(bitrate), recording_bitrate(0),
	incoming_bitrate_avg(0), bitrate_calc_period_sec(3),
	incoming_fps_ctr(0), first_packet_dts(0),
	watermarking(watermarking),
	dvr_name(nullptr), camera_name(nullptr),
	motion_flags_ctr(0)
{
	bc_log(Debug, "new reencoder instance created, streaming bitrate %d,\
output streaming frame_width %dx%d, watermarking %s",
		bitrate, out_frame_w, out_frame_h, watermarking ? "enabled" : "disabled");

#ifdef WATERMARKING_ENABLED
	if (watermarking && out_frame_w == -1)
		mode = REENC_MODE_WMR_ONLY;
#else
	if (watermarking)
		bc_log(Fatal, "watermarking is disabled in this build!");
#endif
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
	if (enc_streaming_todelete)
		delete enc_streaming_todelete;
	if (enc_recording)
		delete enc_recording;
	if (enc_recording_todelete)
		delete enc_recording_todelete;
	if (scl)
		delete scl;
#ifdef WATERMARKING_ENABLED
	if (wmr)
		delete wmr;
#endif
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
#ifdef WATERMARKING_ENABLED
	if (watermarking && wmr && packet.seq % 200 == 0)
		wmr->reinitialize(dec->get_ctx());
#endif
	return true;
}

bool reencoder::init_enc_streaming(AVFrame *in)
{
	if (!enc_streaming)
	{
		bc_log(Debug, "creating encoder instance for streaming");

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
	if (packet.flags & stream_packet::MotionFlag)
		motion_flags_ctr++;

	if (motion_flags_ctr > 250)
		bc_log(Error, "reencoder: motion flags counter grows too big");

	if (packet.dts < 0)
	{
		bc_log(Debug, "reencoder: ignoring packet with negative DTS in bitrate calculations");
		return;
	}

	if (incoming_bitrate_avg == 0)
		first_packet_dts = packet.dts;

	incoming_bitrate_avg += packet.size * 8;

	if (recording_bitrate == 0)
		recording_bitrate = packet.size * 8 * 15;

	if (packet.seq > 1 && incoming_fps_ctr > 0 
		&& packet.dts - first_packet_dts > AV_TIME_BASE * bitrate_calc_period_sec)
	{
		int64_t timediff = packet.dts - first_packet_dts;

		incoming_bitrate_avg = incoming_bitrate_avg *  AV_TIME_BASE / timediff;

		AVRational fps;

		av_reduce(&fps.num, &fps.den, incoming_fps_ctr, timediff / AV_TIME_BASE, 1000);

		bc_log(Debug, "reencoder: input framerate is %i / %i", fps.num, fps.den);
		if (enc_streaming && abs(enc_streaming->get_fps().num - fps.num) > 2)
			enc_streaming->update_framerate(fps);

		if (enc_recording)
		{
			if (abs(enc_recording->get_fps().num - fps.num) > 2)
				enc_recording->update_framerate(fps);

			/* difference is > 25% */
			if (llabs(incoming_bitrate_avg - recording_bitrate) * 100 / recording_bitrate > 25)
			{
				recording_bitrate = incoming_bitrate_avg;
				enc_recording->update_bitrate(recording_bitrate);
				bc_log(Debug, "reencoder: updating recording bitrate to %i, fps %i / %i", recording_bitrate, fps.num, fps.den);
			}
		}
		incoming_bitrate_avg = 0;
		bitrate_calc_period_sec = 10;
		incoming_fps_ctr = 0;
	}
}

bool reencoder::run_loop()
{
	AVFrame *frame;
	AVFrame *scaled_frame = NULL;
	AVFrame *watermarked_frame = NULL;
	bool result = false;
	int motion_flag = 0;

	/* 1) DECODE */
	frame = dec->decoded_frame();

	if (!frame)
		bc_log(Debug, "got no frame from decoder!");

	if (!frame)
		return false;

	incoming_fps_ctr++;

	bc_log(Debug, "reencoder: got frame %dx%d from decoder, bitrate %ld", frame->width, frame->height,  dec->get_ctx()->bit_rate);
#ifdef WATERMARKING_ENABLED
	/* 2) ADD WATERMARK */
	if (watermarking)
	{
		if (!wmr)
		{
			bc_log(Debug, "creating new watermarker instance");

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
#endif
	/* 3) SCALE */
	if (!scl && out_frame_w > 0)
	{
		bc_log(Debug, "creating new scaler instance for reencoding");

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

	//if (last_decoded_fw > 0 && 
	//	(last_decoded_fw != frame->width || last_decoded_fh != frame->height || last_decoded_pixfmt != frame->format))
	if (dec->properties_changed())
	{
		const AVCodecContext *ctx = dec->get_ctx();

		if (watermarking)
		{
			av_buffer_unref(&hw_frames_ctx);
			hw_frames_ctx = vaapi_hwaccel::alloc_frame_ctx(ctx);
		}

		AVBufferRef *hwctx = watermarking ? hw_frames_ctx : ctx->hw_frames_ctx;

		if (scl)
			scl->reinitialize(ctx, hwctx);
#ifdef WATERMARKING_ENABLED
		if (wmr)
			wmr->reinitialize(ctx);
#endif
		if (enc_streaming)
		{
			if (enc_streaming_todelete)
				delete enc_streaming_todelete;

			enc_streaming_todelete = enc_streaming;

			enc_streaming_todelete->push_frame(NULL, 0);

			enc_streaming = nullptr;
		}

		if (enc_recording)
		{
			if (enc_recording_todelete)
				delete enc_recording_todelete;

			enc_recording_todelete = enc_recording;

			enc_recording_todelete->push_frame(NULL, 0);

			enc_recording = nullptr;
		}
	}

	last_decoded_fw = frame->width;
	last_decoded_fh = frame->height;
	last_decoded_pixfmt = (enum AVPixelFormat)frame->format;
#ifdef WATERMARKING_ENABLED
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
#endif
	if (motion_flags_ctr > 0)
	{
		motion_flag = 1;
		motion_flags_ctr--;
	}

	switch (mode)
	{
	case REENC_MODE_WMR_ONLY:

		if (!init_enc_recording(watermarked_frame))
			return false;

		enc_recording->push_frame(watermarked_frame, motion_flag);


		av_frame_unref(watermarked_frame);

		break;
	case REENC_MODE_WMR_AND_STREAMING:

		if (!init_enc_recording(watermarked_frame))
			return false;

		enc_recording->push_frame(watermarked_frame, motion_flag);

		scl->push_frame(watermarked_frame);

		scaled_frame = scl->scaled_frame();

		if (scaled_frame == NULL)
			return false;

		bc_log(Debug, "got frame of size %d x %d vaapi surface %#lx from scaler", scaled_frame->width, scaled_frame->height, (uintptr_t)scaled_frame->data[3]);

		if (!init_enc_streaming(scaled_frame))
			return false;

		enc_streaming->push_frame(scaled_frame, motion_flag);

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


		bc_log(Debug, "got frame of size %d x %d vaapi surface %#lx from scaler", scaled_frame->width, scaled_frame->height, (uintptr_t)scaled_frame->data[3]);

		if (!init_enc_streaming(scaled_frame))
			return false;

		enc_streaming->push_frame(scaled_frame, motion_flag);

		av_frame_unref(scaled_frame);

		break;
	}

	return true;
}

bool reencoder::read_streaming_packet()
{
	bool result = false;

	if (enc_streaming_todelete)
	{
		result = enc_streaming_todelete->encode();

		if (!result)
		{
			delete enc_streaming_todelete;
			enc_streaming_todelete = nullptr;
		}
		else
			return true;
	}

	if (!enc_streaming)
		return false;

	result = enc_streaming->encode();

	return result;
}

bool reencoder::read_recording_packet()
{
	bool result = false;

	if (enc_recording_todelete)
	{
		result = enc_recording_todelete->encode();

		if (!result)
		{
			delete enc_recording_todelete;
			enc_recording_todelete = nullptr;
		}
		else
			return true;
	}

	if (!enc_recording)
		return false;

	result = enc_recording->encode();

	return result;
}

const stream_packet &reencoder::streaming_packet() const
{
	if (mode == REENC_MODE_WMR_ONLY)
	{
		return recording_packet();
	}

	if (enc_streaming_todelete)
		return enc_streaming_todelete->packet();

	return enc_streaming->packet();
}

const stream_packet &reencoder::recording_packet() const
{
	if (enc_recording_todelete)
		return enc_recording_todelete->packet();

	return enc_recording->packet();
}

