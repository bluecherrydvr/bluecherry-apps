#!/bin/bash

echo 'ALTER TABLE AuditLogs DROP FOREIGN KEY `AuditLogs_ibfk_1`; ALTER TABLE AuditLogs DROP FOREIGN KEY `AuditLogs_ibfk_2`; ALTER TABLE AuditLogs DROP FOREIGN KEY `AuditLogs_ibfk_3`;ALTER TABLE AuditLogs ADD CONSTRAINT `AuditLogs_ibfk_1` FOREIGN KEY (`event_type_id`) REFERENCES `AuditEventTypes` (`id`) ON DELETE CASCADE; ALTER TABLE AuditLogs ADD CONSTRAINT `AuditLogs_ibfk_2` FOREIGN KEY (`device_id`) REFERENCES `Devices` (`id`) ON DELETE CASCADE; ALTER TABLE AuditLogs ADD CONSTRAINT `AuditLogs_ibfk_3` FOREIGN KEY (`user_id`) REFERENCES `Users` (`id`) ON DELETE CASCADE;' | mysql -D"$dbname" -u"$user" -p"$password"

exit 0

