#!/bin/bash

echo 'CREATE INDEX time_index USING BTREE ON EventsCam (time);' | mysql -D"$dbname" -u"$user" -p"$password"

exit 0
