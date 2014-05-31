#!/bin/bash
set -e
RELEASE_VERSION=7.7.1
wget ftp://sourceware.org/pub/gdb/releases/${RELEASE_VERSION}.tar.gz -O ${RELEASE_VERSION}.tar.gz
tar xzf ${RELEASE_VERSION}.tar.gz
ln -snf ${RELEASE_VERSION} gdb
echo $RELEASE_VERSION > $BASEDIR/gdb.version
