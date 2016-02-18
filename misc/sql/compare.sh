#!/bin/bash -e

REFERENCE_DUMPFILE=/usr/share/bluecherry/schema_mysql.sql

. /usr/share/bluecherry/load_db_creds.sh

DUMPFILE=`mktemp`

function cleanup() {
	trap - EXIT
	rm $DUMPFILE
}
trap cleanup EXIT

mysqldump "$dbname" --no-data --skip-comments -h "$host" -u"$user" -p"$password" > $DUMPFILE

function filter_variables() {
sed -e 's/ AUTO_INCREMENT=\([^ ;]\+\)//'
}

if diff -Nurd <(cat $REFERENCE_DUMPFILE | filter_variables ) <(cat $DUMPFILE | filter_variables )
then
	echo "Database matches reference schema"
	exit 0
else
	echo "Database doesn't match reference schema"
	exit 1
fi
