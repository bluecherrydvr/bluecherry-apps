#!/bin/bash
set -e

if [[ -z $ARCH ]] || [[ -z $DIST ]]
then
	echo "Please set DIST and ARCH env vars" >&2
	exit 1
fi

if [[ -z "$CHROOT_BASE_LOC" ]]
then
	CHROOT_BASE_LOC=..
fi

WORKCOPY_LOC=.
LOCATION="$CHROOT_BASE_LOC/${DIST}_${ARCH}"
MOUNT_LOC="build"
BUILD_LOC="$LOCATION/$MOUNT_LOC"

if ! [[ -e "$LOCATION" ]]
then
	"$WORKCOPY_LOC/scripts/build_chroot.sh" $DIST $ARCH "$LOCATION"
fi

function cleanup {
	for x in dev proc sys
	do
		sudo umount -l $LOCATION/$x
	done
	sudo umount -l "$BUILD_LOC"
}

trap cleanup INT TERM QUIT

mkdir -p "$BUILD_LOC"
sudo mount --rbind "$WORKCOPY_LOC" "$BUILD_LOC"
for x in dev proc sys
do
	sudo mount --rbind {/,$LOCATION/}$x
done

sudo chroot "$LOCATION" /bin/bash -e -c "cd /$MOUNT_LOC && $COMMAND"

cleanup
