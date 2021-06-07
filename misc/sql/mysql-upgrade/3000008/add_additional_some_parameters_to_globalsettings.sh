#!/bin/bash

echo 'INSERT INTO `GlobalSettings` VALUES '"('G_LIVEVIEW_VIDEO_METHOD', 'HLS')" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

echo 'INSERT INTO `GlobalSettings` VALUES '"('G_SUBDOMAIN_API_BASE_URL', 'https://domains.bluecherrydvr.com/subdomain-provider')" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

echo 'INSERT INTO `GlobalSettings` VALUES '"('G_SUBDOMAIN_EMAIL_ACCOUNT', NULL)" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
