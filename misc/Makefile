include ../mk.conf

all: build FORCE

 ###############
### libconfig ###
 ###############
pkgs += libconfig
libconfig-libs = $(call lpath, config.so.9)
install-libs += $(libconfig-libs)

libconfig-clean: FORCE
	-$(MAKE) -C libconfig clean
	-rm libconfig/Makefile

libconfig/Makefile:
	cd libconfig && ./configure --prefix=$(STAGE) --disable-static --disable-examples

$(libconfig-libs): libconfig/Makefile
	$(MAKE) $(MAKEARGS) -C libconfig install

 ###############
###   libav   ###
 ###############
pkgs += libav
LIBAV_INSTALL = $(call lpath, \
		avcodec.so.53 \
		avformat.so.53 \
		avutil.so.51 \
		swscale.so.2)

install-libs += $(LIBAV_INSTALL)

libav/config.mak:
	cd libav && ./configure --prefix=$(STAGE) --disable-static --enable-shared --enable-pic

libav-clean: FORCE
	-$(MAKE) -C libav distclean

$(LIBAV_INSTALL): libav/config.mak
	$(MAKE) $(MAKEARGS) -C libav install

 ####################
### Common targets ###
 ####################

build: $(install-libs) FORCE

install: build FORCE
	$(INSTALL_PROG) -d $(DESTDIR)$(libexec_dir)
	$(INSTALL_PROG) $(install-libs) -t $(DESTDIR)$(libexec_dir)

clean: $(foreach d,$(pkgs), $d-clean) FORCE
	rm -rf $(STAGE)


FORCE:
