#!/bin/bash

echo 'ALTER TABLE Devices ADD `onvif_port` mediumint(5) DEFAULT '80' AFTER `mjpeg_path`;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

