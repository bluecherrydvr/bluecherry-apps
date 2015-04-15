#!/bin/bash
set -e
cd /sys/class/net
for x in *
do
	if [[ `cat $x/address | sed 's/[0:]//g' | wc -w` != 0 ]]
	then
		echo $x
	fi
done
