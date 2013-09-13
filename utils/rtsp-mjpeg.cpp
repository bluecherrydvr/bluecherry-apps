/*
 * Copyright (C) 2013 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <errno.h>
extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
};

#define LIMIT_IMAGE_SIZE		(300 * 1024)

// rtsp://admin:1234@192.168.0.108:554/Primary?tcp
static void usage(void)
{
	fprintf(stderr, "Usage: rtsp-mjpeg rtsp://[USERINFO@]"
		"SERVER[:PORT]/[SUB_URL]?[PROTOCOL_TYPE(tcp/udp)] \n\n");
	fprintf(stderr, "Example: rtsp-mjpeg rtsp://admin:1234@192.168.0.108:554/Primary?tcp\n\n");
	exit(EXIT_FAILURE);
}

AVFormatContext *in_codec_ctx = NULL;
int video_stream_index = -1;
int video_stream_codec_id = 0;

int int_in_width = 0;
int int_in_height = 0;
int int_out_width = 0;
int int_out_height = 0;

AVCodec *enc_codec = NULL;
AVCodec *dec_codec = NULL;
struct SwsContext *img_convert_ctx = NULL;
AVCodecContext *enc_codec_ctx = NULL;
AVCodecContext *dec_codec_ctx = NULL;
AVFrame *decoded_avframe = NULL;
AVFrame *temp_avframe = NULL;
AVFrame *encode_avframe = NULL;

// transcoder init functino from "fromCodec" to "toCodec"...
int TranscoderInit(int fromCodec, int toCodec);
// transcoding funciton from "ptr_in_buffer" to "ptr_out_buffer"...
// return value is out_buffer's length
int Transcode(int int_in_buffer_len, uint8_t *ptr_in_buffer, uint8_t *ptr_out_buffer);
// transcoder close
int TranscoderClose();
void print_image(int int_image_len, uint8_t *ptr_image_buffer);

int main(int argc, char **argv)
{
	int opt = 0;
	char *url = NULL;

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
		case 'h': 
		default: 
			usage();
		}
	}

	url = argv[optind++];
		
	av_register_all();
	avformat_network_init();
	av_log_set_level(AV_LOG_DEBUG);
	
	if (av_open_input_file(&in_codec_ctx, url, NULL, 0, NULL) != 0) {
		fprintf(stderr, "Could not open URI.\n");
		exit(1);
	}

	if (av_find_stream_info(in_codec_ctx) < 0) {
		av_close_input_file (in_codec_ctx);
		fprintf(stderr, "Could not find stream info.\n");
		exit(1);
	}

	dump_format(in_codec_ctx, 0, url, 0);

	for (unsigned int i = 0; i < in_codec_ctx->nb_streams; ++i) {
		if (in_codec_ctx->streams[i]->codec->codec_type != AVMEDIA_TYPE_VIDEO)
			continue;
			
		printf("Found video codec! -- codec_id = %d, codec_name = %s\n"
		       , in_codec_ctx->streams[i]->codec->codec_id, in_codec_ctx->streams[i]->codec->codec_name);

		dec_codec_ctx = in_codec_ctx->streams[i]->codec;
		video_stream_index = i;
		video_stream_codec_id = in_codec_ctx->streams[i]->codec->codec_id;
		
		int_in_width = dec_codec_ctx->width;
		int_in_height = dec_codec_ctx->height;
		int_out_width = dec_codec_ctx->width;
		int_out_height = dec_codec_ctx->height;
		
		break;
	}

	bool bln_through_pass = false;
	if (video_stream_codec_id == CODEC_ID_MJPEG)
		bln_through_pass = true;

	if (bln_through_pass == false && TranscoderInit(video_stream_codec_id, CODEC_ID_MJPEG) != 0)
	{
		av_close_input_file (in_codec_ctx);
		fprintf(stderr, "Could not init a transcoder.\n");
		exit(1);
	}

	AVPacket packet;
	uint8_t *ptr_out_buffer = new uint8_t[LIMIT_IMAGE_SIZE];

	int re = -1;
	int byteEncoded = 0;
	while ((re = av_read_frame(in_codec_ctx, &packet)) >= 0) {
		if (packet.stream_index != video_stream_index)
			continue;

//		fprintf(stdout, "av_read_frame -- size = %d\n", packet.size);
		
		// if input codec_id is equal CODEC_ID_MJPEG, it's not transcoding...
		if (bln_through_pass == false)
		{
			byteEncoded = Transcode(packet.size, packet.data, ptr_out_buffer);
			if (byteEncoded <= 0)
				continue;
			print_image(byteEncoded, ptr_out_buffer);
		}
		else
		{
			memcpy(ptr_out_buffer, packet.data, packet.size);
			byteEncoded = packet.size;

			print_image(byteEncoded, ptr_out_buffer);
		}
	}

	delete[] ptr_out_buffer;

	av_close_input_file (in_codec_ctx);
	printf("End; last return value %d\n", re);

	exit(0);
}

void print_image(int int_image_len, uint8_t *ptr_image_buffer)
{
	/*
	static int int_counter = 0;
	char str_file_name[1024] = {0};
	sprintf(str_file_name, "/work/temp files/images/img_%06d.jpg", int_counter);
	int_counter ++;
	FILE *fp = fopen(str_file_name, "wb");
	fwrite(ptr_image_buffer, sizeof(uint8_t), int_image_len, fp);
	fclose (fp);
	*/

	fprintf(stdout, "encoded_frame -- size = %d\n", int_image_len);
	fwrite(ptr_image_buffer, int_image_len, 1, stdout);
}

int TranscoderInit(int fromCodec, int toCodec)
{
	dec_codec = avcodec_find_decoder((CodecID)fromCodec);
	enc_codec = avcodec_find_encoder((CodecID)toCodec);

	if (dec_codec == NULL)
	{
		fprintf(stderr, "Could not find decoder.\n");
		return -1;
	}

	if (enc_codec == NULL)
	{
		fprintf(stderr, "Could not find encoder.\n");
		return -1;
	}
	
//	dec_codec_ctx = new struct AVCodecContext();
	enc_codec_ctx = new struct AVCodecContext();

//	avcodec_get_context_defaults(dec_codec_ctx);
	avcodec_get_context_defaults(enc_codec_ctx);

	PixelFormat from_pix_fmt = PIX_FMT_YUV420P;
	PixelFormat to_pix_fmt = PIX_FMT_YUVJ420P;
	
	switch (fromCodec)
	{
	case CODEC_ID_MJPEG:
		from_pix_fmt = PIX_FMT_YUVJ420P;
		break;
	case CODEC_ID_MPEG4:
	case CODEC_ID_H264:
		from_pix_fmt = PIX_FMT_YUV420P;
		break;
	}

	switch (toCodec)
	{
	case CODEC_ID_MJPEG:
		to_pix_fmt = PIX_FMT_YUVJ420P;
		break;
	case CODEC_ID_MPEG4:
	case CODEC_ID_H264:
		to_pix_fmt = PIX_FMT_YUV420P;
		break;
	}

/*	dec_codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
	dec_codec_ctx->bit_rate = 0;
	dec_codec_ctx->width = 0;
	dec_codec_ctx->height = 0;
	dec_codec_ctx->time_base.den = 20;
	dec_codec_ctx->time_base.num = 1;
	dec_codec_ctx->gop_size = 1;
	dec_codec_ctx->max_b_frames = 0;
	dec_codec_ctx->pix_fmt = from_pix_fmt;
*/	

	enc_codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
	enc_codec_ctx->bit_rate = 1000000;
	enc_codec_ctx->width = int_out_width;
	enc_codec_ctx->height = int_out_height;
	enc_codec_ctx->time_base = dec_codec_ctx->time_base;
	enc_codec_ctx->gop_size = 12;
	enc_codec_ctx->qcompress = 0.9f;
	enc_codec_ctx->max_b_frames = 0;
	enc_codec_ctx->rc_buffer_size = 0;
	enc_codec_ctx->rc_lookahead = 0;
	enc_codec_ctx->keyint_min = 25;
	enc_codec_ctx->pix_fmt = to_pix_fmt;

	if (avcodec_open(dec_codec_ctx, dec_codec) < 0)
	{
		fprintf(stderr, "Could not open codec for decoder.\n");		
		return -1;
	}

	if (avcodec_open(enc_codec_ctx, enc_codec) < 0)
	{
		fprintf(stderr, "Could not open codec for encoder.\n");		
		return -1;
	}

	decoded_avframe = avcodec_alloc_frame();
	if (decoded_avframe == NULL)
	{
		fprintf(stderr, "Could not alloc avframe for decoded_avframe.\n");		
		return -1;
	}

	temp_avframe = avcodec_alloc_frame();
	if (temp_avframe == NULL)
	{
		fprintf(stderr, "Could not alloc avframe for temp_avframe.\n");		
		return -1;
	}

	encode_avframe = avcodec_alloc_frame();
	if (encode_avframe == NULL)
	{
		fprintf(stderr, "Could not alloc avframe for encode_avframe.\n");		
		return -1;
	}
	
	img_convert_ctx = NULL;

	return 0;
}

int Transcode(int int_in_buffer_len, uint8_t *ptr_in_buffer, uint8_t *ptr_out_buffer)
{
	int bytesDecoded = 0;
	int bytesEncoded = 0;
	int frameFinished = 0;

	if (dec_codec_ctx == NULL)
		return -1;

	AVPacket avp;
	av_init_packet(&avp);
	avp.data = ptr_in_buffer;
	avp.size = int_in_buffer_len;

	bytesDecoded = avcodec_decode_video2(dec_codec_ctx, decoded_avframe, &frameFinished, &avp);

	if (bytesDecoded <= 0 || !frameFinished)
		return 0;

	if (img_convert_ctx == NULL)
	{
		img_convert_ctx = sws_getContext(int_in_width, int_in_height, dec_codec_ctx->pix_fmt, 
			int_out_width, int_out_height, enc_codec_ctx->pix_fmt, SWS_BICUBIC, 
			NULL, NULL, NULL);

		if (img_convert_ctx == NULL)
		{
			fprintf(stderr, "Could not alloc avframe for decoded_frame.\n");					
			return -1;
		}
	}

	if (enc_codec_ctx == NULL)
		return -1;
	
	if (temp_avframe == NULL)
		return -1;

	if (encode_avframe == NULL)
		return -1;

	int pic_size = 0;

 	uint8_t *picture_buf = NULL;
	pic_size = avpicture_get_size(enc_codec_ctx->pix_fmt, int_out_width, int_out_height) * 2;
	
	picture_buf = new uint8_t[pic_size];
	if (picture_buf == NULL) 
	{
		fprintf(stderr, "Could not alloc memory for avpicture_get_size.\n");		
		return -1;
	}
	
	avpicture_fill((AVPicture *)encode_avframe, picture_buf, enc_codec_ctx->pix_fmt, int_out_width, int_out_height);
	
	if (img_convert_ctx != NULL)
		sws_scale(img_convert_ctx, decoded_avframe->data, decoded_avframe->linesize, 0
		          ,	int_in_height, encode_avframe->data, encode_avframe->linesize);
	
	bytesEncoded = avcodec_encode_video(enc_codec_ctx, ptr_out_buffer, pic_size, encode_avframe);

	if (bytesEncoded < 0) 
	{
		delete[] picture_buf;
		return 0;
	}
	
	delete[] picture_buf;
	return bytesEncoded;
}

int TranscoderClose()
{
	if (decoded_avframe != NULL)
		av_free(decoded_avframe);
	if (temp_avframe != NULL)
		av_free(temp_avframe);
	if (encode_avframe != NULL)
		av_free(encode_avframe);

	if (enc_codec != NULL)
		enc_codec = NULL;
	if (dec_codec != NULL)
		dec_codec = NULL;

	if (enc_codec_ctx != NULL)
	{
		avcodec_close(enc_codec_ctx);
		delete enc_codec_ctx;
		enc_codec_ctx = NULL;
	}

	if (dec_codec_ctx != NULL)
	{
//		avcodec_close(dec_codec_ctx);
//		delete dec_codec_ctx;
		dec_codec_ctx = NULL;
	}

	if (img_convert_ctx != NULL)
	{
		sws_freeContext(img_convert_ctx);
		img_convert_ctx = NULL;
	}
	
	return 0;
}
