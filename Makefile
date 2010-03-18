CPPFLAGS	= -Ilib
LDFLAGS		= -lavformat

CFLAGS		= -Wall -Werror -g -O0

TARGETS		= bc-reader bc.cgi

LIB		= lib/libbluecherry.a

all: $(TARGETS)

$(LIB): FORCE
	$(MAKE) -C lib

bc-reader: $(LIB) bc-reader.o

bc.cgi: $(LIB) bc.cgi.o

clean:
	$(MAKE) -C lib clean
	rm -f *.o $(TARGETS)

FORCE:
