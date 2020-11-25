#!/bin/bash

echo '
CREATE TABLE `OnvifEvents` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `device_id` int(11) NOT NULL,
  `event_time` datetime NOT NULL,
  `onvif_topic` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `OnvifEvents_ibfk_1` (`device_id`),
  CONSTRAINT `OnvifEvents_ibfk_1` FOREIGN KEY (`device_id`) REFERENCES `Devices` (`id`) ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"
