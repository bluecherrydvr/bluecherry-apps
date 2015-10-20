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
		echo "Doing for $DIST $ARCH"
		`dirname $0`/exec_dist_arch.sh
	done
done

