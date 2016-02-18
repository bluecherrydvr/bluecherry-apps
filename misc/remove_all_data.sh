#!/bin/bash -e

set -x # trace

function load_db_creds() {
	# ATTENTION! Duplication of load_db_creds.sh
	eval $(sed '/\(host\|dbname\|user\|password\)/!d;s/ *= */=/'";s/\"/'/g" /etc/bluecherry.conf)
	host=${host:-localhost}
}

function purge_files() {
	echo "Removing recording files"
	echo "SELECT filepath FROM Media" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password" | tail -n +2 | \
	while IFS= read -r MEDIAFILE
	do
		JPGFILE="`echo $MEDIAFILE | sed 's/[.]mkv$/.jpg/'`"
		rm -f $MEDIAFILE $JPGFILE

		# Try to remove empty basedirs recursively.
		# This is probably slow, but
		#  - this is one time thing
		#  - the input is unsorted (possible to improve)
		#  - removing each Media file is not the same as removing Storage dirs contents recursively
		BASEDIR="`dirname $MEDIAFILE`"
		while rmdir "$BASEDIR"
		do
			BASEDIR="`dirname $BASEDIR`"
		done
	done
}

function drop_db() {
	echo "Dropping database $dbname"
	echo "DROP DATABASE $dbname" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
}

CONFIGFILE=/etc/bluecherry.conf

load_db_creds

if ! echo "SELECT filepath FROM Media" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
then 
	echo "Can't connect mysql: " mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
	exit 1
fi

for ext in '~' '%' .bak .ucf-new .ucf-old .ucf-dist;  do
	rm -f $CONFIGFILE$ext || true
done

purge_files
drop_db

echo "Removing config file $CONFIGFILE"
rm -f $CONFIGFILE

userdel -r -f bluecherry || true
groupdel bluecherry || true
