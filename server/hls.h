/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __HLS_H
#define __HLS_H

#include <sys/epoll.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <deque>

#define HLS_REQUEST_MAX             4096
#define HLS_EVENTS_MAX              120000

#define HLS_EVENT_ERROR             1
#define HLS_EVENT_USER              2
#define HLS_EVENT_READ              3
#define HLS_EVENT_WRITE             4
#define HLS_EVENT_HUNGED            5
#define HLS_EVENT_CLOSED            6
#define HLS_EVENT_CLEAR             7
#define HLS_EVENT_DESTROY           8
#define HLS_EVENT_EXCEPTION         9

typedef struct hls_event_data_ {
    void *ptr;      // Data pointer
    int events;     // Ready events
    int type;       // Event type
    int fd;         // Socket descriptor
} hls_event_data;

typedef int(*event_callback_t)(void *events, void* data, int reason);

class hls_events
{
public:
    ~hls_events();

    bool create(size_t max, void *userptr, event_callback_t callBack);
    hls_event_data* register_event(void *ctx, int fd, int events, int type);

    bool add(hls_event_data* data, int events);
    bool modify(hls_event_data *data, int events);
    bool remove(hls_event_data *data);
    bool service(int timeout_ms);

    void service_callback(hls_event_data *data);
    void clear_callback(hls_event_data *data);
    void *get_user_data() { return user_data; }

private:
    event_callback_t        event_callback = NULL;  /* Service callback */
    struct epoll_event*     event_array = NULL;     /* EPOLL event array */
    void*                   user_data = NULL;       /* User data pointer */
    uint32_t                events_max = 0;         /* Max allowed file descriptors */
    int                     event_fd = -1;          /* EPOLL File decriptor */
};

typedef std::vector<uint8_t> hls_byte_buffer;

// Forward declaration
class hls_listener;

class hls_session
{
public:

    enum request_type
    {
        invalid = 0,
        playlist,
        initial,
        payload,
        index
    };

    ~hls_session();

    int writeable();

    std::string get_request();
    bool handle_request(const std::string &request);
    bool create_response();

    void tx_buffer_append(uint8_t *data, size_t size);
    size_t tx_buffer_advance(size_t size);
    size_t tx_buffer_flush();

    void rx_buffer_append(const char *data) { _rx_buffer.append(data); }
    void rx_buffer_advance(size_t size) { _rx_buffer.erase(0, size); }

    void set_hls_event_data(hls_event_data *data) { _ev_data = data; }
    void set_event_handler(hls_events *events) { _events = events; }

    void set_listener(hls_listener *listener) { _listener = listener; } 
    hls_listener* get_listener() { return _listener; }

    const char *get_addr() { return _address.c_str(); }
    void set_addr(const struct in_addr addr);
    void set_fd(int fd) { _fd = fd; }
    int get_fd() { return _fd; }

private:
    /* Request */
    request_type    _type = invalid;
    uint32_t        _segment_id = 0;
    uint8_t         _stream_id = 0;
    int             _device_id = 0;

    /* Objects */
    hls_listener*   _listener = NULL;
    hls_event_data* _ev_data = NULL;
    hls_events*     _events = NULL;

    /* rx/tx */
    hls_byte_buffer _tx_buffer;
    std::string     _rx_buffer;
    std::string     _address;
    int             _fd = -1;
};

class hls_segment 
{
public:
    ~hls_segment()
    {
        free(_data);
        _data = NULL;
    }

    bool add_data(const uint8_t *data, size_t size);
    void set_duration(double duration) { _duration = duration; }
    void set_first(bool first) { _is_first = first; }
    void set_last(bool last) { _is_last = last; }
    void set_key(bool key) { _is_key = key; }
    void set_pts(int64_t pts) { _pts = pts; }
    void set_id(uint32_t id) { _id = id; }

    uint8_t* data() { return _data; }
    size_t size() { return _size; }

    double duration() { return _duration; }
    int64_t pts() { return _pts; }
    uint32_t id() { return _id; }

    bool is_first() { return _is_first; }
    bool is_last() { return _is_last; }
    bool is_key() { return _is_key; }

    hls_segment* copy();

private:
    uint8_t*    _data = NULL;
    size_t      _size = 0;
    uint32_t    _id = 0;
    int64_t     _pts = 0;
    double      _duration = 0;

    bool        _is_first = false;
    bool        _is_last = false;
    bool        _is_key = false;
};

typedef std::deque<hls_segment*> hls_window;
typedef std::vector<uint32_t> hls_segments;

class hls_content
{
public:
    hls_content();
    ~hls_content();

    void set_window_size(size_t size) { _window_size = size; }
    bool clear_window();

    bool add_data(uint8_t *data, size_t size, int64_t pts, int flags);
    bool append_segment(hls_segment *segment);

    hls_segment *get_segment(uint32_t id);
    size_t get_segment_ids(hls_segments &ids);

    bool have_initial_segment() { return _use_initial ? true : false; }
    void use_initial_segment(bool use) { _use_initial = use; }
    void set_initial_segment(hls_segment *segment);
    hls_segment* get_initial_segment();

    void set_id(int id) { _device_id = id; }
    void update_pts(int64_t pts) { _pts = pts; };
    int64_t get_last_pts() { return _pts; };

    pthread_mutex_t _mutex;
    hls_window      _window;
    bool            _init = false;

private:
    hls_byte_buffer _in_buffer;

    bool            _use_initial = false;
    hls_segment*    _init_segment = NULL;

    size_t          _window_size = 0;
    int             _device_id = 0;
    int64_t         _pts = 0;
    uint32_t        _cc = 0;
};

typedef std::unordered_map<int, hls_content*> hls_content_map;
typedef std::unordered_map<int, hls_content*>::iterator hls_content_it;

class hls_listener
{
public:
    hls_listener();
    ~hls_listener();

    void run();
    bool register_listener(uint16_t port);
    hls_content *get_hls_content(int id);

private:
    bool create_socket(uint16_t port);

    /* HLS window buffer */
    hls_content_map  _content;
    pthread_mutex_t  _mutex;

    /* HLS listener context */
    hls_events      _events;
    uint16_t        _port = 0;
    int             _fd = -1;
    bool            _init = false;
};

#endif /* __HLS_H */ 