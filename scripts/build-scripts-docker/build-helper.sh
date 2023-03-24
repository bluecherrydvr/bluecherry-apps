#!/bin/bash -e

# This script is executed within the container as root.  It assumes
# that source code with debian packaging files can be found at
# /source-ro and that resulting packages are written to /output after
# succesful build.  These directories are mounted as docker volumes to
# allow files to be exchanged between the host and the container.

# Install extra dependencies that were provided for the build (if any)
#   Note: dpkg can fail due to dependencies, ignore errors, and use
#   apt-get to install those afterwards
[[ -d /dependencies ]] && dpkg -i /dependencies/*.deb || apt-get -f install -y --no-install-recommends

# Make read-write copy of source code
mkdir -p /build
cp -a /source-ro /build/source
cd /build/source

# Install build dependencies
#mk-build-deps -ir -t "apt-get -o Debug::pkgProblemResolver=yes -y --no-install-recommends"

# Build packages
#debuild -b -uc -us
apt-get update
apt --assume-yes install sudo curl
curl -o utils/onvif_tool http://lizard.bluecherry.net/~curt/onvif_tool
scripts/build_pkg_native.sh

    if [[ -e /etc/lsb-release ]]
    then
        . /etc/lsb-release
    fi

    if [[ -e /etc/os-release ]] 
    then
        . /etc/os-release
    fi
    
    if [[ -e /etc/os-release ]] && [[ $ID == "centos" ]]
    then
            DISTRO=${ID}_${VERSION_ID}
    else
    
    if [[ -e /etc/lsb-release ]]
    then
        DISTRO=$DISTRIB_CODENAME
    else
        DISTRO=`echo $VERSION | sed -e 's/^.*[(]//' -e 's/[)]//'`
    fi
    fi
    echo $DISTRO

ARCH=`uname -m`


# Copy packages to output dir with user's permissions
chown -R $USER:$GROUP /build
mkdir -p /output/$DISTRO/$ARCH/
cp -a releases/*.deb /output/$DISTRO/$ARCH/.
ls -l /output
