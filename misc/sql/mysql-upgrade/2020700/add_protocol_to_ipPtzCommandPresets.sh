#!/bin/bash

echo 'ALTER TABLE ipPtzCommandPresets ADD COLUMN `protocol` varchar(10) DEFAULT "http";' | mysql -D"$dbname" -u"$user" -p"$password"

exit 0
