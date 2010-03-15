#!/bin/bash

start=1
if [ "$1" = "" ]; then
	end=2
else
	end=$(expr $start + $1 - 1)
	shift
fi

for cnt in `seq $start $end`; do
	test -e "/dev/video$cnt" || continue
	./bc-reader -d "/dev/video$cnt" -o "test-$cnt.m4v" $@ &
done
