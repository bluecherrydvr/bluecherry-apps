#!/bin/bash

echo '
ALTER TABLE Devices MODIFY model varchar(64);
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
