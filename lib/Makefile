topdir = ..
include $(topdir)/mk.conf

LDFLAGS		+= -lmysqlclient -lconfig -lm -lidn -lssl -lcrypto -lrt -lz
CPPFLAGS	+= -Ilibcurl/include
CFLAGS		+= -fPIC -DETCDIR="\"$(etc_dir)\""

LIBCURL		= libcurl/lib/.libs/libcurl.a
SOLIB		= libbluecherry.so
SOLIBVER	= $(SOLIB).0
OBJS		= bc-core.o bc-utils.o bc-db-core.o bc-db-mysql.o \
		  bc-key.o bc-media.o rtp-session.o

all: $(SOLIBVER)


libcurl/lib/Makefile: libcurl/configure
	cd libcurl && ./configure --prefix=/usr --disable-shared --disable-debug --enable-optimize --disable-warnings --disable-werror --disable-curldebug --enable-largefile --enable-static --enable-http --disable-ftp --disable-file --disable-ldap --disable-ldaps --enable-rtsp --disable-proxy --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-manual --enable-ipv6 --disable-thread-resolver --disable-verbose --disable-sspi --enable-crypto-auth --disable-hidden-symbols --with-pic --without-krb4 --without-ca-bindle --without-ca-path --without-libssh2

libcurl/lib/.libs/libcurl.a: libcurl/lib/Makefile
	$(MAKE) -C libcurl/lib

rtp-session.o: $(LIBCURL)

$(SOLIBVER): $(OBJS)
	echo $(RES)
	$(LD) $(LDFLAGS) -pthread -shared -Wl,-soname=$@ -o $@ $(OBJS) $(LIBCURL)
	ln -sf $@ $(SOLIB)

install: all
	$(INSTALL_DATA) -D $(SOLIBVER) $(DESTDIR)$(lib_dir)/$(SOLIBVER)

clean:
	rm -f $(OBJS) $(SOLIBVER) $(SOLIB)
	-$(MAKE) -C libcurl distclean
	rm -f libcurl/config.log

FORCE:
