topdir = ..
include $(topdir)/mk.conf

CPPFLAGS	+= -I$(topdir)/lib -I$(topdir)/server
LDFLAGS		+= -L$(topdir)/lib -lbluecherry -lavcodec -lavformat

TARGETS		= getjpeg bc-checkkey getaudio rtsp-client pelcocmd ptzcmd \
		  rtsp-record
GET_AUDIO_OBJS	= getaudio.o $(topdir)/server/g723-dec.o
RTSP_CLIENT_OBJS = $(topdir)/lib/rtp-session.o rtsp-client.o

all: $(TARGETS)

getaudio: $(GET_AUDIO_OBJS)
	$(LD) -o $@ $(GET_AUDIO_OBJS) $(LDFLAGS) -lasound

rtsp-client: $(RTSP_CLIENT_OBJS)
	$(LD) $(RTSP_CLIENT_OBJS) -o $@ $(LDFLAGS)

$(topdir)/lib/rtp-session.o: $(topdir)/lib/rtp-session.c $(topdir)/lib/rtp-session.h
	$(MAKE) -C $(topdir)/lib rtp-session.o

$(topdir)/server/g723-dec.o: $(topdir)/server/g723-dec.c $(topdir)/server/g723-dec.h
	$(MAKE) -C $(topdir)/server g723-dec.o

clean:
	rm -f $(TARGETS) *.o

install:
