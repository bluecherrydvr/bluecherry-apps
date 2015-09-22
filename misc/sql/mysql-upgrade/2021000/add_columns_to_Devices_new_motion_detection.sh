#!/bin/bash

echo 'ALTER TABLE Devices ADD `default_motion_algorithm` tinyint(1) DEFAULT "1" AFTER `motion_map`; ALTER TABLE Devices ADD `frame_downscale_factor` DECIMAL(2,1) DEFAULT "0.1" AFTER `default_motion_algorithm`; ALTER TABLE Devices ADD `min_motion_area` smallint(6) DEFAULT "50" AFTER `frame_downscale_factor`;' | mysql -D"$dbname" -u"$user" -p"$password"

exit 0

