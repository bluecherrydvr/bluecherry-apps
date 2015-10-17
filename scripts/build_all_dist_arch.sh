#!/bin/bash
set -e

# TODO Implement parallel building outside of sources tree
for DIST in trusty precise wheezy jessie
do
	for ARCH in amd64 i386
	do
		echo "Doing for $DIST $ARCH"
		`dirname $0`/build_dist_arch.sh
	done
done

