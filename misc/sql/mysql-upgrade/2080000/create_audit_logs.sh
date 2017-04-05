#!/bin/bash

echo '
CREATE TABLE `AuditEventTypes` (
  `id` int(11) NOT NULL,
  `description` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
' | mysql -D"$dbname" -u"$user" -p"$password"

echo '
CREATE TABLE `AuditLogs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `dt` datetime NOT NULL,
  `event_type_id` int(11) DEFAULT NULL,
  `device_id` int(11) DEFAULT NULL,
  `user_id` int(11) DEFAULT NULL,
  `ip` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`id`),
  CONSTRAINT `AuditLogs_ibfk_1` FOREIGN KEY (`event_type_id`) REFERENCES `AuditEventTypes` (`id`),
  CONSTRAINT `AuditLogs_ibfk_2` FOREIGN KEY (`device_id`) REFERENCES `Devices` (`id`),
  CONSTRAINT `AuditLogs_ibfk_3` FOREIGN KEY (`user_id`) REFERENCES `Users` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
' | mysql -D"$dbname" -u"$user" -p"$password"

echo '
INSERT INTO AuditEventTypes VALUES (1, "Failed login attempt");
INSERT INTO AuditEventTypes VALUES (2, "User logged in");
INSERT INTO AuditEventTypes VALUES (3, "User logged out");
INSERT INTO AuditEventTypes VALUES (4, "Device configuration changed");
INSERT INTO AuditEventTypes VALUES (5, "New device added");
INSERT INTO AuditEventTypes VALUES (6, "Device disabled");
INSERT INTO AuditEventTypes VALUES (7, "Device deleted");
INSERT INTO AuditEventTypes VALUES (8, "Password changed for user");
INSERT INTO AuditEventTypes VALUES (9, "General settings changed");
INSERT INTO AuditEventTypes VALUES (10, "Global schedule changed");
INSERT INTO AuditEventTypes VALUES (11, "Email notification settings changed");
' | mysql -D"$dbname" -u"$user" -p"$password"

