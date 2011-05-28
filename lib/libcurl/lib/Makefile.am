#***************************************************************************
#                                  _   _ ____  _
#  Project                     ___| | | |  _ \| |
#                             / __| | | | |_) | |
#                            | (__| |_| |  _ <| |___
#                             \___|\___/|_| \_\_____|
#
# Copyright (C) 1998 - 2010, Daniel Stenberg, <daniel@haxx.se>, et al.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution. The terms
# are also available at http://curl.haxx.se/docs/copyright.html.
#
# You may opt to use, copy, modify, merge, publish, distribute and/or sell
# copies of the Software, and permit persons to whom the Software is
# furnished to do so, under the terms of the COPYING file.
#
# This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
# KIND, either express or implied.
#
###########################################################################
AUTOMAKE_OPTIONS = foreign nostdinc

lib_LTLIBRARIES = libcurl.la
LIBCURL_LIBS = @LIBCURL_LIBS@

# This might hold -Werror
CFLAGS += @CURL_CFLAG_EXTRAS@

INCLUDES = -I$(top_builddir)/include/curl \
           -I$(top_builddir)/include      \
           -I$(top_srcdir)/include        \
           -I$(top_builddir)/lib          \
           -I$(top_srcdir)/lib

VERSIONINFO=-version-info 6:0:2

libcurl_la_LDFLAGS = $(VERSIONINFO) $(LIBCURL_LIBS)

# Makefile.inc provides the CSOURCES and HHEADERS defines
include Makefile.inc

libcurl_la_SOURCES = $(CSOURCES) $(HHEADERS)
