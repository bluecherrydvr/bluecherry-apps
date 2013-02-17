include ../mk.conf

CPPFLAGS	+= -I$(topdir)/lib -I$(topdir)/server
LDFLAGS		+= -lbluecherry -lavcodec -lavformat -lavutil -lswscale

TARGETS		= getjpeg getaudio pelcocmd ptzcmd \
		  rtsp-record bc-license
GET_AUDIO_OBJS	= getaudio.o $(topdir)/server/g723-dec.o

all: $(TARGETS)

getaudio: $(GET_AUDIO_OBJS)
	$(LD) -o $@ $(GET_AUDIO_OBJS) $(LDFLAGS) -lasound

bc-license: bc-license.o
	$(CXX) -o $@ bc-license.o

$(topdir)/server/g723-dec.o: $(topdir)/server/g723-dec.c $(topdir)/server/g723-dec.h
	$(MAKE) -C $(topdir)/server g723-dec.o

clean:
	rm -f $(TARGETS) *.o

install:
