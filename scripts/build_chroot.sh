#!/bin/bash
set -e

DIST=$1
ARCH=$2

LOCATION="$3"

sudo apt-get install debootstrap
debootstrap --arch $ARCH --variant minbase $DIST "$LOCATION"

case $DIST in
	precise|trusty)
		DISTRO=ubuntu
		APT_SOURCES_URL="http://archive.ubuntu.com/ubuntu/"
		cat <<EOF >> "$LOCATION/etc/apt/sources.list"
deb $APT_SOURCES_URL ${DIST} main restricted universe multiverse
deb $APT_SOURCES_URL ${DIST}-security main restricted universe multiverse
deb $APT_SOURCES_URL ${DIST}-updates main restricted universe multiverse
deb $APT_SOURCES_URL ${DIST}-backports main restricted universe multiverse
EOF
		;;
	wheezy|jessie)
		DISTRO=debian
		cat <<EOF >> "$LOCATION/etc/apt/sources.list"
deb http://ftp.us.debian.org/debian $DIST main
deb http://ftp.debian.org/debian/ ${DIST}-updates main
deb http://security.debian.org/ ${DIST}/updates main
EOF
		;;
	*)
		echo "Unknown DIST" >&2
		exit 1
		;;
esac

cat `dirname $0`/install_prereqs.sh | sudo chroot "$LOCATION" /bin/bash -e
