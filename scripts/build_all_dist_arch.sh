#!/bin/bash
set -e
set -x

# TODO Implement parallel building outside of sources tree
for DIST in precise trusty wily wheezy jessie sid
do
	export DIST
	for ARCH in amd64 i386
	do
		export ARCH
		if [[ -e releases/${DIST}_${ARCH}.done ]]
		then
			continue
		fi
		echo "Doing for $DIST $ARCH"
		`dirname $0`/build_dist_arch.sh
		touch releases/${DIST}_${ARCH}.done
	done
done

