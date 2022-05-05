#/bin/sh
time ./build -i docker-deb-builder:18.04 -o output ~/git/bluecherry-apps && time ./build -i docker-deb-builder:20.10 -o output ~/git/bluecherry-apps && time ./build -i docker-deb-builder:21.04 -o output ~/git/bluecherry-apps && time ./build -i docker-deb-builder:buster -o output ~/git/bluecherry-apps && time ./build -i docker-deb-builder:bullseye -o output ~/git/bluecherry-apps
