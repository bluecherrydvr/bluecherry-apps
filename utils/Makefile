topdir = ..
include $(topdir)/mk.conf

CPPFLAGS	+= -I$(topdir)/lib
LDFLAGS		+= -L$(topdir)/lib -lbluecherry

TARGETS		= getjpeg bc-checkkey

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

install:
