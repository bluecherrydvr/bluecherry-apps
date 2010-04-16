include mk.conf

CPPFLAGS	+= -Ilib
LDFLAGS		+= -Llib -lbluecherry

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

install: all
	$(MAKE) -C lib $@
	$(MAKE) -C server $@
	$(INSTALL_PROG) -D bc.cgi $(DESTDIR)$(cgi_dir)/bc.cgi

FORCE:
