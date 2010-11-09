#!/bin/bash

if [ -z $1 ]; then
	echo "Usage: mac/package.sh path/to/build"
	exit
fi

if [ ! -e $1/BluecherryClient.app ]; then
	echo "$1/BluecherryClient.app does not exist"
	exit
fi

rm -r "$1/Bluecherry Client.app"
cp -R "$1/BluecherryClient.app" "$1/Bluecherry Client.app"

hdiutil create -srcfolder "$1/Bluecherry Client.app" BluecherryClient.dmg
hdiutil internet-enable -yes BluecherryClient.dmg

rm -r "$1/Bluecherry Client.app"
