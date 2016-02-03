#!/bin/bash
set -e # interrupt on failure
. /usr/share/bluecherry/load_db_creds.sh

BKP_ENABLED=`echo "SELECT value FROM GlobalSettings WHERE parameter = 'G_CRON_BKP_ENABLED'" | mysql -h${host} -D"$dbname" -u"$user" -p"$password" | tail -n 1`
if [[ $BKP_ENABLED != 'yes' ]]
then
	exit
fi

STORAGE=`echo "SELECT path FROM Storage ORDER BY priority" | mysql -h${host} -D"$dbname" -u"$user" -p"$password" | tail -n 1`
STORAGE=${STORAGE:-/var/lib/bluecherry/recordings}
DB_BACKUP_GZ_FILE=${STORAGE}/db_backup.sql.gz

# This can be used to ignore some tables
PARAMS=`echo "SELECT value FROM GlobalSettings WHERE parameter = 'G_CRON_BKP_PARAMS'" | mysql -h${host} -D"$dbname" -u"$user" -p"$password" | tail -n 1`

mysqldump --single-transaction -h${host} "$dbname" -u"$user" -p"$password" $PARAMS | gzip -c > ${DB_BACKUP_GZ_FILE}.tmp
mv ${DB_BACKUP_GZ_FILE}.tmp $DB_BACKUP_GZ_FILE
