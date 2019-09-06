#!/bin/bash

echo "ALTER TABLE ActiveUsers MODIFY \`frame_downscale_factor\` decimal(4,3) DEFAULT '0.5'" | mysql -D"$dbname" -u"$user" -p"$password"

exit 0

