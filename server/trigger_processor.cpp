#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "bt.h"

#include "trigger_processor.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libavutil/dict.h>
}

trigger_processor::trigger_processor()
	: stream_consumer("Trigger Processor"), destroy_flag(false)
	  , output_source(this)
{
	output_source = new stream_source("Trigger Processor");
}

trigger_processor::~trigger_processor()
{
	delete output_source;
}

void trigger_processor::destroy()
{
	destroy_flag = true;
	buffer_wait.notify_all();
}

void stream_consumer::receive(const stream_packet &packet)
{
	send(packet);
}
