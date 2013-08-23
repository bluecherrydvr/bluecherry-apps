INCLUDE=-I./ -I./gsoap/
CXX=${HOST}-g++ 
CFLAGS= ${INCLUDE} -DLINUX_NAS -fPIC -c 

LIBS= -L/lib -lssl -lcrypto -ldl 

all:
	$(CXX) $(CFLAGS) gsoap/duration.cpp 
	$(CXX) $(CFLAGS) gsoap/soapC.cpp 
	$(CXX) $(CFLAGS) gsoap/soapClient.cpp 
	$(CXX) $(CFLAGS) gsoap/stdsoap2.cpp
	$(CXX) $(CFLAGS) gsoap/dom.cpp 
	$(CXX) $(CFLAGS) gsoap/mecevp.cpp 
	$(CXX) $(CFLAGS) gsoap/smdevp.cpp 
	$(CXX) $(CFLAGS) gsoap/wsseapi.cpp
	$(CXX) $(CFLAGS) gsoap/threads.cpp 
	$(CXX) $(CFLAGS) devmng.cpp 
	$(CXX) $(LIBS)  -shared -o libsoapdevmng.so duration.o soapC.o soapClient.o stdsoap2.o dom.o mecevp.o smdevp.o wsseapi.o threads.o devmng.o -Wl,--version-script=devmng.map
	
	cp -f libsoapdevmng.so /work/bluecherry/bluecherry-apps/onvif/lib
	cp -f libsoapdevmng.so /usr/lib
	echo Successfully Finished!!!
clean:
	rm -rf *.o *.a *.so
