#!/bin/bash

set -e

# Script to scan recording files and add missing ones to the database
# This is useful after VM restores or when recordings exist but aren't in the DB

DB_LIST_FILE=/tmp/bluecherry_add_missing.db.list
STORAGES_LIST_FILE=/tmp/bluecherry_add_missing.storages.list
FS_LIST_FILE=/tmp/bluecherry_add_missing.fs.list
MISSING_LIST_FILE=/tmp/bluecherry_add_missing.missing.list

function cleanup_files() {
    rm ${DB_LIST_FILE}* ${STORAGES_LIST_FILE}* ${FS_LIST_FILE}* ${MISSING_LIST_FILE}* &>/dev/null || true
}

cleanup_files

echo "=== Bluecherry Missing Recordings Scanner ==="
echo "This script will scan for recording files and add missing ones to the database"
echo ""

# Ensure bc-server is stopped for stable results
if ps ax | grep bc-server | grep -v grep &>/dev/null
then
    echo -e "\n\n\tWARNING! bc-server is running. It must be stopped for stable result of this script\n\n"
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Load database credentials
if [ -f /usr/share/bluecherry/load_db_creds.sh ]; then
    . /usr/share/bluecherry/load_db_creds.sh
else
    echo "ERROR: Cannot find database credentials file"
    exit 1
fi

echo "Step 1: Getting list of files already in database..."
date

# Get list of files already in database
echo "SELECT filepath FROM Media WHERE filepath != ''" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password" | tail -n +2 > ${DB_LIST_FILE}.raw

# Resolve absolute paths for database entries
echo "Resolving absolute file paths from database..."
set +e
I=1
while true
do
    ENTRY=`tail -n +$I ${DB_LIST_FILE}.raw | tail -n 1`
    if [[ -z "$ENTRY" ]]; then
        break
    fi
    I=$(( I + 1 ))

    ABS=`readlink -f "$ENTRY"`
    if [[ $? != 0 ]]; then
        echo "Warning: Absolute path of DB entry $ENTRY cannot be resolved"
    else
        echo "$ABS" >> ${DB_LIST_FILE}.resolved
    fi
done
set -e

sort ${DB_LIST_FILE}.resolved > ${DB_LIST_FILE}.sorted
DB_COUNT=$(wc -l < ${DB_LIST_FILE}.sorted)
echo "Found $DB_COUNT files already in database"

echo ""
echo "Step 2: Getting storage paths..."
date

# Get storage paths from database
echo "SELECT path FROM Storage" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password" | tail -n +2 > ${STORAGES_LIST_FILE}.raw
# Add default storage path
echo "/var/lib/bluecherry/recordings" >> ${STORAGES_LIST_FILE}.raw
# Remove trailing slashes
sed 's/\/$//' ${STORAGES_LIST_FILE}.raw > ${STORAGES_LIST_FILE}.patched

# Resolve absolute paths for storage directories
set +e
I=1
while true
do
    ENTRY=`tail -n +$I ${STORAGES_LIST_FILE}.patched | tail -n 1`
    if [[ -z "$ENTRY" ]]; then
        break
    fi
    I=$(( I + 1 ))

    ABS=`readlink -f "$ENTRY"`
    if [[ $? != 0 ]]; then
        echo "Warning: Absolute path of Storage entry $ENTRY cannot be resolved"
    else
        echo "$ABS" >> ${STORAGES_LIST_FILE}.resolved
    fi
done
set -e

sort ${STORAGES_LIST_FILE}.resolved | uniq > ${STORAGES_LIST_FILE}.filtered

echo ""
echo "Step 3: Scanning filesystem for recording files..."
date

# Build list of actual files in filesystem
I=1
while true
do
    STORAGE_PATH=`tail -n +$I ${STORAGES_LIST_FILE}.filtered | tail -n 1`
    if [[ -z "$STORAGE_PATH" ]]; then
        break
    fi
    I=$(( I + 1 ))

    if [ -d "$STORAGE_PATH" ]; then
        echo "Scanning: $STORAGE_PATH"
        find "$STORAGE_PATH" -type f \( -name "*.mp4" -o -name "*.mkv" -o -name "*.avi" \) 2>/dev/null >> ${FS_LIST_FILE}.raw
    else
        echo "Warning: Storage path does not exist: $STORAGE_PATH"
    fi
done

# Resolve absolute paths for filesystem files
set +e
I=1
while true
do
    ENTRY=`tail -n +$I ${FS_LIST_FILE}.raw | tail -n 1`
    if [[ -z "$ENTRY" ]]; then
        break
    fi
    I=$(( I + 1 ))

    ABS=`readlink -f "$ENTRY"`
    if [[ $? != 0 ]]; then
        echo "Warning: Absolute path of file $ENTRY cannot be resolved"
    else
        echo "$ABS" >> ${FS_LIST_FILE}.resolved
    fi
done
set -e

sort ${FS_LIST_FILE}.resolved | uniq > ${FS_LIST_FILE}.sorted
FS_COUNT=$(wc -l < ${FS_LIST_FILE}.sorted)
echo "Found $FS_COUNT files in filesystem"

echo ""
echo "Step 4: Finding missing files..."
date

# Find files that exist in filesystem but not in database
comm -23 ${FS_LIST_FILE}.sorted ${DB_LIST_FILE}.sorted > ${MISSING_LIST_FILE}
MISSING_COUNT=$(wc -l < ${MISSING_LIST_FILE})

echo "Found $MISSING_COUNT files missing from database"

if [ $MISSING_COUNT -eq 0 ]; then
    echo ""
    echo "✅ No missing files found! Database is in sync with filesystem."
    cleanup_files
    exit 0
fi

echo ""
echo "Step 5: Adding missing files to database..."
date

# Function to extract device ID from file path
get_device_id() {
    local filepath="$1"
    # Extract device ID from path like /path/to/recordings/YYYY/MM/DD/000001/filename.mp4
    local device_id=$(echo "$filepath" | sed -n 's/.*\/\([0-9]\{6\}\)\/.*/\1/p')
    if [ -n "$device_id" ]; then
        echo "$device_id"
    else
        echo "0"
    fi
}

# Function to extract timestamp from filename
get_timestamp() {
    local filepath="$1"
    local filename=$(basename "$filepath")
    # Try to extract timestamp from filename patterns
    local timestamp=$(echo "$filename" | sed -n 's/.*\([0-9]\{4\}-[0-9]\{2\}-[0-9]\{2\}-[0-9]\{2\}-[0-9]\{2\}-[0-9]\{2\}\).*/\1/p')
    if [ -n "$timestamp" ]; then
        echo "$timestamp"
    else
        # Use file modification time as fallback
        date -r "$filepath" '+%Y-%m-%d-%H-%M-%S' 2>/dev/null || date '+%Y-%m-%d-%H-%M-%S'
    fi
}

# Add missing files to database
ADDED_COUNT=0
while IFS= read -r filepath; do
    if [ -f "$filepath" ]; then
        device_id=$(get_device_id "$filepath")
        timestamp=$(get_timestamp "$filepath")
        filesize=$(stat -c%s "$filepath" 2>/dev/null || echo "0")
        
        echo "Adding: $filepath (Device: $device_id, Size: $filesize bytes)"
        
        # Insert into Media table
        mysql -h"$host" -D"$dbname" -u"$user" -p"$password" -e "
            INSERT INTO Media (device_id, filepath, timestamp, filesize, archive, type) 
            VALUES ($device_id, '$filepath', '$timestamp', $filesize, 0, 'video')
        " 2>/dev/null
        
        if [ $? -eq 0 ]; then
            ADDED_COUNT=$((ADDED_COUNT + 1))
            echo "  ✅ Added successfully"
        else
            echo "  ❌ Failed to add (may already exist)"
        fi
    fi
done < ${MISSING_LIST_FILE}

echo ""
echo "=== Summary ==="
echo "Files in database: $DB_COUNT"
echo "Files in filesystem: $FS_COUNT"
echo "Missing files found: $MISSING_COUNT"
echo "Files added to database: $ADDED_COUNT"

if [ $ADDED_COUNT -gt 0 ]; then
    echo ""
    echo "✅ Successfully added $ADDED_COUNT missing recordings to database!"
    echo "You can now start bc-server and view the recordings in the web interface."
else
    echo ""
    echo "⚠️  No files were added. They may already exist in the database."
fi

cleanup_files
echo ""
echo "Script completed." 