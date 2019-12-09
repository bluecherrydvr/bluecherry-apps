#!/bin/bash

echo "ALTER TABLE Devices MODIFY \`frame_downscale_factor\` decimal(4,3) DEFAULT '0.5'" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

