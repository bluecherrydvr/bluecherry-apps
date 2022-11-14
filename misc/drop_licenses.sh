#!/bin/bash
dbname="$1"
user="$2"
password="$3"
host="${4:-localhost}"

echo "

Used V2 Licenses:"
echo 'SELECT * FROM Licenses;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"
echo "

"

echo 'DROP TABLE IF EXISTS Licenses;' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

echo '
CREATE TABLE `Licenses` (
  `license` varchar(64) NOT NULL,
  `authorization` varchar(64) NOT NULL,
  `added` int(11) NOT NULL,
  PRIMARY KEY (`license`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"