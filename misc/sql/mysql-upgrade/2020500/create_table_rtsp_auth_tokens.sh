#!/bin/bash

echo '
CREATE TABLE `RtspAuthTokens` (
  `user_id` int(11) NOT NULL,
  `token` varchar(100) NOT NULL,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE KEY `uniq_session` (`user_id`,`token`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"
