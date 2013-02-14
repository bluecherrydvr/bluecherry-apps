
noinst_PROGRAMS = libconfig_tests

libconfig_tests_SOURCES = tests.c

libconfig_tests_CPPFLAGS = -I../tinytest -I../lib

libconfig_tests_LDADD = -L../tinytest -ltinytest \
	-L../lib/.libs -lconfig


EXTRA_DIST = \
	tests.vcproj \
	testdata/*.cfg \
	testdata/*.txt
