#!/bin/bash

# Given a path to the application bundle, we need to:
#   1. Run macdeployqt to get the Qt frameworks
#   2. Tweak the results from macdeployqt
#   3. Copy the gstreamer libraries into the bundle
#   4. Alter the linker paths on the executable to the gstreamer libraries

if [ -z $2 ]; then
	echo "Usage: ./deploy.sh xxx.app /path/to/macdeployqt"
	exit 1;
fi;

if [ ! -e gstreamer-bin ]; then
	echo "This file must be run from the main application directory ('client')";
	exit 1;
fi;

EXENAME=`basename "$1" | cut -d '.' -f 1`
echo $EXENAME

BINPATH=`dirname "$0"`

if [ ! -d $1/Contents/Frameworks ]; then
	mkdir $1/Contents/Frameworks
fi

cp gstreamer-bin/mac/lib/* $1/Contents/Frameworks/

$BINPATH/replacepath.py --old @loader_path/ --new @executable_path/../Frameworks/ --file $1/Contents/MacOS/$EXENAME

#$2 $1
