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

#include "libbluecherry.h"

typedef enum {
	BC_CODER_STATUS_INIT = 0,
	BC_CODER_STATUS_RUNNING,
	BC_CODER_STATUS_STOPPING,
	BC_CODER_STATUS_STOPPED
} bc_coder_status_code_t;

#define LIMIT_IMAGE_SIZE		(300 * 1024)

/* transcoder provides the jpeg image from rtsp url. */
class transcoder
{
public:
	transcoder(int dev_id);
	~transcoder();

	/* start the transcoding */
	int start();
	
	/* stop the transcoding */
	int stop();

	/* get the binary data of jpeg image */
	void get_image(unsigned char* buf, int& length);

private:

	int device_id;
	bc_coder_status_code_t status_flag;
	unsigned char img_buffer[LIMIT_IMAGE_SIZE];
	int img_length;
	pthread_mutex_t lock;
		
	int video_stream_index;
	int video_stream_codec_id;
	int int_in_width;
	int int_in_height;
	int int_out_width;
	int int_out_height;

	AVFormatContext *in_codec_ctx;
	AVCodec *enc_codec;
	AVCodec *dec_codec;
	struct SwsContext *img_convert_ctx;
	AVCodecContext *enc_codec_ctx;
	AVCodecContext *dec_codec_ctx;
	AVFrame *decoded_avframe;
	AVFrame *temp_avframe;
	AVFrame *encode_avframe;

	void run();
	static void thread_func(void* param);
	pthread_t handle;

	/* connect to device using url */
	int connect();
	
	/* transcoder init functino from "fromCodec" to "toCodec"... */
	int transcoder_init(int fromCodec, int toCodec);
	
	/* transcoding funciton from "ptr_in_buffer" to "ptr_out_buffer"...
	   return value is out_buffer's length. */
	int transcode(int int_in_buffer_len, uint8_t *ptr_in_buffer, uint8_t *ptr_out_buffer);
	
	/* transcoder close */
	int transcoder_close();

	/* set the image buffer */
	void set_image(int int_image_len, uint8_t *ptr_image_buffer);
};


