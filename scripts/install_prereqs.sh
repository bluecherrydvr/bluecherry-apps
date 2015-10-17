#!/bin/bash
# Suppress services launching

. /etc/os-release
. /etc/lsb-release

for x in /sbin/initctl /usr/sbin/invoke-rc.d /sbin/restart /sbin/start /sbin/stop /sbin/start-stop-daemon /usr/bin/service /usr/sbin/service
do
	ln -snfv /bin/true $x
done

# Ubuntu 12 Precise lacks /etc/os-release, thus its ID is empty
if ! [[ -e /etc/os-release ]] && [[ $ID == "" ]] && [[ $DISTRIB_ID == "Ubuntu" ]]
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

export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y -V git build-essential gcc g++ debhelper php5-dev ccache bison flex texinfo yasm cmake libbsd-dev libmysqlclient-dev libopencv-dev libudev-dev rsyslog sudo
apt-get install -y -V $ADDITIONAL_PKGS
apt-get upgrade -y -V
