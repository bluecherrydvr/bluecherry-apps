#!/bin/bash
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

if [[ $VERSION == "9 (stretch)" || $VERSION == "10 (buster)" ]]
then
        ADDITIONAL_PKGS="$ADDITIONAL_PKGS default-libmysqlclient-dev"
else
        ADDITIONAL_PKGS="$ADDITIONAL_PKGS "
fi

export DEBIAN_FRONTEND=noninteractive

apt-get update

apt-get upgrade -y -V

apt-get install -y -V autoconf automake libtool git build-essential gcc g++ debhelper ccache bison flex texinfo yasm cmake libbsd-dev libopencv-dev libudev-dev libva-dev rsyslog sudo gsoap libgsoap-dev $ADDITIONAL_PKGS

