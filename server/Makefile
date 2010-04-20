topdir = ..
include $(topdir)/mk.conf

CFLAGS		+= -pthread
LDFLAGS		+= -L$(topdir)/lib -lavformat -lbluecherry

TARGETS		= bc-server
OBJS		= bc-server.o bc-thread.o bc-av.o

all: $(TARGETS)

bc-server: $(LIB) $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

install: all upstart-script rsyslog.conf logrotate.conf
	$(INSTALL_PROG) -D bc-server $(DESTDIR)$(sbin_dir)/bc-server
	$(INSTALL_DATA) -D upstart-script \
		$(DESTDIR)$(etc_dir)/init/bc-server.conf
	$(INSTALL_DATA) -D rsyslog.conf \
		$(DESTDIR)$(etc_dir)/rsyslog.d/99-bluecherry.conf
	$(INSTALL_DATA) -D logrotate.conf \
		$(DESTDIR)$(etc_dir)/logrotate.d/bluecherry

clean:
	rm -f $(OBJS) $(TARGETS)

FORCE:
