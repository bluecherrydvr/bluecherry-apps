#!/bin/bash
set -e
set -x

if ! [[ -d $KERN_SRC ]]
then
	echo "KERN_SRC not set, or doesn't exist" >&2
	exit 1
fi

. /etc/*-release
DEBARCH=`dpkg --print-architecture`
SCRIPTS_DIR=`dirname $0`
DISTRO=`$SCRIPTS_DIR/build_helper/get_distro_release_name.sh`
BASE_DIR=`readlink -f $SCRIPTS_DIR/..`

`dirname $0`/install_prereqs.sh
sudo apt-get install -y -V bc libncurses-dev

cd $KERN_SRC
git clean -dxf
git submodule update --recursive --init
git submodule foreach --recursive "git clean -dxf"
GIT_VERSION=`git describe --dirty --always --long`
CFG_FILE=`ls -t /boot/config* | head -n1`
cat $CFG_FILE <(echo -e "CONFIG_VIDEO_TW5864=m\nCONFIG_VIDEO_SOLO6X10=m\nCONFIG_VIDEO_ADV_DEBUG=y\n") > .config
unset ARCH
make olddefconfig
export DEBEMAIL="maintainers@bluecherrydvr.com"
export DEBFULLNAME="Bluecherry Maintainers"
make -j `getconf _NPROCESSORS_ONLN` deb-pkg LOCALVERSION="-bc-tw5864-$DISTRO-$DEBARCH"

# Rebuild linux-image package properly, with our postinst
PKG_GUTS_DIR="`pwd`/debian/tmp"
cp -r $BASE_DIR/misc/kernel_pkg/* $PKG_GUTS_DIR/ # TODO Make up some version-defined prefix to avoid packages collision?
dpkg --build "$PKG_GUTS_DIR" ..

DST_DIR=$BASE_DIR/kernel_releases/"$GIT_VERSION"/$DISTRO/$DEBARCH/
mkdir -p "$DST_DIR"
mv ../*.deb "$DST_DIR"
