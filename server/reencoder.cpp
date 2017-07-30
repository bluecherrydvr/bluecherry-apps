#include "reencoder.h"
#include "vaapi.h"


reencoder::reencoder()
	: dec(0), enc(0), scl(0),
	software_decoding(false), hwframe_ctx(0)
{
	bc_log(Info, "new reencoder instance created");
}

reencoder::~reencoder()
{
	//delete dec, enc
	if (dec)
		delete dec;
	if (enc)
		delete enc;
	if (scl)
		delete scl;

	if (hwframe_ctx)
		av_buffer_unref(&hwframe_ctx);
}

bool reencoder::push_packet(const stream_packet &packet)
{

	if (!dec)
		dec = new decoder(AVMEDIA_TYPE_VIDEO);

	dec->push_packet(packet);

	return true;
}

bool reencoder::run_loop()
{
	AVFrame *frame;
	AVFrame *scaled_frame;
	bool result;

	/* 1) DECODE */
	frame = dec->decoded_frame();

	if (!frame)
		bc_log(Debug, "got no frame from decoder!");

	if (!frame)
		return false;

	/* 2) SCALE */
	if (!scl)
	{
		bc_log(Info, "creating new scaler instance for reencoding");

		scl = new scaler();

		const AVCodecContext *ctx = dec->get_ctx();

		if (!ctx->opaque)
		{
			bc_log(Warning, "hardware accelerated decoding is not available, falling back to software decoding");

			hwframe_ctx = vaapi_hwaccel::alloc_frame_ctx(ctx);

			if (!hwframe_ctx)
				return false;

			software_decoding = true;

		}
		else
			hwframe_ctx = av_buffer_ref(ctx->hw_frames_ctx);

		//hardcoded output size
		if (!scl->init_scaler(frame->width / 2, frame->height / 2, hwframe_ctx, ctx))
		{
			bc_log(Error, "Failed to initialize scaler instance for reencoding");
			delete scl;
			scl = NULL;
			return false;
		}
	}

	if (software_decoding)
	{
		/* move frame data from system memory to video mem */
		if (!vaapi_hwaccel::hwupload_frame(hwframe_ctx, frame))
			return false;
	}

	scl->push_frame(frame);

	scaled_frame = scl->scaled_frame();

	if (scaled_frame == NULL)
		return false;


	bc_log(Debug, "got frame of size %d x %d from scaler", scaled_frame->width, scaled_frame->height);

	/* 3) ENCODE */
	if (!enc)
	{
		bc_log(Info, "creating encoder instance for reencoding");

		enc = new encoder();


		//hardcoded parameters, for testing purposes
		if (!enc->init_encoder(AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_H264, 64000, scaled_frame->width, scaled_frame->height, scaled_frame->hw_frames_ctx))
		{
			bc_log(Error, "Failed to initialize encoder instance");
			delete enc;
			enc = NULL;
			return false;
		}
	}

	enc->push_frame(scaled_frame);

	result = enc->encode();

	av_frame_unref(scaled_frame);

	return result;
}

const stream_packet &reencoder::packet() const
{
	return enc->packet();
}

