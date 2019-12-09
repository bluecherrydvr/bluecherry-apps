#!/bin/bash

echo 'DROP TABLE ipCameras; DROP TABLE ipCameraDriver;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
