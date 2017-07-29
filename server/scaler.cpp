#include "scaler.h"

scaler::scaler()
	: buffersink_ctx(0), buffersrc_ctx(0),
	filter_graph(0), hw_frame_ctx(0)
{
}

scaler::~scaler()
{
}

bool scaler::init_scaler(int out_width, int out_height, AVBufferRef *hwframe_ctx)
{
	return false;
}

void scaler::push_frame(AVFrame *in)
{
}

AVFrame *scaler::scaled_frame()
{
	return NULL;
}

//there are different input and output hwframe contexts in vaapi_scale!!!

