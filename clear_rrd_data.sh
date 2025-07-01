#!/bin/bash

# Script to clear old RRD data and restart data collection
# This will remove the old 97% memory data and start fresh

echo "Clearing old RRD data..."

# Backup the current RRD file
sudo cp /var/lib/bluecherry/monitor.rrd /var/lib/bluecherry/monitor.rrd.backup.$(date +%Y%m%d_%H%M%S)

# Remove the old RRD file
sudo rm /var/lib/bluecherry/monitor.rrd

# Restart bc-server to recreate RRD file with new calculations
echo "Restarting bc-server to recreate RRD file..."
sudo systemctl restart bc-server

echo "RRD data cleared. New data collection will start with corrected calculations."
echo "Check the RRD file in a few minutes to see the new data." 