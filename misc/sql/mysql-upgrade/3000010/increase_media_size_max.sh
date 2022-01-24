#!/bin/bash

echo '
alter table Media modify size bigint;
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
