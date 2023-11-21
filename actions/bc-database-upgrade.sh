#!/bin/bash

set -e

if ! echo 'exit' | mysql -h "$BLUECHERRY_DB_HOST" -D"$BLUECHERRY_DB_NAME" \
                         -u"$BLUECHERRY_DB_USER" -p"$BLUECHERRY_DB_PASSWORD"
then
    echo "Can't connect mysql with credentials in /etc/bluecherry.conf"
    echo "Please remove config or fix config"
    exit 1
fi

DB_BACKUP_GZ_FILE=$(mktemp ~bluecherry/bc_db_backup.XXXXXXXXXX.sql.gz)
echo "Going to updgrade Bluecherry DB. Taking a backup into $DB_BACKUP_GZ_FILE just in case" >&2

# Backup the DB
mysqldump -h "$BLUECHERRY_DB_HOST" "$BLUECHERRY_DB_NAME" -u"$BLUECHERRY_DB_USER" \
          -p"$BLUECHERRY_DB_PASSWORD" | gzip -c > $DB_BACKUP_GZ_FILE

if ! /usr/share/bluecherry/bc_db_tool.sh upgrade_db \
        "$BLUECHERRY_DB_NAME" "$BLUECHERRY_DB_USER" \
        "$BLUECHERRY_DB_PASSWORD" "$BLUECHERRY_DB_HOST"
then
    echo Failed upgrade database, restoring backup
    restore_mysql_backup "$BLUECHERRY_DB_NAME" "$BLUECHERRY_DB_USER" \
                         "$BLUECHERRY_DB_PASSWORD" "$BLUECHERRY_DB_HOST"
    exit 1
fi

# database successfully upgraded
if [[ -f "$DB_BACKUP_GZ_FILE" ]]
then
    rm -f "$DB_BACKUP_GZ_FILE"
fi

exit 0
