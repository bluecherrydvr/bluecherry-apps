OBJS = timeval.o base64.o hostip.o progress.o \
	formdata.o cookie.o http.o sendf.o url.o \
	if2ip.o speedcheck.o ssluse.o version.o getenv.o \
	escape.o mprintf.o netrc.o getinfo.o transfer.o \
	strequal.o easy.o wildcard.o memdebug.o fileinfo.o \
	http_chunks.o connect.o llist.o hash.o multi.o \
	content_encoding.o share.o http_digest.o md4.o md5.o \
	curl_rand.o http_negotiate.o strtoofft.o strerror.o \
	hostip4.o hostip6.o hostsyn.o parsedate.o select.o \
	sslgen.o splay.o rawstr.o curl_addrinfo.o slist.o \
	nonblock.o rtsp.o warnless.o hmac.o

CFLAGS = -g -O2 -Wall -fPIC
CPPFLAGS = -I. -D_GNU_SOURCE -DHAVE_CONFIG_H -D_FILE_OFFSET_BITS=64
CC = gcc
LD = $(CC)

TARGET = libcurl.a

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcu $@ $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)
