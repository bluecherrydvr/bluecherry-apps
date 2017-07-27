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
	: frame_context(0)
{
	if (!is_initialised)
		bc_log(Error, "VAAPI device is not initialized!");
}

vaapi_hwaccel::vaapi_hwaccel(AVBufferRef *frctx)
	: frame_context(frctx)
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

int vaapi_hwaccel::init_decoder(AVCodecContext *s)
{
// static method!
//	if (frame_context)
//	{
//		bc_log(Error, "VAAPI frame context already exists, failed to initialize decoder!");
//		return 0;
//	}

	AVBufferRef* ref;
	AVHWFramesContext* frctx;

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

	s->pix_fmt = s->sw_pix_fmt;
	s->hw_frames_ctx = ref;

	s->opaque = (void*)1; //hardware acceleration is enabled for this AVCodecContext instance

//static method!
//	frame_context = ref;

	return 1;
}


