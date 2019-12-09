#!/bin/bash

set -e

# NOTE Beware of braindead host configs having two Storage entries, pointing to a mountpoint and to a symlink to the same mountpoint. Thus, for each DB list entry, `readlink -f` should be applied.
# NOTE If there's .mkv file in DB list, assume there MAY be .jpg file with same name

DB_LIST_FILE=/tmp/bluecherry_cleanup.db.list
STORAGES_LIST_FILE=/tmp/bluecherry_cleanup.storages.list
FS_LIST_FILE=/tmp/bluecherry_cleanup.fs.list
RM_LIST_FILE=/tmp/bluecherry_cleanup.rm.list
OK_LIST_FILE=/tmp/bluecherry_cleanup.ok.list

function rm_files() {
	rm ${DB_LIST_FILE}* ${STORAGES_LIST_FILE}* ${FS_LIST_FILE}* ${RM_LIST_FILE}* ${OK_LIST_FILE}* &>/dev/null || true
}

rm_files

# Ensure bc-server is stopped
if ps ax | grep bc-server | grep -v grep &>/dev/null
then
	echo -e "\n\n\tWARNING! bc-server is running. It must be stopped for stable result of this script\n\n"
fi

# Dump the list of absolute filenames from Media table (DB list)
. /usr/share/bluecherry/load_db_creds.sh
echo "Dumping DB list from Media table"
date
time echo "SELECT filepath FROM Media" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password" | tail -n +2 > ${DB_LIST_FILE}.raw

# Do `readlink -f` for each entry.
echo "Resolving absolute file paths"
date
set +e
I=1
time while true
do
	ENTRY=`tail -n +$I ${DB_LIST_FILE}.raw | tail -n 1`
	if [[ -z "$ENTRY" ]]
	then
		break
	fi
	I=$(( I + 1 ))

	ABS=`readlink -f "$ENTRY"`
	if [[ $? != 0 ]]
	then
		echo "Absolute path of DB entry $ENTRY cannot be resolved"
		echo "$ENTRY" >> ${DB_LIST_FILE}.unresolved
	else
		echo "$ABS" >> ${DB_LIST_FILE}.resolved
	fi
done
set -e

echo "Sorting results"
date
sort ${DB_LIST_FILE}.resolved > ${DB_LIST_FILE}.sorted
echo "Sorted"
date


# Dump the list of absolute filenames in all directories being Storage table entries (FS list)
echo "SELECT path FROM Storage" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password" | tail -n +2 > ${STORAGES_LIST_FILE}.raw
# Patch list with default value, which is actual when the table is empty
echo "/var/lib/bluecherry/recordings" >> ${STORAGES_LIST_FILE}.raw
# Drop trailing slash for uniformness, to avoid duplication
sed 's/\/$//' ${STORAGES_LIST_FILE}.raw > ${STORAGES_LIST_FILE}.patched
# Do `readlink -f` for those dirs.
set +e
I=1
while true
do
	ENTRY=`tail -n +$I ${STORAGES_LIST_FILE}.patched | tail -n 1`
	if [[ -z "$ENTRY" ]]
	then
		break
	fi
	I=$(( I + 1 ))

	ABS=`readlink -f "$ENTRY"`
	if [[ $? != 0 ]]
	then
		echo "Absolute path of Storage entry $ENTRY cannot be resolved"
	else
		echo "$ABS" >> ${STORAGES_LIST_FILE}.resolved
	fi
done
set -e

# Apply `sort | uniq` for the final aggregated list.
sort ${STORAGES_LIST_FILE}.resolved | uniq > ${STORAGES_LIST_FILE}.filtered

# Build actual files list
echo "Building list of files existing in filesystem"
date
I=1
time while true
do
	ENTRY=`tail -n +$I ${STORAGES_LIST_FILE}.filtered | tail -n 1`
	if [[ -z "$ENTRY" ]]
	then
		break
	fi
	I=$(( I + 1 ))

	find "$ENTRY" -type f >> ${FS_LIST_FILE}.raw
done
echo "Built"
date

echo "Checking each file whether it is lost for tracking"
date
I=1
time while true
do
	ENTRY=`tail -n +$I ${FS_LIST_FILE}.raw | tail -n 1`
	if [[ -z "$ENTRY" ]]
	then
		break
	fi
	I=$(( I + 1 ))

	# if the file is .mkv, AND if it is absent in DB list, remove
	if [[ "$ENTRY" =~ '[.]mkv$' ]] && ! grep "$ENTRY" ${DB_LIST_FILE}.sorted > /dev/null
	then
		echo "$ENTRY" >> ${RM_LIST}
	# if the file is .jpg, AND if it is absent in DB list, AND .mkv with same name and path is absent in DB list, remove
	elif [[ "$ENTRY" =~ '[.]jpg$' ]] && ! grep "$ENTRY" ${DB_LIST_FILE}.sorted > /dev/null
	then
		RELATED_MKV=`echo "$ENTRY" | sed 's/[.]jpg$/.mkv/'`
		if ! grep "$RELATED_MKV" ${DB_LIST_FILE}.sorted > /dev/null
		then
			echo "$ENTRY" >> ${RM_LIST_FILE}
		fi
	else
		echo "$ENTRY" >> ${OK_LIST_FILE}
	fi
done
echo "Done"
date

if [[ "$1" != "no-dry-run" ]]
then
	echo -e "Dry run finished, see ${RM_LIST_FILE} . To proceed with removal after building lists, launch this way: \n\n\t$0 no-dry-run\n\n"
	exit 0
fi

echo "Removing untracked files"
date
I=1
time while true
do
	ENTRY=`tail -n +$I ${RM_LIST_FILE} | tail -n 1`
	if [[ -z "$ENTRY" ]]
	then
		break
	fi
	I=$(( I + 1 ))

	rm -f "$ENTRY"
done
echo "Removed files"
date

# Remove empty dirs recursively
echo "Going to remove empty directories in Storages"
date
I=1
time while true
do
	ENTRY=`tail -n +$I ${STORAGES_LIST_FILE}.filtered | tail -n 1`
	if [[ -z "$ENTRY" ]]
	then
		break
	fi
	I=$(( I + 1 ))

	touch "${ENTRY}"/.keep  # prevent removing the dir itself
	find "$ENTRY" -type d -empty -delete
	rm "${ENTRY}"/.keep
done
echo "Done"
date

rm_files
