#!/bin/bash

echo '
ALTER TABLE Devices
  ADD `max_motion_area` smallint(5) DEFAULT '90' AFTER `min_motion_area`;
ALTER TABLE Devices
  ADD `min_motion_frames` smallint(5) DEFAULT '15' AFTER `onvif_events_enabled`;
ALTER TABLE Devices
  ADD `max_motion_frames` smallint(5) DEFAULT '45' AFTER `min_motion_frames`;
ALTER TABLE Devices
  ADD `motion_blend_ratio` smallint(5) DEFAULT '15' AFTER `max_motion_frames`;
ALTER TABLE Devices
  ADD `motion_debug` tinyint(1) DEFAULT '0' AFTER `motion_blend_ratio`;
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
