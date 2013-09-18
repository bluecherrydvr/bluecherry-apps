#include "libbluecherry.h"
#include "transcoder.h"

void avcodec_logs(void* ptr, int level, const char* fmt, va_list vl)
{
	char szBuff[1000];
	vsprintf(szBuff, fmt, vl);

	char szFile[1024];
	sprintf(szFile, "/work/avcodec.log");

	char* pauseTime = new char[2048];
	sprintf(pauseTime, "%s \r\n", szBuff);

	FILE *fp = fopen(szFile, "a");
	fwrite(pauseTime, 1, strlen(pauseTime), fp);
	fclose(fp);

	delete[] pauseTime;
}

transcoder::transcoder(int dev_id) 
{
	in_codec_ctx = NULL;
	video_stream_index = -1;
	video_stream_codec_id = 0;

	int_in_width = 0;
	int_in_height = 0;
	int_out_width = 0;
	int_out_height = 0;

	enc_codec = NULL;
	dec_codec = NULL;
	img_convert_ctx = NULL;
	enc_codec_ctx = NULL;
	dec_codec_ctx = NULL;
	decoded_avframe = NULL;
	temp_avframe = NULL;
	encode_avframe = NULL;

	device_id = dev_id;
	handle = NULL;
	memset(img_buffer, 0, LIMIT_IMAGE_SIZE);
	img_length = 0;
	pthread_mutex_init( &lock, NULL);
	status_flag = BC_CODER_STATUS_INIT;
}

transcoder::~transcoder()
{
	pthread_mutex_destroy(&lock);
}

int transcoder::connect()
{
	transcoder_close();
	
	char url[256];
	sprintf(url, "rtsp://Admin:bluecherry@192.168.0.96:7002/live/%d/streamid=0", device_id);

	if (av_open_input_file(&in_codec_ctx, url, NULL, 0, NULL) != 0) {
		return -1; 
	}

	if (av_find_stream_info(in_codec_ctx) < 0) {
		av_close_input_file (in_codec_ctx);
		return -1;
	}

	dump_format(in_codec_ctx, 0, url, 0);

	for (unsigned int i = 0; i < in_codec_ctx->nb_streams; ++i) {
		if (in_codec_ctx->streams[i]->codec->codec_type != AVMEDIA_TYPE_VIDEO)
			continue;
			
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

	if (bln_through_pass == false && transcoder_init(video_stream_codec_id, CODEC_ID_MJPEG) != 0)
	{
		av_close_input_file (in_codec_ctx);
		return -1;
	}
}

int transcoder::start()
{
	av_register_all();
	avformat_network_init();
	//av_log_set_level(48);
	//av_log_set_callback(avcodec_logs);
	
	if (connect() != 0)
		return -1;

	int thread_result = pthread_create(&handle, NULL, (void*(*)(void*))thread_func, (void*)this);
	if(thread_result != 0)
		return -1;

	pthread_detach(handle);

	return 0;
}

void transcoder::thread_func(void* param)
{
	transcoder* This = (transcoder*)param;
	This->run();
}


void transcoder::run()
{
	bool bln_through_pass = false;
	if (video_stream_codec_id == CODEC_ID_MJPEG)
		bln_through_pass = true;
	
	AVPacket packet;
	uint8_t *ptr_out_buffer = new uint8_t[LIMIT_IMAGE_SIZE];

	int re = -1;
	int byteEncoded = 0;

	status_flag = BC_CODER_STATUS_RUNNING;

	while (status_flag == BC_CODER_STATUS_RUNNING)
	{
		re = av_read_frame(in_codec_ctx, &packet);
		if (re < 0)
		{
			while (status_flag == BC_CODER_STATUS_RUNNING && connect() < 0)
			{
				sleep(1);
			}
			
			if (status_flag == BC_CODER_STATUS_RUNNING)
				continue;
			else
				break;
		}
		
		if (packet.stream_index != video_stream_index)
		{
			continue;
		}

		// if input codec_id is equal CODEC_ID_MJPEG, it's not transcoding...
		if (bln_through_pass == false)
		{
			byteEncoded = transcode(packet.size, packet.data, ptr_out_buffer);
			if (byteEncoded <= 0)
				continue;
			set_image(byteEncoded, ptr_out_buffer);
		}
		else
		{
			memcpy(ptr_out_buffer, packet.data, packet.size);
			byteEncoded = packet.size;

			set_image(byteEncoded, ptr_out_buffer);
		}
	}

	delete[] ptr_out_buffer;

	status_flag = BC_CODER_STATUS_STOPPED;
}

int transcoder::stop()
{
	if (status_flag != BC_CODER_STATUS_RUNNING)
		return 0;

	status_flag = BC_CODER_STATUS_STOPPING;

	while(status_flag == BC_CODER_STATUS_STOPPING)
	{
		usleep(10000);
	}

	transcoder_close();
	av_close_input_file (in_codec_ctx);
}

void transcoder::set_image(int int_image_len, uint8_t *ptr_image_buffer)
{
	pthread_mutex_lock(&lock);

	memcpy(img_buffer, ptr_image_buffer, int_image_len);
	img_length = int_image_len;

	pthread_mutex_unlock(&lock);
}

void transcoder::get_image(unsigned char* buf, int& length)
{
	while (img_length == 0)
		usleep(10000);
	
	pthread_mutex_lock(&lock);

	memcpy(buf, img_buffer, img_length);
	length = img_length;

	pthread_mutex_unlock(&lock);
}

int transcoder::transcoder_init(int fromCodec, int toCodec)
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
	
	enc_codec_ctx = new struct AVCodecContext();
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

/*	
	dec_codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
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

int transcoder::transcode(int int_in_buffer_len, uint8_t *ptr_in_buffer, uint8_t *ptr_out_buffer)
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

int transcoder::transcoder_close()
{
	if (decoded_avframe != NULL)
	{
		av_free(decoded_avframe);
		decoded_avframe = NULL;
	}
	
	if (temp_avframe != NULL)
	{
		av_free(temp_avframe);
		temp_avframe = NULL;
	}
	
	if (encode_avframe != NULL)
	{
		av_free(encode_avframe);
		encode_avframe = NULL;
	}

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
		//avcodec_close(dec_codec_ctx);
		//delete dec_codec_ctx;
		dec_codec_ctx = NULL;
	}

	if (img_convert_ctx != NULL)
	{
		sws_freeContext(img_convert_ctx);
		img_convert_ctx = NULL;
	}

	if (in_codec_ctx != NULL)
	{
		avformat_free_context(in_codec_ctx);
		in_codec_ctx = NULL;
	}
	
	return 0;
}

