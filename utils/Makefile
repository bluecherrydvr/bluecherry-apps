include ../mk.conf

CPPFLAGS	+= -iquote $(topdir)/server
LDFLAGS		+= -lbluecherry -lavcodec -lavformat -lavutil -lswscale

TARGETS		= getjpeg getaudio pelcocmd ptzcmd \
		  rtsp-record bc-license rtsp-mjpeg

all: $(TARGETS)

getaudio: %:%.o $(topdir)/server/g723-dec.o
	$(LD) -o $@ $^ $(LDFLAGS) -lasound

bc-license: %:%.o
	$(CXX) -o $@ $^

$(topdir)/server/g723-dec.o: %.o:%.c %.h
	$(MAKE) -C $(@D) $(@F)

clean:
	rm -f $(TARGETS) *.o

install:
