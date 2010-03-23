CPPFLAGS	= -Ilib
LDFLAGS		= -lavformat

CFLAGS		= -Wall -Werror -g -O2

TARGETS		= bc-reader bc.cgi server/bc-server

LIB		= lib/libbluecherry.a

all: $(TARGETS)

$(LIB): FORCE
	$(MAKE) -C lib

server/bc-server: FORCE
	$(MAKE) -C server

bc-reader: $(LIB) bc-reader.o

bc.cgi: $(LIB) bc.cgi.o

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C server clean
	rm -f *.o $(TARGETS)

FORCE:
