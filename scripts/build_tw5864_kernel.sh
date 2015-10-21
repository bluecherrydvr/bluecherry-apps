#!/bin/bash
set -e
set -x

if ! [[ -d $KERN_SRC ]]
then
	echo "KERN_SRC not set, or doesn't exist" >&2
	exit 1
fi

. /etc/*-release
if [[ -e /etc/lsb-release ]]
then
	# Ubuntu
	DISTRO=$DISTRIB_CODENAME
else
	# Debian
	DISTRO=`echo $VERSION | sed -e 's/^.*[(]//' -e 's/[)]//'`
fi
ARCH=`dpkg --print-architecture`
BASE_DIR=`readlink -f .`

`dirname $0`/install_prereqs.sh
sudo apt-get install -y -V bc libncurses-dev

cd $KERN_SRC
git clean -dxf
git submodule foreach --recursive "git clean -dxf"
GIT_VERSION=`git describe --dirty --always --long`
CFG_FILE=`ls -t /boot/config* | head -n1`
cat $CFG_FILE <(echo "CONFIG_VIDEO_TW5864=m") > .config
make olddefconfig
export DEBEMAIL="maintainers@bluecherrydvr.com"
export DEBFULLNAME="Bluecherry Maintainers"
make -j `getconf _NPROCESSORS_ONLN` deb-pkg LOCALVERSION="-bc-tw5864-$DISTRO-$ARCH"
DST_DIR=$BASE_DIR/kernel_pkg/"$GIT_VERSION"/$DISTRO/$ARCH/
mkdir -p "$DST_DIR"
mv ../*.deb "$DST_DIR"
