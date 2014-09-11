#!/bin/bash

echo 'DROP TABLE ipCameras; DROP TABLE ipCameraDriver;' | mysql -D"$dbname" -u"$user" -p"$password"

exit 0
