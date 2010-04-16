CPPFLAGS	= -Ilib
LDFLAGS		= -Llib -lbluecherry

CFLAGS		= -Wall -Werror -g -O2

TARGETS		= getjpeg bc.cgi server/bc-server

LIB		= lib/libbluecherry.so

all: $(LIB) $(TARGETS)

$(LIB): FORCE
	$(MAKE) -C lib

server/bc-server: $(LIB) FORCE
	$(MAKE) -C server

getjpeg: $(LIB) getjpeg.o

bc.cgi: $(LIB) bc.cgi.o

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C server clean
	rm -f *.o $(TARGETS)

FORCE:
