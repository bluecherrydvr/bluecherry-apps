#!/bin/bash

set -e

export BASEDIR=`readlink -f \`dirname $0\``
cd $BASEDIR
rm -rf results || true
mkdir results

for x in trusty precise lucid wheezy
do
	export DIST=$x
	for y in amd64 i386
	do
		export ARCH=$y
		for z in ffmpeg gdb
		do
			if ! [[ -e $BASEDIR/$z ]]
			then
				echo Fetching $z
				$BASEDIR/${z}_fetch.sh
			fi
			pushd $BASEDIR/$z

			echo Cleaning $z
			$BASEDIR/${z}_clean.sh
			echo Configuring $z
			schroot -c ${x}_${y} -p $BASEDIR/${z}_configure.sh
			echo Making $z
			schroot -c ${x}_${y} -p $BASEDIR/${z}_make.sh
			echo Publishing $z
			$BASEDIR/${z}_publish.sh
			popd
		done
	done
done
cp $BASEDIR/*.version $BASEDIR/results/
