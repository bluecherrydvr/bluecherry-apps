#!/bin/bash
set -e
git clone git://git.videolan.org/ffmpeg.git
cd ffmpeg
git checkout n2.2.2  # Due to http://trac.ffmpeg.org/ticket/3693
echo "2.2.2" > $BASEDIR/ffmpeg.version
