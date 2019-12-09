#!/bin/bash

echo 'INSERT INTO `ipCameraDriver` (`id`, `rtsp_path`, `mjpeg_path`, `rtsp_port`, `mjpeg_port`) VALUES '"('RTSP-VAPIX-AUTO-2', '/axis-media/media.amp', '/mjpg/video.mjpg', 554, 80)" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

echo 'INSERT INTO `ipCameras` (`id`, `type`, `manufacturer`, `model`, `compression`, `resolutions`, `driver`) VALUES '"(378, 'Camera', 'Axis', 'M1034-W', 'H264,MJPEG', '', 'RTSP-VAPIX-AUTO-2')" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
