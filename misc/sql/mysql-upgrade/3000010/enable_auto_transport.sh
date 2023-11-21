#!/bin/bash
# Enable RTSP auto transport '0'

echo '
alter table Devices modify rtsp_rtp_prefer_tcp smallint(5) NOT NULL DEFAULT '0';
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
