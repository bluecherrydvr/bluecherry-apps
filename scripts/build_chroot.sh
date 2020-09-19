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
	precise|trusty|wily|xenial|bionic|focal)
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
	wheezy|jessie|stretch|buster)
		DISTRO=debian
		MIRROR_URL="http://deb.debian.org/debian/"
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
	centos_*)
		DISTRO=centos
		;;
	*)
		echo "Unknown DIST" >&2
		exit 1
		;;
esac
if [[ "$DIST" == centos* ]]
then
	apt-get -y install yum
	rpm -i http://pubmirrors.dal.corespace.com/centos/7/os/x86_64/Packages/centos-release-7-2.1511.el7.centos.2.10.x86_64.rpm || true
	yum -y install yum --installroot=`pwd`/"$LOCATION" --releasever=7
	cp /etc/resolv.conf "$LOCATION"/etc/resolv.conf
	mount --rbind /proc "$LOCATION/proc"
	mount --rbind /sys  "$LOCATION/sys"
	mount --rbind /dev  "$LOCATION/dev"
	chroot "$LOCATION" yum -y group install Base --releasever=7 || true
	RES=$?
	umount -l "$LOCATION/dev"
	umount -l "$LOCATION/sys"
	umount -l "$LOCATION/proc"
	if [[ $RES != 0 ]]
	then
		exit 1
	fi
elif [[ "$ARCH" == arm* ]]
then
	mkdir -p "$LOCATION/"{,p1,p2}
	wget -O - http://odroid.in/ubuntu_14.04lts/ubuntu-14.04lts-server-odroid-xu3-20150725.img.xz \
		| xzdec > $LOCATION/img
	LOOPDEV=`losetup -P --show -f $LOCATION/img`
	mount ${LOOPDEV}p1 $LOCATION/p1
	mount ${LOOPDEV}p2 $LOCATION/p2
	cp -a $LOCATION/p2/* $LOCATION
	cp -a $LOCATION/p1/* $LOCATION/boot
	umount -l ${LOOPDEV}p1
	umount -l ${LOOPDEV}p2
	losetup -d $LOOPDEV
	echo '91.189.88.151 ports.ubuntu.com' >> $LOCATION/etc/hosts
else
	debootstrap --arch $ARCH --variant buildd $ADDITIONAL_ARGS $DIST "$LOCATION" $MIRROR_URL
	cp ./sources.list $LOCATION/etc/apt/sources.list
fi

mkdir -p $LOCATION/build
mount --rbind `dirname $0`/../ $LOCATION/build
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

function cleanup() {
	for x in dev proc sys
	do
		sudo umount -l $LOCATION/$x
	done
	sudo umount -l $LOCATION/build
}
trap cleanup INT TERM QUIT EXIT

if [[ "$DIST" == centos* ]]
then
	sudo chroot "$LOCATION" /bin/bash -e /build/scripts/install_prereqs_RPM.sh
elif [[ $ARCH == arm* ]]
then
	sudo chroot "$LOCATION" qemu-arm-static /bin/bash -e /build/scripts/install_prereqs.sh
else
	sudo chroot "$LOCATION" /bin/bash -e /build/scripts/install_prereqs.sh
fi
