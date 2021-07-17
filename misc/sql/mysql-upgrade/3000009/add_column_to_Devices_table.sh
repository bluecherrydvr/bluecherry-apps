#!/bin/bash

echo 'ALTER TABLE Devices ADD `hls_segment_size` int(11) DEFAULT '1347584' AFTER `motion_debug`;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

echo 'ALTER TABLE Devices ADD `hls_segment_duration` decimal(4,6) DEFAULT '3.0' AFTER `hls_segment_size`;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

echo 'ALTER TABLE Devices ADD `hls_window_size` smallint(2) DEFAULT '5' AFTER `hls_segment_duration`;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

