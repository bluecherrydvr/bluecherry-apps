#!/bin/bash

echo "SET FOREIGN_KEY_CHECKS=0;ALTER TABLE EventTypesCam  MODIFY \`id\` varchar(255) NOT NULL;\
ALTER TABLE EventTypesSys MODIFY \`id\` varchar(255) NOT NULL;\
ALTER TABLE TagNames MODIFY \`name\` varchar(255) DEFAULT NULL;\
ALTER TABLE ipPtzCommandPresets MODIFY \`driver\` varchar(12) DEFAULT '', MODIFY \`stop_zoom\` varchar(128) DEFAULT '' " | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

