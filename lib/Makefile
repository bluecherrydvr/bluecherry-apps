topdir = ..
include $(topdir)/mk.conf

LDFLAGS		+= -lmysqlclient -lconfig -lm -lidn -lssl -lcrypto -lrt -lz
CPPFLAGS	+= -Ilibcurl
CFLAGS		+= -fPIC -DETCDIR="\"$(etc_dir)\""

SOLIB		= libbluecherry.so
SOLIBVER	= $(SOLIB).0
OBJS		= bc-core.o bc-utils.o bc-db-core.o bc-db-mysql.o \
		  bc-key.o bc-media.o rtp-session.o \
		  libcurl/libcurl.a

all: $(SOLIBVER)

libcurl/libcurl.a: FORCE
	$(MAKE) -C libcurl libcurl.a

$(SOLIBVER): $(OBJS)
	echo $(RES)
	$(LD) $(LDFLAGS) -pthread -shared -Wl,-soname=$@ -o $@ $(OBJS)
	ln -sf $@ $(SOLIB)

install: all
	$(INSTALL_DATA) -D $(SOLIBVER) $(DESTDIR)$(lib_dir)/$(SOLIBVER)

clean:
	rm -f $(OBJS) $(SOLIBVER) $(SOLIB)
	$(MAKE) -C libcurl clean

FORCE:
