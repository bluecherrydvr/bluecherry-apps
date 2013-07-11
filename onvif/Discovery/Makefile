INCLUDE=-I./ -I./gsoap/
CXX=${HOST}-g++ 
CFLAGS= ${INCLUDE} -DLINUX_NAS -fPIC -c 

LIBS=-lpthread

all:
	$(CXX) $(CFLAGS) gsoap/threads.cpp
	$(CXX) $(CFLAGS) gsoap/soapC.cpp
	$(CXX) $(CFLAGS) gsoap/soapClient.cpp
	$(CXX) $(CFLAGS) gsoap/stdsoap2.cpp
	$(CXX) $(CFLAGS) gsoap/wsaapi.cpp
	$(CXX) $(CFLAGS) gsoap/wsddapi.cpp
	$(CXX) $(CFLAGS) NVRArrayList.cpp
	$(CXX) $(CFLAGS) NVRList.cpp
	$(CXX) $(CFLAGS) discovery.cpp
	
	$(CXX) $(LIBS) -shared -o libsoapdiscovery.so soapC.o soapClient.o stdsoap2.o threads.o wsaapi.o wsddapi.o NVRArrayList.o NVRList.o discovery.o -Wl,--version-script=discovery.map
	
	cp -f libsoapdiscovery.so /work/bluecherry/bluecherry-apps/onvif/lib
	cp -f libsoapdiscovery.so /usr/lib
clean:
	rm -rf *.o *.a *.so *.gch
