#ifndef RTSP_CONNECTION_H
#define RTSP_CONNECTION_H

#include <pthread.h>
#include <map>
#include <string>

class rtsp_server;
class rtsp_session;

class rtsp_connection {
public:
    rtsp_connection(rtsp_server *server, int fd);
    ~rtsp_connection();

    int readable();
    int writable();

protected:
    rtsp_server *server;
    int fd;
    char rdbuf[4096];
    unsigned int rdbuf_len;

    struct rtsp_write_buffer {
        rtsp_write_buffer *next;
        char data[4096];
        unsigned int len;
        unsigned int pos;
    } *wrbuf, *wrbuf_tail;

    pthread_mutex_t write_lock;
    std::map<std::string, rtsp_session*> sessions;

    virtual unsigned int parse() = 0;
};

#endif // RTSP_CONNECTION_H 