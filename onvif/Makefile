CXX = ${HOST}-g++
TARGET = onvifclient
INCLUDE = -I. -I./ -I./DeviceManagement -I./Discovery -I./Media
SOURCES = OnvifMgm.cpp main.cpp
CFLAGS = ${INCLUDE} -DWITH_DOM -DWITH_OPENSSL -fPIC -DUNICODE
LIBs = -L. -L./ -L./lib -lsoapdiscovery -lsoapdevmng -lsoapmedia -lssl -lpthread -lcrypto -ldl

all:
	$(CXX) $(SOURCES) $(CFLAGS) $(LIBs) -o $(TARGET)
	echo Successfully Finisehd!!!

clean:
	rm -rf *.o *.a
	rm -rf onvifclient
