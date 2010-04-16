include ../mk.conf

CFLAGS		+= -pthread
CPPFLAGS	+= -I../lib -I.
LDFLAGS		+= -L../lib -lavformat -lm -lbluecherry

TARGETS		= bc-server
LIB		= ../lib/libbluecherry.so
OBJS		= bc-server.o bc-thread.o bc-av.o

all: $(LIB) $(TARGETS)

$(LIB): FORCE
	$(MAKE) -C ../lib

bc-server: $(OBJS)

install: all
	$(INSTALL_PROG) -D bc-server $(DESTDIR)$(sbin_dir)/bc-server

clean:
	rm -f $(OBJS) $(TARGETS)

FORCE:
