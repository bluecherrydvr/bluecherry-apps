topdir = .
include $(topdir)/mk.conf

SUBDIRS		= misc lib server utils

all clean install:
	set -e; for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done
	$(MAKE) -C php5 -f Makefile.local $@
