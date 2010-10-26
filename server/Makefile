topdir = ..
include $(topdir)/mk.conf

CFLAGS		+= -pthread
LDFLAGS		+= -L$(topdir)/lib -lavformat -lbluecherry -lasound

TARGETS		= bc-server bc-detect
SERVER_OBJS	= bc-server.o bc-thread.o bc-av.o g723-dec.o bc-motion-tables.o
DETECT_OBJS	= bc-detect.o

all: $(TARGETS)

bc-server: $(LIB) $(SERVER_OBJS)
	$(LD) $(LDFLAGS) $(SERVER_OBJS) -o $@

bc-detect: $(LIB) $(DETECT_OBJS)
	$(LD) $(LDFLAGS) $(DETECT_OBJS) -o $@

install: all upstart-server-script upstart-detect-script rsyslog.conf logrotate.conf
	$(INSTALL_PROG) -D bc-server $(DESTDIR)$(sbin_dir)/bc-server
	$(INSTALL_PROG) -D bc-detect $(DESTDIR)$(sbin_dir)/bc-detect
	$(INSTALL_DATA) -D upstart-server-script \
		$(DESTDIR)$(etc_dir)/init/bc-server.conf
	$(INSTALL_DATA) -D upstart-detect-script \
		$(DESTDIR)$(etc_dir)/init/bc-detect.conf
	$(INSTALL_DATA) -D rsyslog.conf \
		$(DESTDIR)$(etc_dir)/rsyslog.d/99-bluecherry.conf
	$(INSTALL_DATA) -D logrotate.conf \
		$(DESTDIR)$(etc_dir)/logrotate.d/bluecherry

clean:
	rm -f $(SERVER_OBJS) $(DETECT_OBJS) $(TARGETS)

FORCE:
