#include "subtitle_source.h"
#include "input_device.h"
#include "libbluecherry.h"

subtitle_source::subtitle_source()
	: encoder(0), enc_ctx(0), is_started(false),
	packet_counter(0)
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

	current_packet = stream_packet();
	current_properties.reset();

	avcodec_free_context(&enc_ctx);
	is_started = false;
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
	enc_ctx->time_base = (AVRational){1, 1};

	ret = avcodec_open2(enc_ctx, encoder, NULL);
        if (ret < 0) {
		bc_log(Error, "Cannot open subtitle encoder");
		return -1;
	}

	stream_properties *p = new stream_properties;
	p->subs.codec_id = AV_CODEC_ID_TEXT;
	current_properties = std::shared_ptr<stream_properties>(p);

	is_started = true;
	return 0;
}

int subtitle_source::write_packet(const char *in)
{
	AVSubtitle avsub;
	AVSubtitleRect avsubrect;
	int out_size;

	AVSubtitleRect *rects[1] = { &avsubrect };

	memset(&avsub, 0, sizeof(avsub));
	memset(&avsubrect, 0, sizeof(avsubrect));

	subtitle_out = new uint8_t[1024];

	avsubrect.text = in;
	avsubrect.type = SUBTITLE_TEXT;

	avsub.format = 1; //1 for text
	avsub.start_display_time = 0;
	avsub.end_display_time = 1000;//ms
	avsub.num_rects = 1;
	avsub.rects = rects;
	avsub.pts = packet_counter++;

	//.. encode
	out_size = avcodec_encode_subtitle(enc_ctx, subtitle_out,
                                                     1024, &avsub);
	if (out_size < 0) {
		bc_log(Error, "Failed to encode subitle");
		return -1;
	}

	current_packet = stream_packet(subtitle_out, current_properties);
	current_packet.seq      = packet_counter;
	current_packet.size     = out_size
	current_packet.ts_clock = time(NULL);
	current_packet.pts      = packet_counter;
	current_packet.dts      = packet_counter;
	current_packet.ts_monotonic = bc_gettime_monotonic();

	current_packet.type = AVMEDIA_TYPE_SUBTITLE;

	return 0;
}

