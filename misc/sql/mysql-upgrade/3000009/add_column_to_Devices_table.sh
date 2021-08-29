#!/bin/bash

echo '
ALTER TABLE Devices
  ADD `hls_segment_duration` decimal(6,3) DEFAULT '1.000' AFTER `motion_debug`;
ALTER TABLE Devices
  ADD `hls_segment_size` int(11) DEFAULT '1347584' AFTER `hls_segment_duration`;
ALTER TABLE Devices
  ADD `hls_window_size` smallint(5) DEFAULT '3' AFTER `hls_segment_size`;
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0