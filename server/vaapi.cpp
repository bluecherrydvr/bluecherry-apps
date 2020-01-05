#include "vaapi.h"
#include "logc.h"

extern "C"
{
#include <va/va.h>
#include <va/va_drm.h>
#include <libavutil/hwcontext_vaapi.h>
#include <libavutil/pixdesc.h>
#include <libavutil/frame.h>
}

AVBufferRef *vaapi_hwaccel::device_context = 0;
bool vaapi_hwaccel::is_initialised = false;

vaapi_hwaccel::vaapi_hwaccel()
{
	if (!is_initialised)
		bc_log(Error, "VAAPI device is not initialized!");
}

bool vaapi_hwaccel::init(const char *device)
{
	int ret;

	ret = av_hwdevice_ctx_create(&device_context, AV_HWDEVICE_TYPE_VAAPI,
					device, NULL, 0);

	if (ret < 0)
		return false;

	is_initialised = true;

	return true;
}

bool vaapi_hwaccel::hwdownload_frame(const AVCodecContext *s, AVFrame *input)
{
	AVFrame *output = 0;
	int ret;

	if (input->hw_frames_ctx == NULL)
	{
		bc_log(Error, "vaapi_hwaccel::hwdownload_frame: input frame does not contain hardware frame context");
		return false;
	}

	if (input->format != AV_PIX_FMT_VAAPI)
	{
		bc_log(Error, "vaapi_hwaccel::hwdownload_frame: invalid pixel format in input frame");
		return false;
	}

	output = av_frame_alloc();

	if (!output)
		return false;

	output->format = s->sw_pix_fmt;

	if (output->format == AV_PIX_FMT_YUVJ420P)
		output->format = AV_PIX_FMT_YUV420P;

	ret = av_hwframe_transfer_data(output, input, 0);

	if (ret < 0)
	{
		bc_log(Error, "failed to transfer data from hardware frame");
		goto fail;
	}

	ret = av_frame_copy_props(output, input);

	if (ret < 0)
	{
		av_frame_unref(output);
		goto fail;
	}

	av_frame_unref(input);
	av_frame_move_ref(input, output);
	av_frame_free(&output);

	return true;

fail:
	if (output)
		av_frame_free(&output);
	return false;
}

bool vaapi_hwaccel::hwupload_frame(AVBufferRef *hwframe_ctx, AVFrame *input)
{
	AVFrame *output = 0;
	int ret;

	output = av_frame_alloc();

	if (!output)
		return false;

	output->format = AV_PIX_FMT_VAAPI;

	ret = av_hwframe_get_buffer(hwframe_ctx, output, 0);

	if (ret != 0)
		goto fail;

	if (input->format == AV_PIX_FMT_YUVJ420P)
		input->format = AV_PIX_FMT_YUV420P;

	ret = av_hwframe_transfer_data(output, input, 0);

	if (ret < 0)
	{
		goto fail;
	}

	ret = av_frame_copy_props(output, input);

	if (ret < 0) {
		av_frame_unref(output);
		goto fail;
	}

	av_frame_unref(input);
	av_frame_move_ref(input, output);
	av_frame_free(&output);

	return true;

fail:
	bc_log(Error, "Failed to upload frame to vaapi frame context");
	if (output)
		av_frame_free(&output);
	return false;
}

enum AVPixelFormat vaapi_hwaccel::get_format(AVCodecContext *s, const enum AVPixelFormat *pix_fmts)
{

	const enum AVPixelFormat *p;

	s->opaque = 0;

	for (p = pix_fmts; *p != -1; p++)
	{
		const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(*p);

	if (!(desc->flags & AV_PIX_FMT_FLAG_HWACCEL))
		break;

	if (*p == AV_PIX_FMT_VAAPI)
	{
		if (!init_decoder(s))
		{
			bc_log(Warning, "Failed to initialize VAAPI decoder for stream");
			return s->sw_pix_fmt;
		}

		break;
	}

	}

	return *p;
}

int vaapi_hwaccel::get_buffer(AVCodecContext *s, AVFrame *frame, int flags)
{
	int ret = 0;

	if (s->opaque)
	{
		ret = av_hwframe_get_buffer(s->hw_frames_ctx, frame, 0);

		if (ret < 0)
		{
			s->opaque = 0;
			bc_log(Error, "Failed to allocate VAAPI decoder surface!");
		}
	}
	else
		return avcodec_default_get_buffer2(s, frame, flags);

	return ret;
}

AVBufferRef *vaapi_hwaccel::alloc_frame_ctx(const AVCodecContext *s)
{
	AVBufferRef* ref;
	AVHWFramesContext* frctx;

        if (!is_initialised)
	{
		bc_log(Error, "VAAPI device is not initialized!");
		return 0;
	}

	ref = av_hwframe_ctx_alloc(device_context);

	if (ref == NULL)
	{
		bc_log(Error, "failed to allocate VAAPI frame context!");
		return 0;
	}

	frctx = (AVHWFramesContext*)ref->data;

	frctx->format = AV_PIX_FMT_VAAPI;
	frctx->width = s->coded_width;
	frctx->height = s->coded_height;
	frctx->sw_format = (s->sw_pix_fmt == AV_PIX_FMT_YUV420P10 ?
				AV_PIX_FMT_P010 : AV_PIX_FMT_NV12);
	frctx->initial_pool_size = 0;

	if (av_hwframe_ctx_init(ref) < 0)
	{
		bc_log(Error, "failed to initialize VAAPI frame context!");
		av_buffer_unref(&ref);
		return 0;
	}

	return ref;
}

int vaapi_hwaccel::init_decoder(AVCodecContext *s)
{
	if (!is_initialised)
		return 0;

	s->pix_fmt = s->sw_pix_fmt;
	s->hw_device_ctx = av_buffer_ref(device_context);

	s->opaque = (void*)1; //hardware acceleration is enabled for this AVCodecContext instance

	return 1;
}


