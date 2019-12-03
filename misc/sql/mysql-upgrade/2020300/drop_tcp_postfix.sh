#!/bin/bash

echo 'UPDATE ipCameraDriver SET rtsp_path = SUBSTRING(rtsp_path, 1, CHAR_LENGTH(rtsp_path) - 4) WHERE rtsp_path LIKE "%?tcp"' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
