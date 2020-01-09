#!/bin/bash

echo 'ALTER TABLE Devices ADD `substream_mode` tinyint(1) DEFAULT '0' AFTER `reencode_frame_height`; ALTER TABLE Devices ADD `substream_path` varchar(255) DEFAULT NULL AFTER `substream_mode`;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

