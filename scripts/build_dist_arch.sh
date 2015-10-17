#!/bin/bash
set -e
export COMMAND="./scripts/build_pkg_native.sh"
export DIST ARCH
`dirname $0`/exec_dist_arch.sh
