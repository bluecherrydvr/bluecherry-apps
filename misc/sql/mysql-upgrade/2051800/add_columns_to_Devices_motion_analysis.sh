#!/bin/bash

echo 'ALTER TABLE Devices ADD `motion_analysis_ssw_length` int DEFAULT "-1" AFTER `min_motion_area`; ALTER TABLE Devices ADD `motion_analysis_percentage` int DEFAULT "-1" AFTER `motion_analysis_ssw_length`;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

