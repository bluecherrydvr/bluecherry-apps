topdir = ..
include $(topdir)/mk.conf

LDFLAGS		+= -lsqlite3 -lmysqlclient -lconfig -lm

CFLAGS		+= -fPIC -DETCDIR="\"$(etc_dir)\""

SOLIB		= libbluecherry.so
SOLIBVER	= libbluecherry.so.0
OBJS		= bc-core.o bc-utils.o bc-db-core.o bc-db-sqlite.o \
		  bc-db-mysql.o bc-key.o bc-media.o

all: $(SOLIBVER)

$(SOLIBVER): $(OBJS)
	$(LD) $(LDFLAGS) -pthread -shared -Wl,-soname=$@ -o $@ $(OBJS)
	ln -sf $@ $(SOLIB)

install: all
	$(INSTALL_DATA) -D $(SOLIBVER) $(DESTDIR)$(lib_dir)/$(SOLIBVER)
#	ln -sf $(SOLIBVER) $(DESTDIR)$(lib_dir)/$(SOLIB)

clean:
	rm -f $(OBJS) $(SOLIBVER) $(SOLIB)
