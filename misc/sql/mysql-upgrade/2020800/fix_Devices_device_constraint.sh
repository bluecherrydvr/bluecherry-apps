#!/bin/bash

echo 'ALTER TABLE Devices DROP KEY `device`; ALTER TABLE Devices ADD CONSTRAINT `device` UNIQUE KEY `device` (`device`, `mjpeg_path`, `protocol`, `channel`);' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
