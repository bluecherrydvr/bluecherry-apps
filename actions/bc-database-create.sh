#!/bin/bash

set -e

# Create a new bluecherry server database, following the same procedure used
# by bluecherry's "official" script from bluecherry-apps:misc/postinstall.sh
# but using environment variables as injected from docker or docker-compose.

/usr/share/bluecherry/bc_db_tool.sh new_db \
    "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" \
    "$BLUECHERRY_DB_NAME" "$BLUECHERRY_DB_USER" "$BLUECHERRY_DB_PASSWORD" \
    "$BLUECHERRY_DB_HOST" "$BLUECHERRY_USERHOST" \
    || exit 1

exit 0
