CPPFLAGS	= -I../lib -D_GNU_SOURCE
LDFLAGS		= -L../lib -lavformat -lm -lbluecherry

CFLAGS		= -Wall -Werror -g -O2 -pthread

TARGETS		= bc-server
LIB		= ../lib/libbluecherry.so
OBJS		= bc-server.o bc-thread.o bc-av.o

all: $(LIB) $(TARGETS)

$(LIB): FORCE
	$(MAKE) -C ../lib

bc-server: $(OBJS)

clean:
	rm -f $(OBJS) $(TARGETS)

FORCE:
