topdir = .
include $(topdir)/mk.conf

SUBDIRS		= lib server utils

all install: client/Makefile
	set -e; for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done
	$(MAKE) -C php5 -f Makefile.local $@
	$(MAKE) -C client $@

clean:
	set -e; for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done
	$(MAKE) -C php5 -f Makefile.local $@
	-$(MAKE) -C client distclean

client/Makefile: client/client.pro
	cd && qmake
