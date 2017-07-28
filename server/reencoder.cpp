#include "reencoder.h"


reencoder::reencoder()
	: dec(0), enc(0), buffersink_ctx(0), buffersrc_ctx(0), filter_graph(0)
{

}

reencoder::~reencoder()
{
	//delete dec, enc
	if (dec)
		delete dec;
	if (enc)
		delete enc;
	//free contexts
	if (filter_graph)
		avfilter_graph_free(&filter_graph);
}

bool reencoder::push_packet(const stream_packet &packet)
{

	if (!dec)
		dec = new decoder(AVMEDIA_TYPE_VIDEO);

	dec->push_packet(packet);
}

bool reencoder::run_loop()
{
	AVFrame *frame;
	frame = dec->decoded_frame();

	if (!frame)
		return false;

	if (!enc)
	{
		enc = new encoder();

		const AVCodecContext *ctx = dec->get_ctx();

		//hardcoded parameters, for testing purposes
		if (!enc->init_encoder(AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_H264, 64000, frame->width/* /2 */, frame->height/* /2 */, ctx->hw_frames_ctx))
		{
			bc_log(Error, "Failed to initialize encoder instance");
			delete enc;
			return false;
		}
	}

	enc->push_frame(frame);

	return enc->encode();
}

const stream_packet &reencoder::packet() const
{
	return enc->packet();
}

