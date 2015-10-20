#!/bin/bash
set -e
set -x

DIST=$1
ARCH=$2

LOCATION="$3"
if mount | grep `readlink -f $LOCATION`
then
	echo "Requested to build chroot, but something is mounted over LOCATION=$LOCATION" >&2
	exit 1
fi
rm -rf $LOCATION || true

case $DIST in
	precise|trusty|wily)
		DISTRO=ubuntu
		APT_SOURCES_URL="http://archive.ubuntu.com/ubuntu/"
		MIRROR_URL="http://mirrors.kernel.org/ubuntu/"
		cat <<EOF > "./sources.list"
deb $APT_SOURCES_URL ${DIST} main restricted universe multiverse
deb $APT_SOURCES_URL ${DIST}-security main restricted universe multiverse
deb $APT_SOURCES_URL ${DIST}-updates main restricted universe multiverse
deb $APT_SOURCES_URL ${DIST}-backports main restricted universe multiverse
EOF
		;;
	wheezy|jessie)
		DISTRO=debian
		MIRROR_URL="http://mirrors.kernel.org/debian/"
		cat <<EOF > "./sources.list"
deb http://ftp.us.debian.org/debian $DIST main
deb http://ftp.debian.org/debian/ ${DIST}-updates main
deb http://security.debian.org/ ${DIST}/updates main
EOF
		;;
	sid)
		DISTRO=debian
		MIRROR_URL="http://mirrors.kernel.org/debian/"
		cat <<EOF > "./sources.list"
deb http://ftp.us.debian.org/debian $DIST main
EOF
		;;
	*)
		echo "Unknown DIST" >&2
		exit 1
		;;
esac

debootstrap --arch $ARCH --variant minbase $ADDITIONAL_ARGS $DIST "$LOCATION" $MIRROR_URL

cp ./sources.list $LOCATION/etc/apt/sources.list

mkdir -p $LOCATION/build
mount --rbind `dirname $0`/../ $LOCATION/build
for x in dev proc sys
do
	mkdir -p $LOCATION/$x
	sudo mount --rbind {/,$LOCATION/}$x
done

function cleanup() {
	for x in dev proc sys
	do
		sudo umount -l $LOCATION/$x
	done
	sudo umount -l $LOCATION/build
}
trap cleanup INT TERM QUIT

sudo chroot "$LOCATION" /bin/bash -e /build/scripts/install_prereqs.sh

cleanup
