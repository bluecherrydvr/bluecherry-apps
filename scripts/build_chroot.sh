#!/bin/bash
set -e

DIST=$1
ARCH=$2

LOCATION="$3"

case $DIST
	precise|trusty)
		DISTRO=ubuntu
		APT_SOURCES_URL="http://archive.ubuntu.com/ubuntu/"
		;;
	wheezy|jessie)
		DISTRO=debian
		APT_SOURCES_URL="http://httpredir.debian.org/debian/"
		;;
	*)
		echo "Unknown DIST" >&2
		exit 1
		;;
esac

sudo apt-get install debootstrap
debootstrap --arch $ARCH --variant minbase $DIST "$LOCATION"

cat <<EOF >> "$LOCATION/etc/apt/sources.list"
deb $APT_SOURCES_URL ${DIST} main restricted universe multiverse 
deb $APT_SOURCES_URL ${DIST}-security main restricted universe multiverse 
deb $APT_SOURCES_URL ${DIST}-updates main restricted universe multiverse 
deb $APT_SOURCES_URL ${DIST}-backports main restricted universe multiverse
EOF

sudo chroot "$LOCATION" apt-get update
sudo chroot "$LOCATION" `dirname $0`/install_prereqs.sh

