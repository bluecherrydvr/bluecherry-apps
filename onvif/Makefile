CXX = ${HOST}-g++
TARGET = onvifclient
INCLUDE = -I. -I./ -I./DeviceManagement -I./Discovery -I./Media -I./rpcserver/onvifserver
SOURCES = OnvifMgm.cpp main.cpp
CFLAGS = ${INCLUDE} -DWITH_DOM -DWITH_OPENSSL -fPIC -DUNICODE
LIBs = -L. -L./ -L./lib -lsoapdiscovery -lsoapdevmng -lsoapmedia -lxmlrpc_onvif_server -lssl -lpthread -lcrypto -ldl

all: 
	cd Discovery && make
	cd DeviceManagement && make
	cd Media && make
	cd rpcserver && make && make install
	cd rpcserver/onvifserver && make
	$(CXX) $(SOURCES) $(CFLAGS) $(LIBs) -o $(TARGET)
	echo Successfully Finisehd!!!

clean: 
	cd Discovery && make clean
	cd DeviceManagement && make clean
	cd Media && make clean
	cd rpcserver && make clean
	cd rpcserver/onvifserver && make clean
	rm -rf *.o *.a
	rm -rf onvifclient
