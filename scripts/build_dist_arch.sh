#!/bin/bash
set -e

if [[ -z $ARCH ]] || [[ -z $DIST ]]
then
	echo "Please set DIST and ARCH env vars" >&2
	exti 1
fi

if [[ -z "$CHROOT_BASE_LOC" ]]
then
	CHROOT_BASE_LOC=..
fi

WORKCOPY_LOC=.
LOCATION="$CHROOT_BASE_LOC/${DIST}_${ARCH}"
MOUNT_LOC="build"
BUILD_LOC="$LOCATION/$MOUNT_LOC"
RELEASES_LOC="$WORKCOPY_LOC/../releases"

if ! [[ -e "$LOCATION" ]]
then
	"$WORKCOPY_LOC/scripts/build_chroot.sh" $DIST $ARCH "$LOCATION"
fi

mkdir -p "$BUILD_LOC"
sudo mount --bind "$WORKCOPY_LOC" "$BUILD_LOC"

sudo chroot "$LOCATION" /bin/bash -e -c "cd /$MOUNT_LOC && /$MOUNT_LOC/scripts/build_pkg_native.sh"

mkdir -p "$RELEASES_LOC/$DIST/"
cp -v "$LOCATION"/*.deb "$RELEASES_LOC/$DIST/"

sudo umount "$BUILD_LOC"  # TODO bash trap for this
