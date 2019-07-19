#include "watermarker.h"
#include "vaapi.h"

watermarker::watermarker()
	: name_string(0),
	buffersink_ctx(0), buffersrc_ctx(0),
	filter_graph(0), decoder_ctx(0), out_frame(0),
	software_decoding(false)
{
}

void watermarker::release_watermarker()
{
	avfilter_graph_free(&filter_graph);

	av_frame_free(&out_frame);
}

watermarker::~watermarker()
{
	release_watermarker();
}

bool watermarker::init_watermarker(const char *dvrname, const AVCodecContext *dec_ctx)
{
	char args[512];
	int ret = 0;
	AVFilter *buffersrc = NULL;
	AVFilter *buffersink = NULL;
	AVPixelFormat pix_fmt = AV_PIX_FMT_VAAPI;
	AVBufferSrcParameters *par;
	char *filter_dump;

	name_string = dvrname;
	decoder_ctx = dec_ctx;

	if (!dec_ctx->hw_frames_ctx)
	{
		bc_log(Warning, "watermarker: incoming frames are software decoded");

		software_decoding = true;
	}


	AVFilterInOut *outputs = avfilter_inout_alloc();
	AVFilterInOut *inputs  = avfilter_inout_alloc();

	filter_graph = avfilter_graph_alloc();

	if (!outputs || !inputs || !filter_graph)
	{
		ret = AVERROR(ENOMEM);
		goto end;
	}

	filter_graph->nb_threads = 1;

	buffersrc = avfilter_get_by_name("buffer");
	buffersink = avfilter_get_by_name("buffersink");

	if (!buffersrc || !buffersink)
	{
		bc_log(Error, "watermarker: filtering source or sink element not found");
		ret = AVERROR_UNKNOWN;
		goto end;
	}

	snprintf(args, sizeof(args),
		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
		dec_ctx->width, dec_ctx->height, /* dec_ctx->pix_fmt,  AV_PIX_FMT_VAAPI */ pix_fmt,
		dec_ctx->time_base.num, dec_ctx->time_base.den,
		dec_ctx->sample_aspect_ratio.num,
		dec_ctx->sample_aspect_ratio.den);

        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                args, NULL, filter_graph);

        if (ret < 0)
	{
		bc_log(Error, "watermarker: cannot create buffer source");
		goto end;
	}
	/* setting hw_frames_ctx */
	par = av_buffersrc_parameters_alloc();

	if (!par)
	{
		bc_log(Error, "watermarker: cannot alloc buffer parameters");
		goto end;
	}

	memset(par, 0, sizeof(*par));
	par->format = AV_PIX_FMT_NONE;
	//par->hw_frames_ctx = hw_frames_ctx;

	ret = av_buffersrc_parameters_set(buffersrc_ctx, par);
	if (ret < 0)
		goto end;
	av_freep(&par);
	/* ================== */

	ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
		NULL, NULL, filter_graph);

	if (ret < 0)
	{
		bc_log(Error, "watermarker: failed to create buffer sink");
		goto end;
	}

	ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
		/* (uint8_t*)&dec_ctx->pix_fmt, sizeof(dec_ctx->pix_fmt), */
		(uint8_t*)&pix_fmt, sizeof(pix_fmt),
		AV_OPT_SEARCH_CHILDREN);

	if (ret < 0)
	{
		bc_log(Error, "watermarker: cannot set output pixel format");
		goto end;
	}

	outputs->name       = av_strdup("in");
	outputs->filter_ctx = buffersrc_ctx;
	outputs->pad_idx    = 0;
	outputs->next       = NULL;

	inputs->name       = av_strdup("out");
	inputs->filter_ctx = buffersink_ctx;
	inputs->pad_idx    = 0;
	inputs->next       = NULL;

	if (!outputs->name || !inputs->name)
	{
		ret = AVERROR(ENOMEM);
		goto end;
	}

	snprintf(args, sizeof(args), "subtitles=filename=/usr/share/bluecherry/subtitle.ass:original_size=640x480");/*"scale_vaapi=w=%d:h=%d",
			out_width, out_height);*/


	if ((ret = avfilter_graph_parse_ptr(filter_graph, args,
                    &inputs, &outputs, NULL)) < 0)
		goto end;

	if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
		goto end;

	filter_dump = avfilter_graph_dump(filter_graph, NULL);
	bc_log(Info, "watermarker: created filter graph %s", filter_dump);
	av_free(filter_dump);

	out_frame = av_frame_alloc();
	if (!out_frame)
	{
		ret = AVERROR(ENOMEM);
		goto end;
	}

	return true;
end:
	av_strerror(ret, args, sizeof(args));
	bc_log(Error, "failed to init watermarker: %s", args);
	avfilter_inout_free(&inputs);
	avfilter_inout_free(&outputs);
	return false;
}

void watermarker::reinitialize(const AVCodecContext *updated_ctx)
{
		bc_log(Info, "watermarker:  reinitializing watermarker with new decoder context");
		/*
		if (software_decoding)
		{
			av_buffer_unref(&hw_frames_ctx);
			hw_frames_ctx = vaapi_hwaccel::alloc_frame_ctx(updated_ctx);

			if (!hw_frames_ctx)
				return;
		}
		else
			hw_frames_ctx = updated_ctx->hw_frames_ctx;
		*/

		decoder_ctx = updated_ctx;

		release_watermarker();

		if (!init_watermarker(name_string, updated_ctx))
		{
			bc_log(Error, "Failed to reinitialize watermarker for new input frame size");
			return;
		}

}

void watermarker::push_frame(AVFrame *in)
{
	int ret;

	bc_log(Debug, "watermarker: pushing decoded frame to subtitles filter");


	if (!software_decoding)
	{
		/* move frame data from video mem to system mem*/
		if (!vaapi_hwaccel::hwdownload_frame(decoder_ctx, in))
			return;

		bc_log(Debug, "watermarker: downloaded frame %dx%d from vaapi frame context");
        }


	ret = av_buffersrc_add_frame_flags(buffersrc_ctx, in, 0);

	if (ret < 0)
	{
		bc_log(Error, "watermarker: error while feeding the filtergraph");
		return;
	}
}

AVFrame *watermarker::watermarked_frame()
{
	int ret;

	ret = av_buffersink_get_frame(buffersink_ctx,out_frame);

	if (ret < 0)
	{
		if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
		{
			char args[512];
			av_strerror(ret, args, sizeof(args));
			bc_log(Error, "watermarker: failed to pull filtered frame from filtergraph - %s", args);
		}

		return NULL;
	}

	return out_frame;
}

