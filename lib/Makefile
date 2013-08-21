include ../mk.conf

LDFLAGS		+= -lmysqlclient -lconfig -lm -lrt -lz -lbsd
LDFLAGS		+= -lavutil -lavformat -lavcodec
CFLAGS		+= -fPIC -DETCDIR="\"$(etc_dir)\""

SOLIB		= libbluecherry.so
SOLIBVER	= $(SOLIB).0
OBJS		= bc-core.o bc-utils.o bc-db-core.o bc-db-mysql.o \
		  bc-key.o bc-media.o rtp_device.o bc-ptz.o \
		  input_device.o v4l2_device.o stream_elements.o \
		  logging.o

all: $(SOLIBVER) FORCE

$(SOLIBVER): $(OBJS)
	$(CXX) $(LDFLAGS) -pthread -shared -Wl,-soname=$@ -o $@ $(OBJS)
	ln -sf $@ $(SOLIB)

install: all FORCE
	$(INSTALL_DATA) -D $(SOLIBVER) $(DESTDIR)$(lib_dir)/$(SOLIBVER)

clean: FORCE
	rm -f $(OBJS) $(OBJS:.o=.d) $(SOLIBVER) $(SOLIB)

FORCE:

-include $(OBJS:.o=.d)
