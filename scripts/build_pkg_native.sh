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

if [[ $(cat /etc/os-release | grep "^ID=" | grep debian) && \
	$(cat /etc/os-release | grep "^VERSION=" | grep 11) ]]
then
	apt-get install -y libmariadb-dev
	export BC_DEBIAN_BULLSEYE=y
fi

# TODO Implement building outside of sources tree

# Safety measures. TODO quick compilation without spare rebuilds
# NB: the clean/reset below stays disabled: it destroys local changes.
#git submodule foreach --recursive "git clean -dxf && git reset --hard"
# Populate submodules on fresh clones; without this the misc/ build fails
# with "not a git repository" / missing configure.ac (issue #768 reporter).
git submodule update --recursive --init

$(dirname $0)/check-db-version-consistency

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
	# Generate debian/control directly instead of via debian/rules:
	# dpkg >= 1.23 (Ubuntu 26.04+) reads debian/control while parsing
	# debian/rules (buildapi.mk), so invoking rules to CREATE control
	# is a chicken-and-egg failure. This replicates the rule recipe.
	RELEASE_NAME=$(./scripts/build_helper/get_distro_release_name.sh)
	if [ -z "$RELEASE_NAME" ]; then echo "could not determine distro release name" >&2; exit 1; fi
	sed -e "s/^#${RELEASE_NAME}//" debian/control.in | grep -v '^#' > debian/control
	debian/rules clean binary
	mkdir -p $DST_DIR
	mv -v ../bluecherry*.deb $DST_DIR
fi
