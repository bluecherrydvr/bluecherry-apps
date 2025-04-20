#!/bin/bash

echo '
-- Add new ONVIF event types
INSERT INTO EventTypesCam VALUES ("motion_detection");
INSERT INTO EventTypesCam VALUES ("tampering_detection");
INSERT INTO EventTypesCam VALUES ("audio_detection");
INSERT INTO EventTypesCam VALUES ("video_loss");
INSERT INTO EventTypesCam VALUES ("storage_failure");
INSERT INTO EventTypesCam VALUES ("network_loss");
INSERT INTO EventTypesCam VALUES ("line_crossing");
INSERT INTO EventTypesCam VALUES ("object_left");
INSERT INTO EventTypesCam VALUES ("object_removed");
INSERT INTO EventTypesCam VALUES ("object_counting");
INSERT INTO EventTypesCam VALUES ("face_detection");
INSERT INTO EventTypesCam VALUES ("license_plate_recognition");
INSERT INTO EventTypesCam VALUES ("audio_level_detection");
INSERT INTO EventTypesCam VALUES ("temperature_detection");
INSERT INTO EventTypesCam VALUES ("humidity_detection");
INSERT INTO EventTypesCam VALUES ("smoke_detection");
INSERT INTO EventTypesCam VALUES ("fire_detection");
INSERT INTO EventTypesCam VALUES ("water_detection");
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0 