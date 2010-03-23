CPPFLAGS	= -I../lib -D_GNU_SOURCE
LDFLAGS		= -lavformat -lpthread

CFLAGS		= -Wall -Werror -g -O2 -pthread

TARGETS		= bc-server
LIB		= ../lib/libbluecherry.a
OBJS		= bc-server.o bc-av.o

all: $(TARGETS)

$(LIB): FORCE
	$(MAKE) -C ../lib

bc-server: $(LIB) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGETS)

FORCE:
