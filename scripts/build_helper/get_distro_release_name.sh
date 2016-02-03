#!/bin/bash


if [[ -e /etc/lsb-release ]]
then
    . /etc/lsb-release
fi
if [[ -e /etc/os-release ]] 
then
    . /etc/os-release
fi

if [[ -e /etc/os-release ]] && [[ $ID == "centos" ]]
then
        DISTRO=${ID}_${VERSION_ID}
else
if [[ -e /etc/lsb-release ]]
then
	# Ubuntu
	DISTRO=$DISTRIB_CODENAME
else
	# Debian
	DISTRO=`echo $VERSION | sed -e 's/^.*[(]//' -e 's/[)]//'`
fi
fi

echo $DISTRO
