#include "rtsp_stream.h"
#include "../lib/logging.h"
#include <time.h>

rtsp_stream::rtsp_stream(rtsp_server *server, const char *name, const char *url)
    : server(server), name(name), url(url), last_packet_time(0), max_queue_size(1000)
{
    pthread_mutex_init(&sessions_lock, 0);
    pthread_cond_init(&sessions_cond, 0);
    pthread_mutex_init(&queue_lock, 0);
    pthread_cond_init(&queue_cond, 0);
    pthread_create(&thread, 0, run, this);
}

rtsp_stream::~rtsp_stream()
{
    pthread_mutex_lock(&sessions_lock);
    while (!sessions.empty())
        delete sessions.begin()->second;
    pthread_mutex_unlock(&sessions_lock);
    pthread_join(thread, 0);
    pthread_mutex_destroy(&sessions_lock);
    pthread_cond_destroy(&sessions_cond);
    pthread_mutex_destroy(&queue_lock);
    pthread_cond_destroy(&queue_cond);
}

void rtsp_stream::add_session(rtsp_session *session)
{
    pthread_mutex_lock(&sessions_lock);
    sessions[session->id] = session;
    pthread_cond_signal(&sessions_cond);
    pthread_mutex_unlock(&sessions_lock);
}

void rtsp_stream::remove_session(rtsp_session *session)
{
    pthread_mutex_lock(&sessions_lock);
    sessions.erase(session->id);
    pthread_mutex_unlock(&sessions_lock);
}

void rtsp_stream::feed_packet(AVPacket *pkt)
{
    if (!pkt || !pkt->data || pkt->size <= 0)
        return;

    pthread_mutex_lock(&queue_lock);
    
    // Adaptive queue sizing based on number of active sessions
    pthread_mutex_lock(&sessions_lock);
    size_t active_sessions = sessions.size();
    pthread_mutex_unlock(&sessions_lock);
    
    // Adjust max queue size based on number of sessions
    // Base size of 1000 packets, plus 500 per active session
    max_queue_size = 1000 + (active_sessions * 500);
    
    // If queue is too large, drop oldest packets
    while (packet_queue.size() >= max_queue_size) {
        AVPacket *old_pkt = packet_queue.front();
        packet_queue.pop();
        av_packet_free(&old_pkt);
        bc_log(Debug, "Dropped old packet due to queue size limit");
    }

    // Create new packet and copy data
    AVPacket *new_pkt = av_packet_alloc();
    if (!new_pkt) {
        bc_log(Error, "Failed to allocate new packet");
        pthread_mutex_unlock(&queue_lock);
        return;
    }

    if (av_packet_ref(new_pkt, pkt) < 0) {
        bc_log(Error, "Failed to reference packet");
        av_packet_free(&new_pkt);
        pthread_mutex_unlock(&queue_lock);
        return;
    }

    packet_queue.push(new_pkt);
    last_packet_time = time(0);
    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_lock);
}

void *rtsp_stream::run(void *arg)
{
    rtsp_stream *stream = (rtsp_stream *)arg;
    while (1) {
        pthread_mutex_lock(&stream->queue_lock);
        while (stream->packet_queue.empty()) {
            pthread_cond_wait(&stream->queue_cond, &stream->queue_lock);
        }

        AVPacket *pkt = stream->packet_queue.front();
        stream->packet_queue.pop();
        pthread_mutex_unlock(&stream->queue_lock);

        // Check for session timeouts
        time_t now = time(0);
        pthread_mutex_lock(&stream->sessions_lock);
        for (auto it = stream->sessions.begin(); it != stream->sessions.end();) {
            rtsp_session *session = it->second;
            if (now - session->last_activity > 60) {  // 60 second timeout
                bc_log(Debug, "Session %s timed out", session->id.c_str());
                delete session;
                it = stream->sessions.erase(it);
            } else {
                ++it;
            }
        }
        pthread_mutex_unlock(&stream->sessions_lock);

        // Send packet to all active sessions
        pthread_mutex_lock(&stream->sessions_lock);
        for (auto &pair : stream->sessions) {
            rtsp_session *session = pair.second;
            if (session->state == rtsp_session::Playing) {
                session->send_packet(pkt);
            }
        }
        pthread_mutex_unlock(&stream->sessions_lock);

        av_packet_free(&pkt);
    }
    return 0;
} 