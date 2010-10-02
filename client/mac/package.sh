#!/bin/bash

if [ -z $1 ]; then
	echo "Usage: mac/package.sh path/to/build"
	exit
fi

if [ ! -e $1/bluecherry-client.app ]; then
	echo "$1/bluecherry-client.app does not exist"
	exit
fi

hdiutil create -srcfolder $1/bluecherry-client.app bluecherry-client.dmg
hdiutil internet-enable -yes bluecherry-client.dmg
