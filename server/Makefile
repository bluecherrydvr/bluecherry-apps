topdir = ..
include $(topdir)/mk.conf

CFLAGS		+= -pthread $(shell pkg-config --cflags libsysfs)
LDFLAGS		+= -L$(topdir)/lib -lavformat -lbluecherry -lasound \
		   $(shell pkg-config --libs libsysfs)

TARGETS		= bc-server
SERVER_OBJS	= bc-server.o bc-thread.o bc-av.o g723-dec.o bc-motion-tables.o \
		  bc-detect.o

#CFLAGS		+= -DEBUG

all: $(TARGETS)

bc-server: $(LIB) $(SERVER_OBJS)
	$(LD) $(LDFLAGS) $(SERVER_OBJS) -o $@

install: all rsyslog.conf
	$(INSTALL_PROG) -D bc-server $(DESTDIR)$(sbin_dir)/bc-server
	$(INSTALL_DATA) -D rsyslog.conf \
		$(DESTDIR)$(etc_dir)/rsyslog.d/99-bluecherry.conf
	$(INSTALL_PROG) -d $(DESTDIR)/var/run/bluecherry
	echo "State file" > $(DESTDIR)/var/run/bluecherry/media-writable
	echo "State file" > $(DESTDIR)/var/run/bluecherry/db-writable
	touch -m -t 197001010000.00 $(DESTDIR)/var/run/bluecherry/media-writable
	touch -m -t 197001010000.00 $(DESTDIR)/var/run/bluecherry/db-writable

clean:
	rm -f $(SERVER_OBJS) $(DETECT_OBJS) $(TARGETS)

FORCE:
