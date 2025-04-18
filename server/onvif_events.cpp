#include "onvif_events.h"
#include "trigger_processor.h"
#include "trigger_server.h"
#include "bc-server.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

onvif_events::onvif_events()
    : record(nullptr), exit_flag(false)
{
}

onvif_events::~onvif_events()
{
    stop();
}

void onvif_events::stop()
{
    exit_flag = true;
}

void onvif_events::run(bc_record *rec)
{
    record = rec;

    const char *fifo_path = "/tmp/bluecherry_trigger";
    char line[512];

    rec->log.log(Info, "ONVIF event listener starting for device %d", rec->id);

    // Open FIFO for reading
    int fd = open(fifo_path, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        rec->log.log(Error, "Failed to open %s for reading", fifo_path);
        return;
    }

    FILE *fp = fdopen(fd, "r");
    if (!fp) {
        rec->log.log(Error, "fdopen() failed for FIFO");
        close(fd);
        return;
    }

    while (!exit_flag) {
        if (fgets(line, sizeof(line), fp)) {
            char *newline = strchr(line, '\n');
            if (newline) *newline = '\0';

            // Format: "camera_id|event_type|action"
            int cam_id = -1;
            char event_type[256] = {0};
            char action[16] = {0};

            if (sscanf(line, "%d|%255[^|]|%15s", &cam_id, event_type, action) == 3) {
                if (cam_id == rec->id) {
                    trigger_processor *proc = trigger_server::Instance().find_processor(rec->id);
                    if (proc) {
                        // Format: "event_type|action"
                        char trigger_msg[512];
                        snprintf(trigger_msg, sizeof(trigger_msg), "%s|%s", event_type, action);
                        proc->trigger(trigger_msg);
                        
                        bc_db_query("INSERT INTO OnvifEvents (device_id, event_time, onvif_topic, action) VALUES(%i, NOW(), '%.255s', '%.15s')",
                                    rec->id, event_type, action);
                    } else {
                        rec->log.log(Error, "No trigger processor found for device %d", rec->id);
                    }
                }
            } else {
                rec->log.log(Warning, "Malformed trigger line: %s", line);
            }
        } else {
            // Sleep briefly to prevent busy loop
            usleep(100 * 1000); // 100ms
        }
    }

    fclose(fp);
    rec->log.log(Info, "ONVIF event listener exiting for device %d", rec->id);
}

