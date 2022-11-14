## Building from source

[![Join the chat at https://gitter.im/bluecherrydvr/dev-server](https://badges.gitter.im/bluecherrydvr/dev-server.svg)](https://gitter.im/bluecherrydvr/dev-server?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)[![Join our Slack!](https://img.shields.io/static/v1?message=join%20chat&color=9cf&logo=slack&label=slack)](https://join.slack.com/t/bluecherry-public/shared_invite/zt-rhxb117k-m5sCsp_FF0l8cToB56HeYA) 

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

