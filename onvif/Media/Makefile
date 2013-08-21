INCLUDE=-I./ -I./gsoap/
CXX=${HOST}-g++ 
CFLAGS= ${INCLUDE} -DLINUX_NAS -fPIC -c 

LIBS = -L/lib -lssl -lcrypto -ldl

all:
	$(CXX) $(CFLAGS) gsoap/soapC.cpp
	$(CXX) $(CFLAGS) gsoap/soapClient.cpp
	$(CXX) $(CFLAGS) gsoap/stdsoap2.cpp
	$(CXX) $(CFLAGS) gsoap/duration.cpp
	$(CXX) $(CFLAGS) gsoap/dom.cpp
	$(CXX) $(CFLAGS) gsoap/mecevp.cpp
	$(CXX) $(CFLAGS) gsoap/smdevp.cpp
	$(CXX) $(CFLAGS) gsoap/wsseapi.cpp
	$(CXX) $(CFLAGS) media.cpp
	
	$(CXX) $(LIBS) -shared -o libsoapmedia.so soapC.o soapClient.o stdsoap2.o duration.o  wsseapi.o mecevp.o smdevp.o dom.o media.o -Wl,--version-script=media.map
	
	cp -f libsoapmedia.so /work/bluecherry/bluecherry-apps/onvif/lib
	cp -f libsoapmedia.so /usr/lib	
	echo Successfully Finisehd!!!

clean:
	rm -rf *.o *.a *.so
