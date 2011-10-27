topdir = ..
include $(topdir)/mk.conf

CFLAGS		+= -pthread $(shell pkg-config --cflags libsysfs)
LDFLAGS		+= -L$(topdir)/lib -lbluecherry -lasound \
		   $(shell pkg-config --libs libsysfs) -lbsd \
		   -L$(LIBAVPATH)/lib -Wl,-rpath $(libexec_dir) -lavformat -lavcodec -lavutil

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

clean:
	rm -f $(SERVER_OBJS) $(DETECT_OBJS) $(TARGETS)

FORCE:
