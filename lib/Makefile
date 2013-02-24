include ../mk.conf

LDFLAGS		+= -lmysqlclient -lconfig -lm -lrt -lz -lbsd
CFLAGS		+= -fPIC -DETCDIR="\"$(etc_dir)\""

SOLIB		= libbluecherry.so
SOLIBVER	= $(SOLIB).0
OBJS		= bc-core.o bc-utils.o bc-db-core.o bc-db-mysql.o \
		  bc-key.o bc-media.o rtp-session.o bc-ptz.o motion.o

all: $(SOLIBVER) FORCE

$(SOLIBVER): $(OBJS)
	g++ $(LDFLAGS) -pthread -shared -Wl,-soname=$@ -o $@ $(OBJS)
	ln -sf $@ $(SOLIB)

install: all FORCE
	$(INSTALL_DATA) -D $(SOLIBVER) $(DESTDIR)$(lib_dir)/$(SOLIBVER)

clean: FORCE
	rm -f $(OBJS) $(SOLIBVER) $(SOLIB)
FORCE:
