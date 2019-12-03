#!/bin/bash

echo "ALTER TABLE ActiveUsers MODIFY \`ip\` varchar(64) DEFAULT NULL" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

