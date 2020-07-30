#!/bin/bash

echo 'ALTER TABLE Devices ADD `onvif_events_enabled` tinyint(1) DEFAULT '0';' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

