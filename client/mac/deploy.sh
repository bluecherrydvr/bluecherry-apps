#!/bin/bash

if [ -z $1 ]; then
	echo "Usage: ./deploy.sh xxx.app [/path/to/macdeployqt]"
	exit 1;
fi;

if [ ! -e gstreamer-bin ]; then
	echo "This file must be run from the main application directory ('client')";
	exit 1;
fi;

EXENAME=`basename "$1" | cut -d '.' -f 1`

BINPATH=`dirname "$0"`

if [ ! -d $1/Contents/Frameworks ]; then
	mkdir $1/Contents/Frameworks
fi

echo "Copying GStreamer libraries..."
cp gstreamer-bin/mac/lib/* $1/Contents/Frameworks/

echo "Copying GStreamer plugins..."
mkdir -p $1/Contents/PlugIns/gstreamer
cp gstreamer-bin/mac/plugins/* $1/Contents/PlugIns/gstreamer/

echo "Copying Breakpad framework..."
rm -r $1/Contents/Frameworks/Breakpad.framework
cp -R breakpad-bin/mac/Breakpad.framework $1/Contents/Frameworks/

echo "Replacing library paths..."
$BINPATH/replacepath.py --old @loader_path/ --new @executable_path/../Frameworks/ --file $1/Contents/MacOS/$EXENAME
$BINPATH/replacepath.py --old @loader_path/ --new @executable_path/../Frameworks/ --dir $1/Contents/PlugIns/gstreamer/

if [ ! -z $2 ]; then
	echo "Running macdeployqt..."
	$2 $1
fi

echo "Running lipo..."
for I in $1/Contents/Frameworks/*.dylib; do
	FILE=$I;
        if [ `file $FILE | grep -c 'for architecture'` == "0" ]; then
		echo "    Skipping $FILE (already thin)"
        else
		lipo -thin i386 -output $FILE $FILE
	fi
done

for I in `find $1/Contents/Frameworks/*.framework/Versions -type f -maxdepth 2`; do
    if [ `file $I | grep -c 'for architecture'` == "0" ]; then
        echo "    Skipping $I (already thin)"
    else
        lipo -thin i386 -output $I $I
    fi
done
