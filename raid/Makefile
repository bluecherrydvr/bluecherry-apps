CXX = ${HOST}-g++
TARGET = xmlrpcserver
INCLUDE = -I. -I./ -I./service
SOURCES = RaidMgm.cpp main.cpp
CFLAGS = ${INCLUDE} -DWITH_DOM -DWITH_OPENSSL -fPIC -DUNICODE
LIBs = -L. -L./ -L./lib -lraid_service -lssl -lpthread -lcrypto -ldl

all: 
	cd service && make
	$(CXX) $(SOURCES) $(CFLAGS) $(LIBs) -o $(TARGET)
	echo Successfully Finished!!!

clean: 
	cd service && make clean
	rm -rf *.o *.a
	rm -rf xmlrpcserver
