include $(topdir)/mk.conf

LIBAV_INSTALL = libav/lib/libavcodec.so.53 \
		libav/lib/libavformat.so.53 \
		libav/lib/libavutil.so.51 \
		libav/lib/libswscale.so.2

all: libav-build FORCE

clean: FORCE
	-$(MAKE) -C libav distclean

install: FORCE
	install -d $(DESTDIR)$(libexec_dir)
	$(INSTALL_PROG) $(LIBAV_INSTALL) -t $(DESTDIR)$(libexec_dir)

libav/config.mak:
	cd libav && ./configure --prefix=`pwd` --disable-static --enable-shared --enable-pic

libav-build: $(LIBAV_INSTALL) FORCE

$(LIBAV_INSTALL): libav/config.mak
	$(MAKE) $(MAKEARGS) -C libav install

FORCE:
