include ../mk.conf

CFLAGS		+= -pthread -MMD
LDFLAGS		+= -lbluecherry -lbsd -ludev \
		   -lavformat -lavcodec -lavutil -lswscale

TARGETS		= bc-server
SERVER_OBJS	= bc-server.o bc-thread.o bc-av.o g723-dec.o bc-detect.o streaming.o rtsp.o signals.o motion_processor.o

all: $(TARGETS) FORCE

bc-server: $(LIB) $(SERVER_OBJS)
	$(CXX) $(LDFLAGS) $(SERVER_OBJS) -o $@

install: all rsyslog.conf FORCE
	$(INSTALL_PROG) -D bc-server $(DESTDIR)$(sbin_dir)/bc-server
	$(INSTALL_DATA) -D rsyslog.conf \
		$(DESTDIR)$(etc_dir)/rsyslog.d/99-bluecherry.conf

clean: FORCE
	rm -f $(SERVER_OBJS) $(SERVER_OBJS:%.o=%.d) $(TARGETS)

-include $(SERVER_OBJS:%.o=%.d)

FORCE:
