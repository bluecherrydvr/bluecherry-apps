#!/bin/bash

if grep -q debian /etc/os-release # debian like
then
    dpkg --print-architecture
else # assume Centos
    uname -m
fi
