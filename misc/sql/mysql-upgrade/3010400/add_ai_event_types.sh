#!/bin/bash

echo '
-- Add new AI event types
INSERT INTO EventTypesCam VALUES ("ai_human");
INSERT INTO EventTypesCam VALUES ("ai_vehicle");
INSERT INTO EventTypesCam VALUES ("ai_animal");

-- Add column to store enabled AI event types per device
ALTER TABLE Devices 
ADD COLUMN ai_event_types VARCHAR(255) DEFAULT NULL 
COMMENT "Comma-separated list of enabled AI event types (e.g., ai_human, ai_vehicle, ai_animal)";
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0 