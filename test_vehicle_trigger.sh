#!/bin/bash
# Script to test vehicle trigger and verify it's working

# Stop the service first to ensure a clean start
echo "Stopping Bluecherry service..."
sudo service bluecherry stop
sleep 2

# Start the service
echo "Starting Bluecherry service..."
sudo service bluecherry start
sleep 10

# Send a vehicle trigger for device 43
echo "Sending vehicle trigger for device 43..."
sudo echo -n "43|vehicle|start" | sudo socat - UNIX-CONNECT:/tmp/bluecherry_trigger
sleep 5

# Check the logs for confirmation
echo "Checking logs for trigger confirmation..."
sudo tail -n 20 /var/log/bluecherry.log | grep -i "vehicle\|trigger"

# Check the database to see if vehicle events are recorded correctly
echo "Checking database for vehicle events..."
mysql -ubluecherry -p123 bluecherry -e "SELECT id, device_id, level_id, type_id, time, FROM_UNIXTIME(time) as timestamp FROM EventsCam WHERE device_id = 43 ORDER BY id DESC LIMIT 5;"

echo "Test completed!" 