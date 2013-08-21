# Since the programs in this directory are examples for the user, this make
# file should be as ordinary as possible.  It should not rely heavily on
# included make files or configuration parameters.  Also, we don't try to
# build or rebuild the libraries on which these programs depend or even
# recognize that they've changed on their own.


ifeq ($(SRCDIR),)
SRCDIR = $(CURDIR)/..
BLDDIR = $(SRCDIR)
endif
SUBDIR = onvifserver

include $(BLDDIR)/config.mk

default: all

CFLAGS = $(CFLAGS_PERSONAL) $(CADD)
LDFLAGS += $(LADD)

# If this were a real application, working from an installed copy of
# Xmlrpc-c, XMLRPC_C_CONFIG would just be 'xmlrpc-c-config'.  It would be
# found in the user's PATH.
XMLRPC_C_CONFIG = $(BLDDIR)/xmlrpc-c-config.test

SERVERPROGS_ABYSS = \
  xmlrpc_onvif_server \

# Build up PROGS:
PROGS = 

PROGS += $(BASIC_PROGS)

ifeq ($(ENABLE_ABYSS_SERVER),yes)
  PROGS += $(SERVERPROGS_ABYSS)
endif

INCLUDES = -I. $(shell $(XMLRPC_C_CONFIG) client abyss-server --cflags)

LIBS_SERVER_ABYSS = \
  $(shell $(XMLRPC_C_CONFIG) abyss-server --libs)

all: $(PROGS)
	cp -f libxmlrpc_onvif_server.so /work/bluecherry/bluecherry-apps/onvif/lib
	cp -f libxmlrpc_onvif_server.so /usr/lib
	echo Successfully Finished!!!

$(SERVERPROGS_ABYSS):%:%.o
	$(CCLD) -fPIC -shared -o lib$@.so $^ $(LIBS_SERVER_ABYSS) $(LDFLAGS)

OBJECTS = $(patsubst %,%.o,$(patsubst %.cgi,%_cgi,$(PROGS)))

$(OBJECTS):%.o:%.c
	$(CC) -c $(INCLUDES) $(CFLAGS) $<

# config.h and xmlrpc_amconfig.h just describe the build environment.
# We use them so that the example programs will build in users'
# various environments.  If you're copying these examples, you can
# just remove these headers from the programs and hardcode whatever is
# right for your build environment.

$(OBJECTS): config.h xmlrpc_amconfig.h

config.h:
	$(LN_S) $(BLDDIR)/xmlrpc_config.h $@
xmlrpc_amconfig.h:
	$(LN_S) $(BLDDIR)/$@ .

clean:
	rm -rf *.o *.a *.so
