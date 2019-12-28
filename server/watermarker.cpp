#ifdef WATERMARKER_ENABLED
#include "watermarker.h"
#include "vaapi.h"

#include <ass/ass.h>


typedef struct AssContext {
    const AVClass *avclass;
    ASS_Library  *library;
    ASS_Renderer *renderer;
    ASS_Track    *track;
    char *filename;
    char *fontsdir;
    char *charenc;
    char *force_style;
    int stream_index;
    int alpha;
    uint8_t rgba_map[4];
    int     pix_step[4];       ///< steps per pixel for each plane of the main output
    int original_w, original_h;
    int shaping;
    //FFDrawContext draw;
} AssContext;

watermarker::watermarker()
	: dvr_name(nullptr), camera_name(nullptr),
	buffersink_ctx(0), buffersrc_ctx(0),
	filter_graph(0), decoder_ctx(0), out_frame(0),
	track(0), software_decoding(false),
	last_timestamp(0)
{
}

void watermarker::release_watermarker()
{
	if (track)
	{
		track->events[0].Text = NULL;
		track->events[1].Text = NULL;
	}

	avfilter_graph_free(&filter_graph);

	av_frame_free(&out_frame);
}

watermarker::~watermarker()
{
	release_watermarker();
}

void watermarker::find_asstrack()
{
	for (int i = 0; i < filter_graph->nb_filters; i++)
	{
		if (strcmp(filter_graph->filters[i]->filter->name, "subtitles") == 0)
		{
			AssContext *assctx = (AssContext*)filter_graph->filters[i]->priv;

			track = assctx->track;

			bc_log(Debug, "watermarker: ass context found %s", track->events[0].Text);

			free(track->events[0].Text);
			free(track->events[1].Text);
			track->events[0].Text = (char*)dvr_name;
			track->events[1].Text = (char*)camera_name;

			break;
		}
	}
}

bool watermarker::init_watermarker(const char *dvrname, const char *cameraname, const AVCodecContext *dec_ctx)
{
	char args[512];
	int ret = 0;
	const AVFilter *buffersrc = NULL;
	const AVFilter *buffersink = NULL;
	AVBufferSrcParameters *par;
	char *filter_dump;

	dvr_name = dvrname;
	camera_name = cameraname;
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
		dec_ctx->width, dec_ctx->height,
		software_decoding ? dec_ctx->pix_fmt : dec_ctx->sw_pix_fmt,
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
		software_decoding ? (uint8_t*)&dec_ctx->pix_fmt : (uint8_t*)&dec_ctx->sw_pix_fmt,
		sizeof(dec_ctx->pix_fmt),
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

	if (dec_ctx->width >= 640)
		snprintf(args, sizeof(args),
			"subtitles=filename=/usr/share/bluecherry/subtitle.ass:original_size=640x480");
	else
		snprintf(args, sizeof(args),
			"subtitles=filename=/usr/share/bluecherry/subtitle_lowres.ass:original_size=352x240");


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

	find_asstrack();

	return true;
end:
	av_strerror(ret, args, sizeof(args));
	bc_log(Error, "failed to init watermarker: %s", args);
	avfilter_inout_free(&inputs);
	avfilter_inout_free(&outputs);
	avfilter_graph_free(&filter_graph);
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

		if (!init_watermarker(dvr_name, camera_name, updated_ctx))
		{
			bc_log(Error, "Failed to reinitialize watermarker for new input frame size");
			return;
		}

}

void watermarker::push_frame(AVFrame *in)
{
	int ret;
	time_t t;
	int namelen;

	bc_log(Debug, "watermarker: pushing decoded frame to subtitles filter");


	if (!software_decoding)
	{
		/* move frame data from video mem to system mem*/
		if (!vaapi_hwaccel::hwdownload_frame(decoder_ctx, in))
			return;
        }

	if (in->width == 0 || in->height == 0)
	{
		bc_log(Error, "watermarker: empty frame received, discarding");
		return;
	}

	t = time(NULL);

	if (last_timestamp != t)
	{
		track->events[0].Duration = LLONG_MAX;
		track->events[1].Duration = LLONG_MAX;

		track->events[0].Text = (char*)dvr_name;

		namelen = strlen(camera_name);

		namelen = namelen > (sizeof timestamp_buf) / 2 ? (sizeof timestamp_buf) / 2 : namelen;

		memcpy(timestamp_buf, camera_name, namelen);
		timestamp_buf[namelen] = '\0';

		strftime(timestamp_buf + namelen,sizeof timestamp_buf - namelen, " %F %H:%M:%S", localtime(&t));
		track->events[1].Text = timestamp_buf;

		bc_log(Debug, "watermarker: track %s %lld %lld", track->events[0].Text, track->events[0].Start, track->events[0].Duration);

		last_timestamp = t;
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
#endif
