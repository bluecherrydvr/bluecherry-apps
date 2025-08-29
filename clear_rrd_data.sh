#!/bin/bash

# Script to clear old RRD data and restart data collection
# This will remove the old data and start fresh with network interface support

echo "Clearing old RRD data..."

# Backup the current RRD file
sudo cp /var/lib/bluecherry/monitor.rrd /var/lib/bluecherry/monitor.rrd.backup.$(date +%Y%m%d_%H%M%S)

# Remove the old RRD file
sudo rm /var/lib/bluecherry/monitor.rrd

# Restart bc-server to recreate RRD file with network interfaces
echo "Restarting bc-server to recreate RRD file with network interface support..."
sudo systemctl restart bc-server

echo "RRD data cleared. New data collection will start with network interface monitoring."
echo "Check the RRD file in a few minutes to see the new data including network interfaces."
echo "Network interfaces will be automatically detected and added to the monitoring." 