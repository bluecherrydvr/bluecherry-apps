#include "rtsp_connection.h"
#include "../lib/logging.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

rtsp_connection::rtsp_connection(rtsp_server *server, int fd)
    : server(server), fd(fd), rdbuf_len(0), wrbuf(0), wrbuf_tail(0)
{
    pthread_mutex_init(&write_lock, 0);
    server->addFd(this, fd, POLLIN);

    /* Configure socket buffers for optimal streaming performance */
    int sndbuf = 1024*1024;  // 1MB send buffer
    int rcvbuf = 1024*1024;  // 1MB receive buffer
    int keepalive = 1;       // Enable TCP keepalive
    int keepidle = 60;       // Start keepalive after 60 seconds of inactivity
    int keepintvl = 10;      // Send keepalive every 10 seconds
    int keepcnt = 3;         // Consider connection dead after 3 failed keepalives

    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
}

rtsp_connection::~rtsp_connection()
{
    // The called destructor goes back to rtsp_connection object and erases itself from `sessions`. Nasty loop. TODO Simplify objects relations
    while (!sessions.empty())
        delete sessions.begin()->second;
    server->removeFd(fd);
    close(fd);
    for (rtsp_write_buffer *p = wrbuf, *n; p; p = n) {
        n = p->next;
        delete p;
    }
    pthread_mutex_destroy(&write_lock);
}

int rtsp_connection::readable()
{
    if (rdbuf_len+1 >= sizeof(rdbuf)) {
        bc_log(Debug, "Connection read buffer exceeded; dropped connection");
        return -1;
    }

    ssize_t rd = read(fd, rdbuf + rdbuf_len, sizeof(rdbuf) - rdbuf_len - 1);
    if (rd < 1) {
        if (rd == 0) {
            bc_log(Debug, "Connection closed by peer");
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  // No data available, but connection is still good
        } else {
            bc_log(Error, "Read error: %s", strerror(errno));
        }
        return -1;
    }

    rdbuf_len += rd;
    rdbuf[rdbuf_len] = 0;  // Ensure null termination

    unsigned int parsed = parse();
    if (parsed < 0)
        return -1;

    if (parsed) {
        if (parsed < rdbuf_len)
            memmove(rdbuf, rdbuf + parsed, rdbuf_len - parsed);
        if (parsed > rdbuf_len)
            parsed = rdbuf_len;
        rdbuf_len -= parsed;
    }

    return 0;
}

int rtsp_connection::writable()
{
    pthread_mutex_lock(&write_lock);
    while (wrbuf) {
        rtsp_write_buffer *p = wrbuf;
        ssize_t wr = write(fd, p->data + p->pos, p->len - p->pos);
        if (wr < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                pthread_mutex_unlock(&write_lock);
                return 0;
            }
            bc_log(Error, "Write error: %s", strerror(errno));
            pthread_mutex_unlock(&write_lock);
            return -1;
        }
        p->pos += wr;
        if (p->pos == p->len) {
            wrbuf = p->next;
            if (!wrbuf)
                wrbuf_tail = 0;
            delete p;
        } else {
            pthread_mutex_unlock(&write_lock);
            return 0;
        }
    }
    pthread_mutex_unlock(&write_lock);
    return 0;
} 