#!/bin/bash
# Suppress services launching
set -e
set -x

. /etc/*-release

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
	for x in /sbin/initctl /usr/sbin/invoke-rc.d /sbin/restart /sbin/start /sbin/stop /sbin/start-stop-daemon /usr/bin/service /usr/sbin/service
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
apt-get upgrade -y -V
fake

apt-get install -y -V git build-essential gcc g++ debhelper php5-dev ccache bison flex texinfo yasm cmake libbsd-dev libmysqlclient-dev libopencv-dev libudev-dev rsyslog sudo $ADDITIONAL_PKGS
