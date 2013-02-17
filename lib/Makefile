include ../mk.conf

LDFLAGS		+= -lmysqlclient -lconfig -lm -lrt -lz -lbsd
CFLAGS		+= -fPIC -DETCDIR="\"$(etc_dir)\""

SOLIB		= libbluecherry.so
SOLIBVER	= $(SOLIB).0
OBJS		= bc-core.o bc-utils.o bc-db-core.o bc-db-mysql.o \
		  bc-key.o bc-media.o rtp_device.o bc-ptz.o \
		  input_device.o v4l2_device.o stream_elements.o \
		  logging.o

all: $(SOLIBVER)

$(SOLIBVER): $(OBJS)
	g++ $(LDFLAGS) -pthread -shared -Wl,-soname=$@ -o $@ $(OBJS)
	ln -sf $@ $(SOLIB)

install: all
	$(INSTALL_DATA) -D $(SOLIBVER) $(DESTDIR)$(lib_dir)/$(SOLIBVER)

clean:
	rm -f $(OBJS) $(SOLIBVER) $(SOLIB)
