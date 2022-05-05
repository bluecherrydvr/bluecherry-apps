
# Creating Bluecherry packages

## Overview

Docker can be used to set up a clean build environment for Debian
packaging.  This tutorial shows how to create a container with
required build tools and how to use it to build packages.

## Create build environment for your targeted environment

Currently build scripts are provided for Debian 9/10, Ubuntu 18.04/20.04 and 21.04.

For example, to build for Ubuntu 20.04 run the following command:

    docker build -t ubuntu:20.04 -f Dockerfile-ubuntu-20.04 .


## Building packages

Clone the
[bluecherry-apps](https://github.com/bluecherrydvr/bluecherry-apps)
(the repository you are reading now).  For the example below we use ~git/bluecherry-apps however modify this to your liking and run the build script to see
usage:

./build -i ubuntu:20.04 -o output ~/git/bluecherry-apps/

    # create destination directory to store the build results
    mkdir output

    # build package from source directory
    ./build -i docker-deb-builder:17.04 -o output ~/my-package-source

After successful build you will find the `.deb` files in `output\$distro\$arch`
directory.

