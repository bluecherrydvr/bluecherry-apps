#!/bin/bash
# Usage: "source $0" or ". $0"
eval $(sed '/\(host\|dbname\|user\|password\)/!d;s/ *= */=/'";s/\"/'/g" /etc/bluecherry.conf)
