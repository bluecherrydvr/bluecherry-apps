#!/bin/bash


. /etc/*-release
if [[ -e /etc/lsb-release ]]
then
	# Ubuntu
	DISTRO=$DISTRIB_CODENAME
else
	# Debian
	DISTRO=`echo $VERSION | sed -e 's/^.*[(]//' -e 's/[)]//'`
fi

echo $DISTRO
