#!/bin/bash

echo 'ALTER TABLE Devices MODIFY `video_quality` smallint(6) DEFAULT 100' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"
echo 'UPDATE Devices SET video_quality = 100' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
