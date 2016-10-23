#!/bin/bash
# Suppress services launching
set -e
set -x

for x in /etc/*-release
do
	source $x
done

# Ubuntu 12 Precise lacks /etc/os-release, thus its ID is empty
if [[ $ID == "" ]] && [[ $DISTRIB_ID == "Ubuntu" ]]
then
	ID="ubuntu"
fi

case $ID in
	debian)
		SUFFIX=`dpkg --print-architecture | sed s/i386/686-pae/`
		ADDITIONAL_PKGS="linux-image-$SUFFIX linux-headers-$SUFFIX"
		;;
	ubuntu)
		ADDITIONAL_PKGS="linux-image-generic linux-headers-generic"
		;;
	*)
		exit 1
		;;
esac

case "`uname --hardware-platform`" in
	arm*)
		ADDITIONAL_PKGS=
		;;
esac

export DEBIAN_FRONTEND=noninteractive

function fake() {
	for x in /sbin/initctl /usr/sbin/invoke-rc.d /sbin/restart /sbin/start /sbin/stop /sbin/start-stop-daemon /usr/bin/service /usr/sbin/service /bin/systemctl
	do
		ln -snfv /bin/true $x
	done
}

apt-get update
if dpkg -l upstart | grep ^ii >/dev/null
then
	# Upgrade separately
	fake
	apt-get install -y -V upstart
	fake
fi

sed -i "s/update_initramfs=yes/update_initramfs=no/" $LOCATION/etc/initramfs-tools/update-initramfs.conf || true
echo "nameserver 8.8.8.8" > $LOCATION/etc/resolvconf/resolv.conf.d/head || true
echo "nameserver 8.8.8.8" > $LOCATION/etc/resolv.conf

apt-get upgrade -y -V
fake

sed -i "s/update_initramfs=yes/update_initramfs=no/" $LOCATION/etc/initramfs-tools/update-initramfs.conf || true
echo "nameserver 8.8.8.8" > $LOCATION/etc/resolvconf/resolv.conf.d/head || true
echo "nameserver 8.8.8.8" > $LOCATION/etc/resolv.conf

apt-get install -y -V git build-essential gcc g++ debhelper ccache bison flex texinfo yasm cmake libbsd-dev libmysqlclient-dev libopencv-dev libudev-dev rsyslog sudo $ADDITIONAL_PKGS
