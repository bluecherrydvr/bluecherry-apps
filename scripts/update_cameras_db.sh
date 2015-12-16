#!/bin/bash
set -e

echo | php misc/sql/import-cambase-data.php misc/sql/camera_info_sqlite_data.sql --create-tables
rm misc/sql/cameras_shipped.db
sqlite3 misc/sql/cameras_shipped.db < misc/sql/camera_info_sqlite_data.sql
