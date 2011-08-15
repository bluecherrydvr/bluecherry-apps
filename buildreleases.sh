#!/bin/sh
# Make sure the directories are created
mkdir ../releases/lucid/i386 -p
mkdir ../releases/lucid/amd64 -p
mkdir ../releases/maverick/amd64 -p
mkdir ../releases/maverick/i386 -p
mkdir ../releases/natty/i386 -p
mkdir ../releases/natty/amd64 -p

# clean and build a release for each chroot.  Move the deb to it's release folder so it isn't overwritten
schroot -c lucid_i386 -u root fakeroot debian/rules clean
schroot -c lucid_i386 -u root fakeroot debian/rules binary
mv ../bluecherry_2.0.0-0beta7.2_i386.deb ../releases/lucid/i386
schroot -c lucid_amd64 -u root fakeroot debian/rules clean
schroot -c lucid_amd64 -u root fakeroot debian/rules binary
mv ../bluecherry_2.0.0-0beta7.2_amd64.deb ../releases/lucid/amd64
schroot -c maverick_i386 -u root fakeroot debian/rules clean 
schroot -c maverick_i386 -u root fakeroot debian/rules binary
mv ../bluecherry_2.0.0-0beta7.2_i386.deb ../releases/maverick/i386
schroot -c maverick_amd64 -u root fakeroot debian/rules clean
schroot -c maverick_amd64 -u root fakeroot debian/rules binary
mv ../bluecherry_2.0.0-0beta7.2_amd64.deb ../releases/maverick/amd64
schroot -c natty_i386 -u root fakeroot debian/rules clean
schroot -c natty_i386 -u root fakeroot debian/rules binary
mv ../bluecherry_2.0.0-0beta7.2_i386.deb ../releases/natty/i386
schroot -c natty_amd64 -u root fakeroot debian/rules clean
schroot -c natty_amd64 -u root fakeroot debian/rules binary
mv ../bluecherry_2.0.0-0beta7.2_amd64.deb ../releases/natty/amd64

# Throw a md5sum down for good measure
md5sum ../releases/*/*/* >> ../releases/md5sum.txt
