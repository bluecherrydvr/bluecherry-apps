#!/bin/bash
make -j`grep -c ^processor /proc/cpuinfo`
