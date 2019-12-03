#!/bin/bash

echo 'ALTER TABLE Devices ADD `reencode_livestream` tinyint(1) DEFAULT '0' AFTER `debug_level`;ALTER TABLE Devices ADD `reencode_bitrate` int(11) NOT NULL DEFAULT '64000' AFTER `reencode_livestream`;ALTER TABLE Devices ADD `reencode_frame_width` smallint(6) NOT NULL DEFAULT '352' AFTER `reencode_bitrate`;ALTER TABLE Devices ADD `reencode_frame_height` smallint(6) NOT NULL DEFAULT '240' AFTER `reencode_frame_width`;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

