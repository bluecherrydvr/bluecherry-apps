#!/bin/bash

echo '
ALTER TABLE Devices
  ADD `connection_retry_seconds` smallint(5) DEFAULT '10' AFTER `hls_window_size`;
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0