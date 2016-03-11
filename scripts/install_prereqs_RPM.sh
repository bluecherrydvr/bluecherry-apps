#!/bin/bash
set -e
set -x

yum -y update
yum -y install epel-release # must be separate line, it installs epel repository
yum -y install dpkg-dev git make rpm-build gcc-c++ ccache autoconf automake libtool bison flex texinfo php-devel yasm cmake libbsd-devel chrpath mariadb-devel opencv-devel systemd-devel sudo

useradd -m build \
	&& echo "build ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/build \
	|| true

sudo -i -u build bash -c "cd; mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS} ; echo '%_topdir /home/build/rpmbuild' > ~/.rpmmacros"
