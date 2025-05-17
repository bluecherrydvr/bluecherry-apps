#!/bin/bash

echo '
-- Add new ONVIF event types
INSERT INTO EventTypesCam VALUES ("person");
INSERT INTO EventTypesCam VALUES ("animal");
INSERT INTO EventTypesCam VALUES ("vehicle");

-- Add column to store enabled ONVIF event types per device
ALTER TABLE Devices 
ADD COLUMN onvif_event_types VARCHAR(255) DEFAULT NULL 
COMMENT "Comma-separated list of enabled ONVIF event types";

-- Add event type column to OnvifEvents table
ALTER TABLE OnvifEvents
ADD COLUMN event_type VARCHAR(50) DEFAULT NULL 
COMMENT "Type of ONVIF event (person, animal, vehicle)";
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0 