INCLUDE=-I./
CXX=${HOST}-gcc 
CFLAGS= ${INCLUDE} -fPIC -c 
LIBS= -lpthread

all:
	$(CXX) $(CFLAGS) onvif_service.c
	$(CXX) $(LIBS) -shared -o libonvif_service.so onvif_service.o -lxmlrpc_server_abyss -lxmlrpc_server -xmlrpc_abyss -lxmlrpc -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok
	
	cp -f libonvif_service.so ./../lib
	cp -f libonvif_service.so /usr/lib
	echo Successfully Finished!!!
clean:
	rm -rf *.o *.a *.so
