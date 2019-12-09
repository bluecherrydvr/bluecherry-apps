#!/bin/bash
# Usage: "source $0" or ". $0"
eval $(sed '/\(userhost\|host\|dbname\|user\|password\)/!d;s/ *= */=/'";s/\"/'/g" /etc/bluecherry.conf)
host=${host:-localhost}
