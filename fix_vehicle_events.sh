#!/bin/bash
# Script to fix vehicle event type issues in bluecherry database

# First, check if a lowercase "vehicle" entry exists and try to remove it
mysql -ubluecherry -p123 bluecherry -e "DELETE FROM EventTypesCam WHERE id='vehicle';" 2>/dev/null

# Ensure the "Vehicle" (uppercase V) entry exists
mysql -ubluecherry -p123 bluecherry -e "INSERT IGNORE INTO EventTypesCam (id) VALUES ('Vehicle');"

# Update any events that might have the wrong type to the correct one
mysql -ubluecherry -p123 bluecherry -e "UPDATE EventsCam SET type_id='Vehicle' WHERE type_id='vehicle';"

# List the current event types to verify
mysql -ubluecherry -p123 bluecherry -e "SELECT id FROM EventTypesCam ORDER BY id;"

echo "Vehicle event type fixes applied!" 