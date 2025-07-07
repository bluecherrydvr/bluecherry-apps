#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "bt.h"

#include "trigger_processor.h"
#include "trigger_server.h"
#include "bc-server.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libavutil/dict.h>
}

void trigger_processor::trigger(const char *description)
{
    pthread_mutex_lock(&trigger_flag_lock);
    trigger_flag = true;
    pthread_mutex_unlock(&trigger_flag_lock);

    // Extract type from description (before the | character)
    char type[64] = {0};
    const char *pipe_pos = strchr(description, '|');
    if (pipe_pos) {
        strncpy(type, description, pipe_pos - description);
        type[pipe_pos - description] = '\0';
    } else {
        strncpy(type, description, sizeof(type) - 1);
    }
    
    // Convert to lowercase for consistency
    for (int i = 0; type[i]; i++) {
        type[i] = tolower(type[i]);
    }

    // Insert a new event into EventsCam for this trigger
    bc_db_query("INSERT INTO EventsCam (time, level_id, device_id, type_id, details) "
                "VALUES (UNIX_TIMESTAMP(), 'info', %d, '%s', 'triggered')", camera_id, type);
    bc_log(Info, "Trigger event recorded: camera_id=%d, type=%s, details=triggered", camera_id, type);
    printf("Trigger: %s\n", description);
}

trigger_processor::trigger_processor(int camera_id)
    : stream_consumer("Trigger Processor"),
      destroy_flag(false),
      camera_id(camera_id),
      trigger_flag(false)
{
    pthread_mutex_init(&trigger_flag_lock, NULL);
    output_source = new stream_source("Trigger Detection");
    trigger_server::Instance().register_processor(camera_id, this);
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
	// TODO If flag is set, reset it to 0 and set packet's flag
	bool cached_trigger_flag;
	pthread_mutex_lock(&trigger_flag_lock);
	cached_trigger_flag = trigger_flag;
	trigger_flag = false;
	pthread_mutex_unlock(&trigger_flag_lock);

	if (cached_trigger_flag) {
		packet.flags |= stream_packet::TriggerFlag;
		bc_log(Info, "Marking the passed packet with TriggerFlag");
	}
	output_source->send(packet);
}


