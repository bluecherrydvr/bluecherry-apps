INCLUDE=-I./
CXX=${HOST}-g++ 
CFLAGS= ${INCLUDE} -fPIC -c 
LIBS= -lpthread

all:
	$(CXX) $(CFLAGS) raid_service.c
	$(CXX) $(LIBS) -shared -o libraid_service.so raid_service.o -lxmlrpc_server_abyss -lxmlrpc_server -xmlrpc_abyss -lxmlrpc -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok
	
	cp -f libraid_service.so ./../lib
	cp -f libraid_service.so /usr/lib
	echo Successfully Finished!!!
clean:
	rm -rf *.o *.a *.so
