#!/bin/bash
set -e
set -x

if [[ -z $ARCH ]] || [[ -z $DIST ]]
then
	echo "Please set DIST and ARCH env vars" >&2
	exit 1
fi

if [[ -z "$CHROOT_BASE_LOC" ]]
then
	CHROOT_BASE_LOC=../chroots/
fi

WORKCOPY_LOC=.
LOCATION="$CHROOT_BASE_LOC/${DIST}_${ARCH}"
MOUNT_LOC="build"
BUILD_LOC="$LOCATION/$MOUNT_LOC"

if ! [[ -e "${LOCATION}.setup" ]]
then
	mkdir -p $CHROOT_BASE_LOC
	"$WORKCOPY_LOC/scripts/build_chroot.sh" $DIST $ARCH "$LOCATION"
	touch "${LOCATION}.setup"
fi

function cleanup {
	set +e
	for x in dev proc sys
	do
		sudo umount -l $LOCATION/$x
	done
	sudo umount -l "$BUILD_LOC"
}

trap cleanup INT TERM QUIT EXIT

mkdir -p "$BUILD_LOC"
sudo mount --rbind "$WORKCOPY_LOC" "$BUILD_LOC"
for x in dev proc sys
do
	mkdir -p $LOCATION/$x
	sudo mount --rbind {/,$LOCATION/}$x
done

if [[ -e /sys/fs/selinux ]]
then
	mount --rbind /sys/fs/selinux $LOCATION/sys/fs/selinux
	mount --bind -o remount,ro $LOCATION/sys/fs/selinux
fi

case $ARCH in
	arm*)
		sudo chroot "$LOCATION" qemu-arm-static /bin/bash -e -c "cd /$MOUNT_LOC && $COMMAND"
		;;
	*)
		sudo chroot "$LOCATION" /bin/bash -e -c "cd /$MOUNT_LOC && $COMMAND"
		;;
esac
