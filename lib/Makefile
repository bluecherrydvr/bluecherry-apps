topdir = ..
include $(topdir)/mk.conf

LDFLAGS		+= -lmysqlclient -lconfig -lm

CFLAGS		+= -fPIC -DETCDIR="\"$(etc_dir)\""

SOLIB		= libbluecherry.so
SOLIBVER	= $(SOLIB).0
OBJS		= bc-core.o bc-utils.o bc-db-core.o bc-db-mysql.o \
		  bc-key.o bc-media.o

all: $(SOLIBVER)

$(SOLIBVER): $(OBJS)
	$(LD) $(LDFLAGS) -pthread -shared -Wl,-soname=$@ -o $@ $(OBJS)
	ln -sf $@ $(SOLIB)

install: all
	$(INSTALL_DATA) -D $(SOLIBVER) $(DESTDIR)$(lib_dir)/$(SOLIBVER)

clean:
	rm -f $(OBJS) $(SOLIBVER) $(SOLIB)
