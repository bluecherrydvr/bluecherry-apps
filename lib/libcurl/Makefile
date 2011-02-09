OBJS = timeval.o base64.o hostip.o progress.o \
	formdata.o cookie.o http.o sendf.o url.o dict.o \
	if2ip.o speedcheck.o ssluse.o version.o getenv.o \
	escape.o mprintf.o netrc.o getinfo.o transfer.o \
	strequal.o easy.o security.o curl_fnmatch.o \
	wildcard.o memdebug.o fileinfo.o \
	http_chunks.o strtok.o connect.o llist.o hash.o multi.o \
	content_encoding.o share.o http_digest.o md4.o md5.o \
	curl_rand.o http_negotiate.o http_ntlm.o inet_pton.o \
	strtoofft.o strerror.o hostip4.o \
	hostip6.o hostsyn.o inet_ntop.o parsedate.o \
	select.o gtls.o sslgen.o splay.o strdup.o \
	rawstr.o curl_addrinfo.o \
	curl_sspi.o slist.o \
	nonblock.o curl_memrchr.o \
	rtsp.o warnless.o hmac.o \
	curl_gethostname.o

CFLAGS = -g -O2 -Wall -fPIC #-Werror
CPPFLAGS = -DCURLDEBUG -DDEBUGBUILD -I. -DHAVE_CONFIG_H
CC = gcc
LD = $(CC)

TARGET = libcurl.a

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcu $@ $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)
