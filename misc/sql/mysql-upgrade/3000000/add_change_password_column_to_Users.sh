#!/bin/bash

if echo 'EXPLAIN Users;' | mysql -D"$dbname" -u"$user" -p"$password" | grep 'change_password' >/dev/null
then
        echo 'This patch is already applied, skipping'
        exit 0
fi

echo 'ALTER TABLE Users ADD `change_password` tinyint(1) DEFAULT NULL AFTER `access_schedule`;' | mysql -D"$dbname" -u"$user" -p"$password"

exit 0

