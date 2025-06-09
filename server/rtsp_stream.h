#ifndef RTSP_STREAM_H
#define RTSP_STREAM_H

#include <pthread.h>
#include <queue>
#include <map>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
}

class rtsp_server;
class rtsp_session;

class rtsp_stream {
public:
    rtsp_stream(rtsp_server *server, const char *name, const char *url);
    ~rtsp_stream();

    void add_session(rtsp_session *session);
    void remove_session(rtsp_session *session);
    void feed_packet(AVPacket *pkt);

private:
    static void *run(void *arg);

    rtsp_server *server;
    std::string name;
    std::string url;
    time_t last_packet_time;
    size_t max_queue_size;

    pthread_t thread;
    pthread_mutex_t sessions_lock;
    pthread_cond_t sessions_cond;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_cond;

    std::map<std::string, rtsp_session*> sessions;
    std::queue<AVPacket*> packet_queue;
};

#endif // RTSP_STREAM_H 