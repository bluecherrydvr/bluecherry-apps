topdir = ..
include $(topdir)/mk.conf

CFLAGS		+= -pthread
LDFLAGS		+= -L$(topdir)/lib -lavformat -lm -lbluecherry

TARGETS		= bc-server
OBJS		= bc-server.o bc-thread.o bc-av.o

all: $(TARGETS)

bc-server: $(LIB) $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

install: all
	$(INSTALL_PROG) -D bc-server $(DESTDIR)$(sbin_dir)/bc-server

clean:
	rm -f $(OBJS) $(TARGETS)

FORCE:
