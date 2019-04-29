## Building from source

Currently building from source requires running pre-configured scripts to generate some of the required files for a build.  In the past we used chroots for build environments, however it is possible to build for your specific distributions using the following command:

In the root of bluecherry-apps run the following command.

``scripts/build_pkg_native.sh``

Example:

``root@buildtest:~/git/bluecherry-apps# scripts/build_pkg_native.sh``
 
**Please do not use 'dh' specific commands, the scripts generate several files (version.h for example) and the Debian control files for you.**



Dependencies (server):
 * mysql (debian: libmysqlclient-dev)
 * libbsd (debian: libbsd-dev)
 * php (debian: php5-dev)
 * udev (debian: libudev-dev)

Dependencies (libav):
 * yasm
 * alsa-lib (debian: libasound2-dev)

Bundled libraries:
 * libconfig
 * libav

--------

