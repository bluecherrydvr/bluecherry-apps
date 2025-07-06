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

#ifdef BC_HLS_WITH_SSL
#include <openssl/x509.h>
#include <openssl/ssl.h>

class hls_ssl
{
public:
    struct cert
    {
        int verify_flags = -1; //SSL_VERIFY_PEER;
        std::string cert_path;
        std::string key_path;
        std::string ca_path;
    };

    hls_ssl() {};
    ~hls_ssl() { shutdown(); }

    static void global_init();
    static void global_destroy();

    bool init_server(int sock, hls_ssl::cert *cert);
    bool get_peer_cert(std::string &subject, std::string &issuer);

    int ssl_read(uint8_t* buffer, int size, bool exact);
    int ssl_write(const uint8_t *buffer, int length);

    hls_ssl* accept_new(struct sockaddr_in *inaddr);
    void shutdown();

    SSL* get_ssl() { return _ssl; }
    void set_ssl(SSL *ssl) { _ssl = ssl; }
    void set_fd(int fd) { SSL_set_fd(_ssl, fd); _sock = fd; }
    int get_fd() { return _sock; }

    std::string get_last_error();
    std::string last_ssl_error();

protected:
    std::string     _last_error;
    SSL_CTX*        _ssl_ctx = NULL;
    SSL*            _ssl = NULL;
    int             _sock = -1;

    STACK_OF(X509)* _ca = NULL;
    EVP_PKEY*       _key = NULL;
    X509*           _cert = NULL;
};
#endif /* BC_HLS_WITH_SSL */

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

typedef int(*event_callback_t)(void *events, void* data, int reason);

class hls_events
{
public:

    typedef enum {
        listener = (int)0,
        session,
        fstream
    } type;

    typedef struct {
        hls_events::type type;  // Event type
        void *ptr;              // Data pointer
        int events;             // Ready events
        int fd;                 // Socket descriptor
    } event_data;

    ~hls_events();

    bool create(size_t max, void *userptr, event_callback_t callBack);
    hls_events::event_data* register_event(void *ctx, int fd, int events, hls_events::type type);

    bool add(hls_events::event_data* data, int events);
    bool modify(hls_events::event_data *data, int events);
    bool remove(hls_events::event_data *data);
    bool service(int timeout_ms);

    void service_callback(hls_events::event_data *data);
    void clear_callback(hls_events::event_data *data);
    void *get_user_data() { return user_data; }

private:
    event_callback_t        event_callback = NULL;  /* Service callback */
    struct epoll_event*     event_array = NULL;     /* EPOLL event array */
    void*                   user_data = NULL;       /* User data pointer */
    uint32_t                events_max = 0;         /* Max allowed file descriptors */
    int                     event_fd = -1;          /* EPOLL File decriptor */
};

class hls_byte_buffer
{
public:
    ~hls_byte_buffer() { this->clear(); }

    size_t append(uint8_t *data, size_t size);
    size_t erase(size_t posit, size_t size);
    size_t advance(size_t size);
    size_t resize(size_t size);
    void clear();

    uint8_t* data() { return _data; }
    size_t size() { return _size; }
    size_t used() { return _used; }

private:
    uint8_t *_data = NULL;
    size_t _size = 0;
    size_t _used = 0;
};

class hls_filestream 
{
public:
    ~hls_filestream() { finish(); }
    void finish();

    bool open_file(const char *path);
    ssize_t read_data(uint8_t *data, size_t size);
    size_t get_size() { return _size; }
    int get_fd() { return _fd; }

    bool is_active() { return (_fd >= 0); }
    bool eof_reached() { return _eof; }

private:
    size_t  _size = 0;
    size_t  _read = 0;
    bool    _eof = false;
    int     _fd = -1;
};

// Forward declaration
class hls_listener;

class hls_session
{
public:

    enum request_type
    {
        invalid = 0,
        unauthorized,
        rec_playlist,
        recording,
        playlist,
        initial,
        payload,
        index
    };

    ~hls_session();

    int writeable();

    std::string get_request();
    bool authenticate(const std::string &uri, const std::string &request);
    bool handle_request(const std::string &request);
    bool create_response();

    hls_byte_buffer& tx_buffer_get() { return _tx_buffer; }
    void tx_buffer_append(uint8_t *data, size_t size);
    size_t tx_buffer_advance(size_t size);
    ssize_t tx_buffer_flush();

    void rx_buffer_append(const char *data) { _rx_buffer.append(data); }
    void rx_buffer_advance(size_t size) { _rx_buffer.erase(0, size); }

    void set_hls_event_data(hls_events::event_data *data) { _ev_data = data; }
    void set_event_handler(hls_events *events) { _events = events; }
    hls_events* get_event_handler() { return _events; }

    void set_listener(hls_listener *listener) { _listener = listener; } 
    hls_listener* get_listener() { return _listener; }

    const char *get_addr() { return _address.c_str(); }
    void set_addr(const struct in_addr addr);
    void set_fd(int fd) { _fd = fd; }
    int get_fd() { return _fd; }

    void set_fstream(hls_filestream *fstream) { _fstream = fstream; } 
    hls_filestream* get_fstream() { return _fstream; } 

#ifdef BC_HLS_WITH_SSL
    void set_ssl(hls_ssl* ssl) { _ssl = ssl; }
    hls_ssl* get_ssl() { return _ssl; }
#endif

private:
    /* Request */
    std::string     _recording;
    std::string     _auth_token;
    request_type    _type = invalid;
    uint32_t        _segment_id = 0;
    uint8_t         _stream_id = 0;
    int             _device_id = 0;

    /* Objects */
    hls_events::event_data* _ev_data = NULL;
    hls_filestream*         _fstream = NULL;
    hls_listener*           _listener = NULL;
    hls_events*             _events = NULL;

#ifdef BC_HLS_WITH_SSL
    /* SSL support */
    hls_ssl*         _ssl = NULL;
#endif

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

    enum class type {
        mpegts,
        fmp4
    };

    struct meta {
        hls_segment::type   type = type::mpegts;
        uint32_t            id = 0;
        int64_t             pts = 0;
        double              duration = 0;
        bool                is_last = false;
        bool                is_key = false;
    };

    bool add_data(const uint8_t *data, size_t size);
    uint8_t* data() { return _data; }
    size_t size() { return _size; }
    hls_segment* copy();

    hls_segment::meta   _meta;

private:
    uint8_t*            _data = NULL;
    size_t              _size = 0;
};

typedef std::deque<hls_segment*> hls_window;
typedef std::vector<uint32_t> hls_segments;

typedef struct {
    size_t window_size;
    size_t segment_size;
    double segment_duration;
} hls_config;

class hls_content
{
public:
    hls_content(hls_config *cfg);
    ~hls_content();

    bool add_data(uint8_t *data, size_t size, int64_t pts, hls_segment::type type, int flags);
    bool append_segment(hls_segment *segment);
    bool finish_segment(int64_t pts);
    bool clear_window();

    size_t get_window_size() { return _config.window_size; };
    void get_config(size_t *segment_size, double *segment_duration);
    void set_config(hls_config *config);

    hls_segment *get_segment(uint32_t id);
    size_t get_segment_ids(hls_segments &ids);

    bool have_initial_segment() { return _use_initial ? true : false; }
    void use_initial_segment(bool use) { _use_initial = use; }
    void set_initial_segment(hls_segment *segment);
    hls_segment* get_initial_segment();

    void set_id(int id) { _device_id = id; }
    void update_pts(int64_t pts) { _pts = pts; };
    int64_t get_last_pts() { return _pts; };

    pthread_mutex_t     _mutex;
    hls_window          _window;
    bool                _fmp4 = false;
    bool                _init = false;

    bool                _append_criteria = false;
    hls_segment::meta   _meta;

private:
    hls_byte_buffer     _in_buffer;
    hls_config          _config;

    bool                _use_initial = false;
    hls_segment*        _init_segment = NULL;
    
    int                 _device_id = 0;
    int64_t             _pts = 0;
    uint32_t            _cc = 0;

    // Helper functions for mutex operations
    bool lock_mutex() {
        int ret = pthread_mutex_lock(&_mutex);
        if (ret != 0) {
            bc_log(Error, "Failed to lock HLS content mutex: %s", strerror(ret));
            return false;
        }
        return true;
    }

    bool unlock_mutex() {
        int ret = pthread_mutex_unlock(&_mutex);
        if (ret != 0) {
            bc_log(Error, "Failed to unlock HLS content mutex: %s", strerror(ret));
            return false;
        }
        return true;
    }
};

typedef std::unordered_map<int, hls_content*> hls_content_map;
typedef std::unordered_map<int, hls_content*>::iterator hls_content_it;

class hls_listener
{
public:
    hls_listener();
    ~hls_listener();

    void run();
    void reconfigure(hls_config *config);
    bool register_listener(uint16_t port);
    hls_content *get_hls_content(int id);

    void set_ssl_ctx(const char *key, const char* crt, const char *ca);
    bool ssl_service();

    void set_auth(bool auth) { _auth = auth; }
    bool get_auth() { return _auth; }

private:
    bool create_socket(uint16_t port);

    /* HLS window buffer */
    hls_content_map  _content;
    pthread_mutex_t  _mutex;

    /* SSL configuration */
#ifdef BC_HLS_WITH_SSL
    hls_ssl::cert   _ssl_cert;
    hls_ssl         _ssl;
#endif
    bool            _use_ssl = false;

    /* HLS listener context */
    hls_config      _config;
    hls_events      _events;
    uint16_t        _port = 0;
    int             _fd = -1;
    bool            _auth = false;
    bool            _init = false;
};

#endif /* __HLS_H */ 