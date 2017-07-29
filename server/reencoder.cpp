#include "reencoder.h"
#include "vaapi.h"

reencoder::reencoder()
	: dec(0), enc(0),
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
	frame = dec->decoded_frame();

	if (!frame)
		bc_log(Info, "got no frame from decoder!");

	if (!frame)
		return false;

	if (!enc)
	{
		bc_log(Info, "creating encoder instance for reencoding");

		enc = new encoder();

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
			hwframe_ctx = ctx->hw_frames_ctx;

		//hardcoded parameters, for testing purposes
		if (!enc->init_encoder(AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_H264, 64000, frame->width/* /2 */, frame->height/* /2 */, hwframe_ctx))
		{
			bc_log(Error, "Failed to initialize encoder instance");
			delete enc;
			return false;
		}
	}

	if (software_decoding)
	{
		/* move frame data from system memory to video mem */
		if (!vaapi_hwaccel::hwupload_frame(hwframe_ctx, frame))
			return false;
	}

	enc->push_frame(frame);

	return enc->encode();
}

const stream_packet &reencoder::packet() const
{
	return enc->packet();
}

