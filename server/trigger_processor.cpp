#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "bt.h"

#include "trigger_processor.h"
#include "trigger_server.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libavutil/dict.h>
}


trigger_processor::trigger_processor(int cam_id)
    : stream_consumer("Trigger Processor"), destroy_flag(false), camera_id(cam_id), trigger_flag(false)
{
    pthread_mutex_init(&trigger_flag_lock, NULL);
    output_source = new stream_source("Trigger Detection");
    trigger_server::Instance().register_processor(camera_id, this);
}

void trigger_processor::trigger(const char* msg)
{
    bc_log(Info, "Trigger message received: '%s'", msg);

    int cam_id;
    char event_type[64];
    char action[16];

    if (sscanf(msg, "%d|%63[^|]|%15s", &cam_id, event_type, action) != 3) {
        bc_log(Error, "Invalid trigger message format: '%s'", msg);
        return;
    }

    bc_log(Info, "Parsed trigger - cam_id: %d, event: %s, action: %s", cam_id, event_type, action); 

    if (strcmp(action, "start") == 0) {
        pthread_mutex_lock(&trigger_flag_lock);
        trigger_flag = true;
        pthread_mutex_unlock(&trigger_flag_lock);
        bc_log(Info, "Triggered START for device %d with event '%s'", cam_id, event_type);
    } else if (strcmp(action, "stop") == 0) {
        pthread_mutex_lock(&trigger_flag_lock);
        trigger_flag = false;
        pthread_mutex_unlock(&trigger_flag_lock);
        bc_log(Info, "Triggered STOP for device %d with event '%s'", cam_id, event_type);
    } else {
        bc_log(Error, "Unknown trigger action: '%s'", action);
    }
}


trigger_processor::~trigger_processor()
{
	trigger_server::Instance().unregister_processor(camera_id, this);
	pthread_mutex_destroy(&trigger_flag_lock);
	delete output_source;
}

void trigger_processor::destroy()
{
	destroy_flag = true;
	buffer_wait.notify_all();
}

void trigger_processor::receive(const stream_packet &packet)
{
    bool cached_trigger_flag;
    pthread_mutex_lock(&trigger_flag_lock);
    cached_trigger_flag = trigger_flag;
    pthread_mutex_unlock(&trigger_flag_lock);

    if (cached_trigger_flag) {
        packet.flags |= stream_packet::TriggerFlag;
        bc_log(Info, "Marking the passed packet with TriggerFlag");
    }

    output_source->send(packet);
}


