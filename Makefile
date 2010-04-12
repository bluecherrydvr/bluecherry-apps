CPPFLAGS	= -Ilib
LDFLAGS		= -Llib -lbluecherry

CFLAGS		= -Wall -Werror -g -O2

TARGETS		= getjpeg bc.cgi server/bc-server

LIB		= lib/libbluecherry.so

all: $(LIB) $(TARGETS)

$(LIB): FORCE
	$(MAKE) -C lib

server/bc-server: FORCE
	$(MAKE) -C server

getjpeg: getjpeg.o

bc.cgi: bc.cgi.o

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C server clean
	rm -f *.o $(TARGETS)

FORCE:
