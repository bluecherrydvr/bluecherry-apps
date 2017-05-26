#include "subtitle_source.h"
#include "input_device.h"
#include "libbluecherry.h"

subtitle_source::subtitle_source()
	: encoder(0), enc_ctx(0), is_started(false) 
{
}

subtitle_source::~subtitle_source()
{
	stop();
}

void subtitle_source::stop()
{
	if (!is_started)
		return;

	//...
}

int subtitle_source::start()
{
	int ret;

	encoder = avcodec_find_encoder(AV_CODEC_ID_TEXT);

	if (!encoder) {
		bc_log(Error, "Failed to find subtitle codec");
		return -1;
	}

	enc_ctx = avcodec_alloc_context3(encoder);

	if (!enc_ctx) {
		bc_log(Error, "Failed to alloc subtitle codec context");
		return -1;
	}

	//... fill enc_ctx fields
	ret = avcodec_open2(enc_ctx, encoder, NULL);
        if (ret < 0) {
		bc_log(Error, "Cannot open subtitle encoder");
		return -1;
	}
	//...
	is_started = true;
	return 0;
}

int subtitle_source::get_stream_packet(stream_packet *out, const char *in)
{
	//...
}

