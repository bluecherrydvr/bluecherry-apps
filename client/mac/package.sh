#!/bin/bash

if [ -z $1 ]; then
	echo "Usage: mac/package.sh path/to/build"
	exit
fi

if [ ! -e $1/BluecherryClient.app ]; then
	echo "$1/BluecherryClient.app does not exist"
	exit
fi

hdiutil create -srcfolder $1/BluecherryClient.app BluecherryClient.dmg
hdiutil internet-enable -yes BluecherryClient.dmg
