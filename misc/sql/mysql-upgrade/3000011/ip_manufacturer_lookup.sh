#!/bin/bash
# Enable live lookups of IP camera manufacturers (pulled from our API)

echo "
INSERT INTO GlobalSettings VALUES ('G_DATA_SOURCE','live');
" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
