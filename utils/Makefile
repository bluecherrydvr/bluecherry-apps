topdir = ..
include $(topdir)/mk.conf

CPPFLAGS	+= -I$(topdir)/lib
LDFLAGS		+= -L$(topdir)/lib -lbluecherry

TARGETS		= getjpeg

all: $(TARGETS)

$(TARGETS): $(LIB)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $@.c -o $@

clean:
	rm -f $(TARGETS)

install:
