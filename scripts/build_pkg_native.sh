#!/bin/bash
set -e
set -x

DST_DIR=releases/$DIST/

# Handy to avoid manual chroots updating and installation of new builddeps
if [[ $DIST == centos* ]]
then
	sudo `dirname $0`/install_prereqs_RPM.sh
else
	sudo `dirname $0`/install_prereqs_native.sh
fi

# TODO Implement building outside of sources tree

# Safety measures. TODO quick compilation without spare rebuilds
git submodule foreach --recursive "git clean -dxf && git reset --hard"
git submodule update --recursive --init

echo "#define GIT_REVISION \"`git describe --dirty --always --long` `git describe --all`\"" > server/version.h

if [[ $DIST == centos* ]]
then
	UPSTREAM_VERSION=`dpkg-parsechangelog | grep ^Version: | grep -E -o '([0-9]+\.){2,3}([0-9]+)'`
	tar -czf ~build/rpmbuild/SOURCES/bluecherry-$UPSTREAM_VERSION.tar.gz .
	sed -e "s/%%UPSTREAM_VERSION%%/${UPSTREAM_VERSION}/" rpm/bluecherry.spec.template \
	       > ~build/rpmbuild/SPECS/bluecherry.spec
	sudo -i -u build bash -c "cd rpmbuild/SPECS; rpmbuild -bb bluecherry.spec"
	mkdir -p $DST_DIR
	mv -v ~build/rpmbuild/RPMS/x86_64/*.rpm $DST_DIR
else
	debian/rules debian/control
	debian/rules clean binary
	mkdir -p $DST_DIR
	mv -v ../bluecherry*.deb $DST_DIR
fi
