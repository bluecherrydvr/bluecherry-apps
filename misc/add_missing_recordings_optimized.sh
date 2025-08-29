#!/bin/bash

set -e

# Script to scan recording files and add missing ones to the database
# This is useful after VM restores or when recordings exist but aren't in the DB
# OPTIMIZED for large datasets (20TB+) with batch processing

# Command line arguments
RUNNING_OVERRIDE=false
BATCH_SIZE=100
PROGRESS_INTERVAL=50

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --running=override)
            RUNNING_OVERRIDE=true
            shift
            ;;
        --batch-size=*)
            BATCH_SIZE="${1#*=}"
            shift
            ;;
        --progress-interval=*)
            PROGRESS_INTERVAL="${1#*=}"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --running=override    Allow script to run while bc-server is active"
            echo "  --batch-size=N        Set batch size for database inserts (default: 100)"
            echo "  --progress-interval=N Set progress reporting interval (default: 50)"
            echo "  --help, -h            Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                                    # Normal run (warns if bc-server running)"
            echo "  $0 --running=override                 # Run even if bc-server is active"
            echo "  $0 --batch-size=200 --running=override # Larger batches, allow running server"
            echo ""
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

DB_LIST_FILE=/tmp/bluecherry_add_missing.db.list
STORAGES_LIST_FILE=/tmp/bluecherry_add_missing.storages.list
FS_LIST_FILE=/tmp/bluecherry_add_missing.fs.list
MISSING_LIST_FILE=/tmp/bluecherry_add_missing.missing.list

function cleanup_files() {
    rm ${DB_LIST_FILE}* ${STORAGES_LIST_FILE}* ${FS_LIST_FILE}* ${MISSING_LIST_FILE}* &>/dev/null || true
}

cleanup_files

echo "=== Bluecherry Missing Recordings Scanner (OPTIMIZED) ==="
echo "This script will scan for recording files and add missing ones to the database"
echo "Optimized for large datasets with batch processing and progress reporting"
echo ""
echo "Configuration:"
echo "  Batch size: $BATCH_SIZE files per database transaction"
echo "  Progress interval: $PROGRESS_INTERVAL files"
echo "  Running override: $RUNNING_OVERRIDE"
echo ""

# Performance estimation function
estimate_processing_time() {
    local file_count=$1
    local estimated_minutes=$((file_count / 2000))  # Rough estimate: 2000 files per minute
    if [ $estimated_minutes -lt 1 ]; then
        estimated_minutes=1
    fi
    echo $estimated_minutes
}

# Check bc-server status with override support
if ps ax | grep bc-server | grep -v grep &>/dev/null
then
    if [ "$RUNNING_OVERRIDE" = true ]; then
        echo "⚠️  WARNING: bc-server is running, but continuing due to --running=override"
        echo "   This may cause inconsistent results if bc-server is actively recording"
        echo ""
    else
        echo -e "\n\n\tWARNING! bc-server is running. It must be stopped for stable result of this script\n\n"
        echo "To override this check, run: $0 --running=override"
        echo ""
        read -p "Continue anyway? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
else
    echo "✅ bc-server is not running - optimal conditions for database sync"
    echo ""
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

# Check if we got any results
if [ ! -s ${DB_LIST_FILE}.raw ]; then
    echo "No files found in database (this is normal for a fresh install or after VM restore)"
    echo "" > ${DB_LIST_FILE}.resolved
    echo "" > ${DB_LIST_FILE}.sorted
    DB_COUNT=0
    echo "Found $DB_COUNT files already in database"
else
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
fi

echo ""
echo "Step 2: Getting storage paths..."
date

# Get storage paths from database
echo "SELECT path FROM Storage" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password" | tail -n +2 > ${STORAGES_LIST_FILE}.raw
# Add default storage path
echo "/var/lib/bluecherry/recordings" >> ${STORAGES_LIST_FILE}.raw
# Remove trailing slashes
sed 's/\/$//' ${STORAGES_LIST_FILE}.raw > ${STORAGES_LIST_FILE}.patched

# Show storage paths being scanned
echo "Storage paths to scan:"
cat ${STORAGES_LIST_FILE}.patched | while read path; do
    if [ -n "$path" ]; then
        echo "  - $path"
    fi
done
echo ""

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

# Build list of actual files in filesystem with progress
I=1
TOTAL_STORAGE_PATHS=$(wc -l < ${STORAGES_LIST_FILE}.filtered)
while true
do
    STORAGE_PATH=`tail -n +$I ${STORAGES_LIST_FILE}.filtered | tail -n 1`
    if [[ -z "$STORAGE_PATH" ]]; then
        break
    fi
    I=$(( I + 1 ))

    if [ -d "$STORAGE_PATH" ]; then
        echo "Scanning: $STORAGE_PATH ($I/$TOTAL_STORAGE_PATHS)"
        find "$STORAGE_PATH" -type f \( -name "*.mp4" -o -name "*.mkv" -o -name "*.avi" \) 2>/dev/null >> ${FS_LIST_FILE}.raw
    else
        echo "Warning: Storage path does not exist: $STORAGE_PATH"
    fi
done

# Resolve absolute paths for filesystem files
set +e
I=1
TOTAL_FS_FILES=$(wc -l < ${FS_LIST_FILE}.raw)
echo "Resolving paths for $TOTAL_FS_FILES files..."
while true
do
    ENTRY=`tail -n +$I ${FS_LIST_FILE}.raw | tail -n 1`
    if [[ -z "$ENTRY" ]]; then
        break
    fi
    I=$(( I + 1 ))

    # Show progress every 1000 files
    if [ $((I % 1000)) -eq 0 ]; then
        echo "  Progress: $I/$TOTAL_FS_FILES files processed"
    fi

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

# Estimate processing time
ESTIMATED_MINUTES=$(estimate_processing_time $MISSING_COUNT)
echo "Estimated processing time: $ESTIMATED_MINUTES minutes"

echo ""
echo "Step 5: Adding missing files to database (batch processing)..."
date

# Function to extract device ID from file path
get_device_id() {
    local filepath="$1"
    # Extract device ID from path like /path/to/recordings/YYYY/MM/DD/000001/filename.mp4
    local device_id=$(echo "$filepath" | sed -n 's/.*\/\([0-9]\{6\}\)\/.*/\1/p')
    if [ -n "$device_id" ]; then
        # Convert 6-digit device ID to actual device number (remove leading zeros)
        device_id=$(echo "$device_id" | sed 's/^0*//')
        if [ -z "$device_id" ]; then
            device_id="0"
        fi
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

# Add missing files to database with batch processing
ADDED_COUNT=0
BATCH_COUNT=0
CURRENT_BATCH=0

echo "Processing $MISSING_COUNT files in batches of $BATCH_SIZE..."

# Create batch file for SQL
BATCH_SQL_FILE="/tmp/bluecherry_batch_insert.sql"

while IFS= read -r filepath; do
    if [ -f "$filepath" ]; then
        device_id=$(get_device_id "$filepath")
        timestamp=$(get_timestamp "$filepath")
        filesize=$(stat -c%s "$filepath" 2>/dev/null || echo "0")
        
        # Add to batch SQL
        if [ $BATCH_COUNT -eq 0 ]; then
            echo "INSERT INTO Media (device_id, filepath, start, end, size, archive) VALUES" > "$BATCH_SQL_FILE"
        else
            echo "," >> "$BATCH_SQL_FILE"
        fi
        
        echo "($device_id, '$filepath', UNIX_TIMESTAMP('$timestamp'), UNIX_TIMESTAMP('$timestamp') + 300, $filesize, 0)" >> "$BATCH_SQL_FILE"
        
        BATCH_COUNT=$((BATCH_COUNT + 1))
        ADDED_COUNT=$((ADDED_COUNT + 1))
        
        # Show progress every PROGRESS_INTERVAL files
        if [ $((ADDED_COUNT % PROGRESS_INTERVAL)) -eq 0 ]; then
            echo "Progress: $ADDED_COUNT/$MISSING_COUNT files processed"
        fi
        
        # Execute batch when full
        if [ $BATCH_COUNT -ge $BATCH_SIZE ]; then
            echo ";" >> "$BATCH_SQL_FILE"
            
            echo "Executing batch $((CURRENT_BATCH + 1)) with $BATCH_COUNT files..."
            
            mysql -h"$host" -D"$dbname" -u"$user" -p"$password" < "$BATCH_SQL_FILE" 2>/dev/null
            
            if [ $? -eq 0 ]; then
                echo "  ✅ Batch $((CURRENT_BATCH + 1)) completed successfully"
            else
                echo "  ❌ Batch $((CURRENT_BATCH + 1)) failed"
            fi
            
            CURRENT_BATCH=$((CURRENT_BATCH + 1))
            BATCH_COUNT=0
        fi
    fi
done < ${MISSING_LIST_FILE}

# Execute final batch if any remaining
if [ $BATCH_COUNT -gt 0 ]; then
    echo ";" >> "$BATCH_SQL_FILE"
    
    echo "Executing final batch with $BATCH_COUNT files..."
    
    mysql -h"$host" -D"$dbname" -u"$user" -p"$password" < "$BATCH_SQL_FILE" 2>/dev/null
    
    if [ $? -eq 0 ]; then
        echo "  ✅ Final batch completed successfully"
    else
        echo "  ❌ Final batch failed"
    fi
fi

# Clean up batch file
rm -f "$BATCH_SQL_FILE"

echo ""
echo "=== Summary ==="
echo "Files in database: $DB_COUNT"
echo "Files in filesystem: $FS_COUNT"
echo "Missing files found: $MISSING_COUNT"
echo "Files added to database: $ADDED_COUNT"
echo "Batches processed: $((CURRENT_BATCH + 1))"

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