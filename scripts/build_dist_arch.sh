#!/bin/bash
set -e
set -x

export COMMAND="export DIST=$DIST ARCH=$ARCH && ./scripts/build_pkg_native.sh"
export DIST ARCH
`dirname $0`/exec_dist_arch.sh
