#!/bin/bash

# Assuming that you've built all of the gstreamer dependencies in macports as expected,
# this script will copy them to their local location, and modify them as necessary for
# the bundled distribution.

rm -r lib include plugins
mkdir lib
mkdir plugins
mkdir include

# gstreamer, gst-plugins-base
cp /opt/local/lib/{libgstreamer,libgstbase,libgstapp,libgstvideo,libgstinterfaces}-0.10.0.dylib lib/
cp -R /opt/local/include/gstreamer-0.10/ include/

# more gstreamer and gst-plugins-base, as required by our plugins
cp /opt/local/lib/libgst{riff,audio,tag,pbutils}-0.10.0.dylib lib/

# gstreamer plugins (gst-plugins-base, gst-plugins-good, gst-ffmpeg)
cp /opt/local/lib/gstreamer-0.10/libgst{coreelements,app,decodebin,matroska,osxaudio,osxvideosink,videoscale,ffmpegcolorspace,ffmpeg,typefindfunctions}.so plugins/

# glib
cp /opt/local/lib/{libglib,libgobject,libgthread,libgmodule,libgio}-2.0.0.dylib lib/
cp -R /opt/local/include/glib-2.0/ include/
cp /opt/local/lib/glib-2.0/include/glibconfig.h include/

# other libraries
cp /opt/local/lib/{libiconv.2.dylib,libintl.8.dylib,liborc-0.4.0.dylib,libz.1.dylib,libbz2.1.0.dylib} lib/

# Replace library paths
../../mac/replacepath.py --old /opt/local/lib/ --new @loader_path/ --dir lib -R
../../mac/replacepath.py --old /opt/local/lib/ --new @loader_path/ --dir plugins -R
