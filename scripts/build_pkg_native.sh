#!/bin/bash
set -e

# TODO Implement building outside of sources tree

# Safety measures. TODO quick compilation without spare rebuilds
git submodule foreach --recursive "git clean -dxf && git reset --hard"
git submodule update --recursive --init

echo "#define GIT_REVISION \"`git describe --dirty --always --long` `git describe --all`\"" > server/version.h

debian/rules clean binary
