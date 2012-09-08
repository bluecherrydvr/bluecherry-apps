topdir = ..
include $(topdir)/mk.conf

CFLAGS		+= -pthread -MMD
LDFLAGS		+= -L$(topdir)/lib -lbluecherry -lbsd -ludev \
		   -L$(LIBAVPATH)/lib -Wl,-rpath=$(libexec_dir) -lavformat -lavcodec -lavutil -lswscale

TARGETS		= bc-server
SERVER_OBJS	= bc-server.o bc-thread.o bc-av.o g723-dec.o bc-detect.o streaming.o rtsp.o

all: $(TARGETS) FORCE

bc-server: $(LIB) $(SERVER_OBJS)
	g++ $(LDFLAGS) $(SERVER_OBJS) -o $@

install: all rsyslog.conf FORCE
	$(INSTALL_PROG) -D bc-server $(DESTDIR)$(sbin_dir)/bc-server
	$(INSTALL_DATA) -D rsyslog.conf \
		$(DESTDIR)$(etc_dir)/rsyslog.d/99-bluecherry.conf

clean: FORCE
	rm -f $(SERVER_OBJS) $(DETECT_OBJS) $(TARGETS)

-include $(SERVER_OBJS:%.o=%.d)

FORCE:
