#!/bin/bash

if echo 'EXPLAIN Devices;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password" | grep 'motion_algorithm' >/dev/null
then
	echo 'This patch is already applied, skipping'
	exit 0
fi

echo 'ALTER TABLE Devices ADD `motion_algorithm` tinyint(1) DEFAULT "1" AFTER `motion_map`; ALTER TABLE Devices ADD `frame_downscale_factor` DECIMAL(2,1) DEFAULT "0.5" AFTER `motion_algorithm`; ALTER TABLE Devices ADD `min_motion_area` smallint(6) DEFAULT "5" AFTER `frame_downscale_factor`;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

