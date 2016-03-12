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

# RPM
export DIST=centos_7 ARCH=x86_64
work

# TODO Implement parallel building outside of sources tree
for DIST in precise trusty wily wheezy jessie
do
	export DIST
	for ARCH in amd64 i386
	do
		export ARCH
		work
	done
done

export DIST=trusty ARCH=arm7l
work
