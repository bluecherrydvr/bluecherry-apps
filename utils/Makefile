include ../mk.conf

CPPFLAGS	+= -iquote $(topdir)/server
LDFLAGS		+= -lbluecherry -lavcodec -lavformat -lavutil -lswscale

TARGETS		= getjpeg getaudio pelcocmd ptzcmd \
		  rtsp-record bc-license

all: $(TARGETS)

getaudio: %:%.o $(topdir)/server/g723-dec.o
	$(LD) -o $@ $^ $(LDFLAGS) -lasound

bc-license: %:%.o
	$(CXX) -o $@ $^

$(topdir)/server/g723-dec.o: %.o:%.c %.o:%.h
	$(MAKE) -C $(topdir)/server g723-dec.o

clean:
	rm -f $(TARGETS) *.o

install:
