#!/bin/bash

echo '
ALTER TABLE EventsCam DROP CONSTRAINT `EventsCam_ibfk_3`;
ALTER TABLE EventsCam ADD  CONSTRAINT `EventsCam_ibfk_3` FOREIGN KEY (`device_id`) REFERENCES `Devices` (`id`) ON UPDATE CASCADE ON DELETE CASCADE;

ALTER TABLE Media     DROP CONSTRAINT `Media_ibfk_1`;
ALTER TABLE Media     ADD  CONSTRAINT `Media_ibfk_1`     FOREIGN KEY (`device_id`) REFERENCES `Devices` (`id`) ON UPDATE CASCADE ON DELETE CASCADE;
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
