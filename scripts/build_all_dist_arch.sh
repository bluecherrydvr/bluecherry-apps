#!/bin/bash
set -e
set -x

function work {
	if [[ -e releases/${DIST}_${ARCH}.done ]]
	then
		continue
	fi
	echo "Doing for $DIST $ARCH"
	`dirname $0`/build_dist_arch.sh
	touch releases/${DIST}_${ARCH}.done
}

# TODO Implement parallel building outside of sources tree
for DIST in bionic buster
do
	export DIST
	for ARCH in amd64 i386
	do
		export ARCH
		work
	done
done

# RPM
export DIST=centos_7 ARCH=x86_64
work

# Doesn't work, needs debugging about qemu executing foreign-arch executables
#export DIST=trusty ARCH=arm7l
#work
