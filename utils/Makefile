topdir = ..
include $(topdir)/mk.conf

CPPFLAGS	+= -I$(topdir)/lib
LDFLAGS		+= -L$(topdir)/lib -lbluecherry

TARGETS		= getjpeg bc.cgi

all: $(TARGETS)

$(TARGETS): $(LIB)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $@.c -o $@

clean:
	rm -f $(TARGETS)

install: bc.cgi
	$(INSTALL_PROG) -D bc.cgi $(DESTDIR)$(cgi_dir)/bc.cgi
