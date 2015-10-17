#!/bin/bash
set -e

# TODO Implement parallel building outside of sources tree
for DIST in trusty precise wheezy jessie
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

