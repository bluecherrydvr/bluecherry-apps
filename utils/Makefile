topdir = ..
include $(topdir)/mk.conf

CPPFLAGS	+= -I$(topdir)/lib -I$(topdir)/server
LDFLAGS		+= -L$(topdir)/lib -lbluecherry -lavcodec -lavformat

TARGETS		= getjpeg bc-checkkey getaudio rtsp-client
GET_AUDIO_OBJS	= getaudio.o $(topdir)/server/g723-dec.o

all: $(TARGETS)

getaudio: $(GET_AUDIO_OBJS)
	$(LD) -o $@ $(GET_AUDIO_OBJS) -lavformat -lasound

$(topdir)/server/g723-dec.o: $(topdir)/server/g723-dec.c $(topdir)/server/g723-dec.h
	$(MAKE) -C $(topdir)/server g723-dec.o

clean:
	rm -f $(TARGETS) $(GET_AUDIO_OBJS)

install:
