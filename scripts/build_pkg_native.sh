#!/bin/bash
set -e
set -x

DST_DIR=releases/$DIST/

# Handy to avoid manual chroots updating and installation of new builddeps
sudo `dirname $0`/install_prereqs.sh

# TODO Implement building outside of sources tree

# Safety measures. TODO quick compilation without spare rebuilds
git submodule foreach --recursive "git clean -dxf && git reset --hard"
git submodule update --recursive --init

echo "#define GIT_REVISION \"`git describe --dirty --always --long` `git describe --all`\"" > server/version.h

debian/rules clean binary
mkdir -p $DST_DIR
mv -v ../*.deb $DST_DIR
