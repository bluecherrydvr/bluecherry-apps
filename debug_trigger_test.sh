#!/bin/bash
# Debug script for testing vehicle triggers

echo "Restarting Bluecherry service..."
sudo service bluecherry restart
sleep 5

echo "Setting debug levels to maximum..."
mysql -ubluecherry -p123 bluecherry -e "UPDATE Devices SET debug_level = 10 WHERE id IN (43, 47); UPDATE GlobalSettings SET value = 10 WHERE parameter = 'debug' OR parameter = 'debug_level';"

echo "Sending vehicle trigger for device 43..."
sudo echo -n "43|vehicle|start" | sudo socat - UNIX-CONNECT:/tmp/bluecherry_trigger
sleep 2

echo "Checking logs for trigger events..."
sudo tail -n 100 /var/log/bluecherry.log | grep -i "trigger\|vehicle\|recording"

echo "Checking database for recent events..."
mysql -ubluecherry -p123 bluecherry -e "SELECT id, device_id, level_id, type_id, time, FROM_UNIXTIME(time) as timestamp FROM EventsCam WHERE device_id = 43 ORDER BY id DESC LIMIT 5;"

echo "Debug test completed." 