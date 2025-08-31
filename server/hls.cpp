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

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <string>
#include <vector>

#include "libbluecherry.h"
#include "decoder.h"
#include "hls.h"

extern "C" {
#include <libavutil/avstring.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/base64.h>
}

#ifdef BC_HLS_WITH_SSL
#include <openssl/opensslv.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <netinet/in.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif /* BC_HLS_WITH_SSL */

#ifndef PTHREAD_MUTEX_RECURSIVE
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif

#define HLS_WINDOW_SIZE             5                       // Default HLS window size
#define HLS_SEGMENT_SIZE            (188 * 7 * 1024)        // Most accepted HLS segment size
#define HLS_SEGMENT_SIZE_MAX        (188 * 7 * 1024 * 5)    // Maximum allowed segment size
#define HLS_SEGMENT_DURATION        3.0                     // Most accepted HLS segment duration
#define HLS_HEADER_EXPIRE_SEC       2                       // HLS header expiration seconds
#define HLS_SERVER_CHUNK_MAX        65535                   // RX chunk size to send per one call

static void std_string_append(std::string &source, const char *data, ...)
{
    char buffer[4048];
    size_t length = 0;
    va_list args;

    va_start(args, data);
    length += vsnprintf(buffer, sizeof(buffer), data, args);
    va_end(args);

    source.append(buffer, length);
}

static int sock_shutdown(int fd)
{
    if (fd >= 0)
    {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }

    return -1;
}

//////////////////////////////////////////////////
// SSL SUPPORT
//////////////////////////////////////////////////

#ifdef BC_HLS_WITH_SSL
void hls_ssl::global_init()
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
#else
    OPENSSL_init_ssl(0, NULL);
#endif
}

void hls_ssl::global_destroy()
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    ENGINE_cleanup();
    ERR_free_strings();
#else
    EVP_PBE_cleanup();
#endif
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
}

bool hls_ssl::init_server(int sock, hls_ssl::cert *cert)
{
    if (sock < 0)
    {
        _last_error = "Invalid TCP socket";
        return false;
    }

    _ssl_ctx = SSL_CTX_new(SSLv23_server_method());
    if (_ssl_ctx == NULL)
    {
        _last_error = "Can not create server SSL context";
        this->shutdown();
        return false;
    }

    SSL_CTX_set_ecdh_auto(_ssl_ctx, 1);
    if (cert != NULL)
    {
        /* Note: If verify is enabled, client must send its sertificate */
        if (cert->verify_flags >= 0) SSL_CTX_set_verify(_ssl_ctx, cert->verify_flags, NULL);

        if (cert->ca_path.length())
        {
            /* Note: Tell the client what certificates to use for verification */
            if (SSL_CTX_load_verify_locations(_ssl_ctx, cert->ca_path.c_str(), NULL) <= 0)
            {
                _last_error = "Can not load root ca file (" + cert->ca_path + ")";
                this->shutdown();
                return false;
            }

            SSL_CTX_set_client_CA_list(_ssl_ctx, SSL_load_client_CA_file(cert->ca_path.c_str()));
        }

        if (cert->cert_path.length() && cert->key_path.length())
        {
            if (SSL_CTX_use_certificate_file(_ssl_ctx, cert->cert_path.c_str(), SSL_FILETYPE_PEM) <= 0 ||
                SSL_CTX_use_PrivateKey_file(_ssl_ctx, cert->key_path.c_str(), SSL_FILETYPE_PEM) <= 0)
            {
                _last_error = "Failet to setup SSL cert/key";
                this->shutdown();
                return false;
            }
        }
    }

    _sock = sock;
    return true;
}

void hls_ssl::shutdown()
{
    if (_ssl)
    {
        SSL_shutdown(_ssl);
        SSL_free(_ssl);
        _ssl = NULL;
    }

    if (_ssl_ctx)
    {
        SSL_CTX_free(_ssl_ctx);
        _ssl_ctx = NULL;
    }

    if (_sock >= 0)
    {
        close(_sock);
        _sock = -1;
    }
}

std::string hls_ssl::last_ssl_error()
{
    BIO *bio = BIO_new(BIO_s_mem());
    ERR_print_errors(bio);

    char *err_buff = NULL;
    size_t len = BIO_get_mem_data(bio, &err_buff);
    if (!len) return std::string(strerror(errno));

    std::string errstr = std::string("\n");
    errstr.append(err_buff, len - 1);

    BIO_free(bio);
    return errstr;
}

std::string hls_ssl::get_last_error()
{
    std::string errstr = _last_error;
    errstr.append(": " + last_ssl_error());
    return errstr;
}

hls_ssl* hls_ssl::accept_new(struct sockaddr_in *inaddr)
{
    hls_ssl *client_ssl = new hls_ssl;
    socklen_t len = sizeof(struct sockaddr_in);

    int sock = accept(_sock, (struct sockaddr*)inaddr, &len);
    if (sock < 0)
    {
        _last_error = "Can not accept to the HLS (SSL) socket";
        delete client_ssl;
        return NULL;
    }

    client_ssl->set_ssl(SSL_new(_ssl_ctx));
    client_ssl->set_fd(sock);

    if (SSL_accept(client_ssl->get_ssl()) <= 0)
    {
        _last_error = "SSL_accept failed";
        delete client_ssl;
        return NULL;
    }

    return client_ssl;
}

int hls_ssl::ssl_read(uint8_t *buffer, int size, bool exact)
{
    if (_ssl == NULL) return false;
    int received = 0;
    int left = size;

    while ((left > 0 && exact) || !received)
    {
        int bytes = SSL_read(_ssl, &buffer[received], left);
        if (bytes <= 0 && SSL_get_error(_ssl, bytes) != SSL_ERROR_WANT_READ)
        {
            if (!bytes) _last_error = "Disconnected SSL client";
            else _last_error = "SSL_read failed";

            this->shutdown();
            return bytes;
        }

        received += bytes;
        left -= bytes;
    }

    buffer[received] = 0;
    return received;
}

int hls_ssl::ssl_write(const uint8_t *buffer, int length)
{
    if (_ssl == NULL) return false;
    int left = length;
    int sent = 0;

    while (left > 0)
    {
        int bytes = SSL_write(_ssl, &buffer[sent], left);
        if (bytes <= 0 && SSL_get_error(_ssl, bytes) != SSL_ERROR_WANT_WRITE)
        {
            _last_error = "SSL_write failed";
            this->shutdown();
            return bytes;
        }

        sent += bytes;
        left -= bytes;
    }

    return sent;
}

bool hls_ssl::get_peer_cert(std::string &subject, std::string &issuer)
{
    X509 *cert = SSL_get_peer_certificate(_ssl);
    if (cert == NULL) return false;

    char *line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    if (line != NULL)
    {
        subject = std::string(line);
        delete line;
    }

    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    if (line != NULL)
    {
        issuer = std::string(line);
        delete line;
    }

    X509_free(cert);
    return true;
}
#endif /* BC_HLS_WITH_SSL */

//////////////////////////////////////////////////
// HLS EVENT HANDLER
//////////////////////////////////////////////////

hls_events::~hls_events()
{
    if (event_fd >= 0)
    {
        close(event_fd);
        event_fd = -1;
    }

    if (event_array)
    {
        free(event_array);
        event_array = NULL;
    }

    if (event_callback)
        event_callback(this, NULL, HLS_EVENT_DESTROY);
}

bool hls_events::create(size_t max, void *userptr, event_callback_t callback)
{
    events_max = max > 0 ? max : HLS_EVENTS_MAX;
    event_callback = callback;
    user_data = userptr;

    /* Allocate memory for event array */
    event_array = (struct epoll_event*)calloc(events_max, sizeof(struct epoll_event));
    if (event_array == NULL)
    {
        bc_log(Error, "Can not allocate memory for event array: %s", strerror(errno));
        return false;
    }

    /* Create event epoll instance */
    event_fd = epoll_create1(0);
    if (event_fd < 0)
    {
        bc_log(Error, "Can not crerate epoll: %s", strerror(errno));
        free(event_array);
        event_array = NULL;
        return false;
    }

    return true;
}

void hls_events::service_callback(hls_events::event_data *data)
{
    /* Check error condition */
    if (data->events & EPOLLRDHUP) { event_callback(this, data, HLS_EVENT_CLOSED); return; }
    if (data->events & EPOLLHUP) { event_callback(this, data, HLS_EVENT_HUNGED); return; }
    if (data->events & EPOLLERR) { event_callback(this, data, HLS_EVENT_ERROR); return; }
    if (data->events & EPOLLPRI) { event_callback(this, data, HLS_EVENT_EXCEPTION); return; }

    /* Callback on writeable */
    if (data->events & EPOLLOUT && event_callback(this, data, HLS_EVENT_WRITE) < 0)
        { event_callback(this, data, HLS_EVENT_USER); return; } // User requested callback

    /* Callback on readable */
    if (data->events & EPOLLIN && event_callback(this, data, HLS_EVENT_READ) < 0)
        { event_callback(this, data, HLS_EVENT_USER); return; } // User requested callback
}

void hls_events::clear_callback(hls_events::event_data *data)
{
    if (data != NULL)
    {
        event_callback(this, data, HLS_EVENT_CLEAR);
        free(data);
    }
}

hls_events::event_data* hls_events::register_event(void *ctx, int fd, int events, hls_events::type type)
{
    /* Allocate memory for event data */
    hls_events::event_data* data = (hls_events::event_data*)malloc(sizeof(hls_events::event_data));
    if (data == NULL)
    {
        bc_log(Error, "Can not allocate memory for event: %s", strerror(errno));
        return NULL;
    }

    /* Initialize event */
    data->events = 0;
    data->type = type;
    data->ptr = ctx;
    data->fd = fd;

    /* Add event to the instance */
    if (!this->add(data, events))
    {
        free(data);
        return NULL;
    }

    return data;
}

bool hls_events::add(hls_events::event_data* data, int events)
{
    struct epoll_event event;
    event.data.ptr = data;
    event.events = events;

    if (epoll_ctl(event_fd, EPOLL_CTL_ADD, data->fd, &event) < 0)
    {
        bc_log(Error, "epoll_ctl() add failed: %s", strerror(errno));
        return false;
    }

    return true;
}

bool hls_events::modify(hls_events::event_data *data, int events)
{
    struct epoll_event event;
    event.data.ptr = data;
    event.events = events;

    if (epoll_ctl(event_fd, EPOLL_CTL_MOD, data->fd, &event) < 0)
    {
        bc_log(Error, "epoll_ctl() mod failed: %s", strerror(errno));
        return false;
    }

    return true;
}

bool hls_events::remove(hls_events::event_data *data)
{
    bool status = true;
    if (data->fd >= 0 && epoll_ctl(event_fd, EPOLL_CTL_DEL, data->fd, NULL) < 0) status = false;
    clear_callback(data);
    return status;
}

bool hls_events::service(int timeout_ms)
{
    int count; /* Wait for ready events */
    do count = epoll_wait(event_fd, event_array, events_max, timeout_ms);
    while (errno == EINTR);

    for (int i = 0; i < count; i++)
    {
        /* Call callback for each ready event */
        hls_events::event_data *data = (hls_events::event_data*)event_array[i].data.ptr;
        data->events = event_array[i].events;
        service_callback(data);
    }

    return (count < 0) ? false : true;
}

//////////////////////////////////////////////////
// HLS SEGMENT
//////////////////////////////////////////////////

bool hls_segment::add_data(const uint8_t *data, size_t size)
{
    if (_data != NULL)
    {
        bc_log(Error, "HLS segment is already allocated");
        return false;
    }

    _data = (uint8_t*)malloc(size);
    if (_data == NULL)
    {
        bc_log(Error, "Can not allocate data for HLS segment");
        return false;
    }

    memcpy(_data, data, size);
    _size = size;
    return true;
}

hls_segment* hls_segment::copy()
{
    hls_segment *segment = new hls_segment;
    segment->_meta = this->_meta;

    if (!segment->add_data(_data, _size))
    {
        delete segment;
        return NULL;
    }

    return segment;
}

//////////////////////////////////////////////////////
// HLS DATE
//////////////////////////////////////////////////////

class hls_date 
{
public:
    hls_date();

    void to_tm(struct tm *tm);
    void from_tm(const struct tm *tm);

    time_t to_epoch();
    void to_http(char *dst, size_t size);
    void make();
    void inc_sec(int sec);

    int _year; 
    int _month; 
    int _day;
    int _hour;
    int _min;
    int _sec;
};

hls_date::hls_date()
{
    struct tm timeinfo;
    time_t rawtime = time(NULL);
    localtime_r(&rawtime, &timeinfo);
    this->from_tm(&timeinfo);
}

void hls_date::to_tm(struct tm *tm)
{
    tm->tm_year = _year - 1900;
    tm->tm_mon = _month - 1;
    tm->tm_mday = _day;
    tm->tm_hour = _hour;
    tm->tm_min = _min;
    tm->tm_sec = _sec;
    tm->tm_isdst = -1;  
}

void hls_date::from_tm(const struct tm *tm) 
{
    _year = tm->tm_year+1900;
    _month = tm->tm_mon+1;
    _day = tm->tm_mday;
    _hour = tm->tm_hour;
    _min = tm->tm_min;
    _sec = tm->tm_sec;
}

time_t hls_date::to_epoch()
{
    struct tm tminf;
    to_tm(&tminf);
    return mktime(&tminf);
}

void hls_date::to_http(char *dst, size_t size)
{
    struct tm timeinfo;
    time_t raw_time = to_epoch();
    gmtime_r(&raw_time, &timeinfo);
    strftime(dst, size, "%a, %d %b %G %H:%M:%S GMT", &timeinfo);
}

void hls_date::make()
{
    struct tm timeinfo;
    to_tm(&timeinfo);
    mktime(&timeinfo);
    from_tm(&timeinfo);
}

void hls_date::inc_sec(int sec)
{
    _sec += sec;
    this->make();
}

//////////////////////////////////////////////////
// HLS SESSION
//////////////////////////////////////////////////

hls_session::~hls_session()
{
#ifdef BC_HLS_WITH_SSL
    if (_ssl != NULL)
    {
        delete _ssl;
        _ssl = NULL;
    }
#endif

    if (_fstream != NULL)
    {
        delete _fstream;
        _fstream = NULL;
    }

    _fd = sock_shutdown(_fd);
}

int hls_session::writeable()
{
    int events = EPOLLRDHUP | EPOLLOUT;
    return _events->modify(_ev_data, events) ? 1 : -1;
}

void hls_session::set_addr(const struct in_addr addr)
{
    char address[64];

    /* Get IP string from network byte order */
    int length = snprintf(address, 
        sizeof(address), "%d.%d.%d.%d",
        (int)((addr.s_addr & 0x000000FF)),
        (int)((addr.s_addr & 0x0000FF00)>>8),
        (int)((addr.s_addr & 0x00FF0000)>>16),
        (int)((addr.s_addr & 0xFF000000)>>24));

    _address = std::string(address, length);
}

std::string hls_session::get_request() 
{
    std::string request;
    request.clear(); // for dumb compilers

    size_t posit = _rx_buffer.find("\r\n\r\n");
    if (posit != std::string::npos)
    {
        request = _rx_buffer.substr(0, posit + 4);
        rx_buffer_advance(request.length());
    }

    return request;
}

void hls_session::tx_buffer_append(uint8_t *data, size_t size) 
{
    _tx_buffer.append(data, size);
}

ssize_t hls_session::tx_buffer_flush()
{
    if (!_tx_buffer.used()) return 0;

    ssize_t sent = send(_fd, _tx_buffer.data(), _tx_buffer.used(), MSG_NOSIGNAL);
    if (sent < 0)
    {
        bc_log(Error, "Can not send data to HLS client: %s (%s)", get_addr(), strerror(errno));
        return sent;
    }

    _tx_buffer.advance(sent);
    return _tx_buffer.used();
}

bool hls_session::create_response()
{
    if (_type == hls_session::request_type::unauthorized) 
    {
        bc_log(Debug, "Authorization failure for HLS playback for device %d", _device_id);
        std::string response = std::string("HTTP/1.1 401 Unauthorized\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s\r\n", "*");
        std_string_append(response, "WWW-Authenticate: Basic realm=\"HLS Server\"\r\n");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Length: 0\r\n\r\n");

        _tx_buffer.append((uint8_t*)response.c_str(), response.length());
        return true;
    }
    else if (_type == hls_session::request_type::invalid)
    {
        bc_log(Warning, "Invalid HLS request for device: %d id(%u)", _device_id, _segment_id);
        std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Length: 0\r\n\r\n");

        _tx_buffer.append((uint8_t*)response.c_str(), response.length());
        return true;
    }
    else if (_type == hls_session::request_type::index)
    {
        std::string body;
        std_string_append(body, "#EXTM3U\n");
        std_string_append(body, "#EXT-X-INDEPENDENT-SEGMENTS\n");
        std_string_append(body, "#EXT-X-STREAM-INF:BANDWIDTH=%d\n", 90000);
        std_string_append(body, "0/playlist.m3u8");

        if (!get_listener()->get_auth() || !_auth_token.length()) std_string_append(body, "\n");
        else std_string_append(body, "?authtoken=%s\n", _auth_token.c_str());

        std::string response = std::string("HTTP/1.1 200 OK\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s\r\n", "*");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: %s\r\n", "application/vnd.apple.mpegurl");
        std_string_append(response, "Cache-Control: %s\r\n", "no-cache");
        std_string_append(response, "Content-Length: %zu\r\n\r\n", body.length());
        response.append(body);

        _tx_buffer.append((uint8_t*)response.c_str(), response.length());
        return true;
    }
    else if (_type == hls_session::request_type::playlist)
    {
        hls_content *content = _listener->get_hls_content(_device_id);
        if (content == NULL)
        {
            bc_log(Warning, "HLS content not found for requested device: %d id(%u)", _device_id, _segment_id);
            std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");

            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        if (pthread_mutex_lock(&content->_mutex))
        {
            bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
            // CRITICAL FIX: Return proper error response instead of false
            std::string response = std::string("HTTP/1.1 503 Service Unavailable\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        size_t window_size = content->_window.size();
        size_t window_max = content->get_window_size();

        uint32_t sequence = window_size ? content->_window[0]->_meta.id : 0;
        double duration = window_size ? content->_window[0]->_meta.duration : 0.;
        const char* segment_extension = content->_fmp4 ? "m4s" : "ts";

        std::string body;
        std_string_append(body, "#EXTM3U\n");
        std_string_append(body, "#EXT-X-VERSION: 3\n");
        std_string_append(body, "#EXT-X-ALLOW-CACHE: NO\n");
        std_string_append(body, "#EXT-X-TARGETDURATION: %.f\n", duration + 1);
        std_string_append(body, "#EXT-X-MEDIA-SEQUENCE: %u\n", sequence);

        if (content->_fmp4 && content->have_initial_segment())
        {
            std_string_append(body, "#EXT-X-INDEPENDENT-SEGMENTS\n");
            std_string_append(body, "#EXT-X-MAP:URI=\"init.mp4\"\n");
        }

        if (window_size >= window_max)
        {
            for (size_t i = 0; i < window_size; i++)
            {
                std_string_append(body, "#EXTINF:%f,\n", content->_window[i]->_meta.duration);
                std_string_append(body, "%u/payload.%s\n", content->_window[i]->_meta.id, segment_extension);
            }
        }

        if (pthread_mutex_unlock(&content->_mutex))
        {
            bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
            return false;
        }

        hls_date date;
        char curr_date[128], exp_date[128];
        date.to_http(curr_date, sizeof(curr_date));
        date.inc_sec(HLS_HEADER_EXPIRE_SEC);
        date.to_http(exp_date, sizeof(exp_date));

        std::string response = std::string("HTTP/1.1 200 OK\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s\r\n", "*");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: %s\r\n", "application/vnd.apple.mpegurl");
        std_string_append(response, "Cache-Control: %s\r\n", "no-cache");
        std_string_append(response, "Date: %s\r\n", curr_date);
        std_string_append(response, "Expires: %s\r\n", exp_date);
        std_string_append(response, "Content-Length: %zu\r\n\r\n", body.length());
        response.append(body);

        _tx_buffer.append((uint8_t*)response.c_str(), response.length());
        return true;
    }
    else if (_type == hls_session::request_type::rec_playlist)
    {
        struct stat buffer;
        if (lstat(_recording.c_str(), &buffer) < 0)
        {
            bc_log(Warning, "Requested HLS recording is not found: %s", _recording.c_str());
            std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");

            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        AVFormatContext* pFormatCtx = avformat_alloc_context();
        if (pFormatCtx == NULL)
        {
            bc_log(Error, "Failed to allocate av format context for recording: %s", _recording.c_str());
            std::string response = std::string("HTTP/1.1 500 Internal server error\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");

            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        int ret = avformat_open_input(&pFormatCtx, _recording.c_str(), NULL, NULL);
        if (ret != 0)
        {
            bc_log(Error, "HLS Failed to open input: %s", _recording.c_str());
            std::string response = std::string("HTTP/1.1 500 Internal server error\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());

            avformat_free_context(pFormatCtx);
            return true;
        }

        ret = avformat_find_stream_info(pFormatCtx,NULL);
        if (ret < 0)
        {
            bc_log(Error, "HLS Failed analyze file: %s", _recording.c_str());
            std::string response = std::string("HTTP/1.1 500 Internal server error\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());

            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
            return true;
        }

        double duration = (double)pFormatCtx->duration / (double)1000000;
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);

        std::string body;
        std_string_append(body, "#EXTM3U\n");
        std_string_append(body, "#EXT-X-VERSION: 3\n");
        std_string_append(body, "#EXT-X-TARGETDURATION: %.f\n", duration + 1);
        std_string_append(body, "#EXT-X-MEDIA-SEQUENCE: %u\n", 0);
        std_string_append(body, "#EXT-X-PLAYLIST-TYPE:VOD\n");

        std_string_append(body, "#EXTINF:%f,\n", duration);
        std_string_append(body, "%s", _recording.c_str());
        if (!get_listener()->get_auth() || !_auth_token.length()) std_string_append(body, "\n");
        else std_string_append(body, "?authtoken=%s\n", _auth_token.c_str());
        std_string_append(body, "#EXT-X-ENDLIST\n");

        std::string response = std::string("HTTP/1.1 200 OK\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s\r\n", "*");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: %s\r\n", "application/vnd.apple.mpegurl");
        std_string_append(response, "Cache-Control: %s\r\n", "no-cache");
        std_string_append(response, "Content-Length: %zu\r\n\r\n", body.length());
        response.append(body);

        _tx_buffer.append((uint8_t*)response.c_str(), response.length());
        return true;
    }
    else if (_type == hls_session::request_type::initial)
    {
        hls_content *content = _listener->get_hls_content(_device_id);
        if (content == NULL)
        {
            bc_log(Warning, "HLS content not found for requested device: %d id(%u)", _device_id, _segment_id);
            std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        if (pthread_mutex_lock(&content->_mutex))
        {
            bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
            return false;
        }

        const char* content_type = content->_fmp4 ? "mp4" : "MP2T";
        hls_segment *segment = content->get_initial_segment();

        if (pthread_mutex_unlock(&content->_mutex))
        {
            bc_log(Error, "Can not unlock pthread mutex: %s", strerror(errno));
            // CRITICAL FIX: Return proper error response instead of false
            std::string response = std::string("HTTP/1.1 503 Service Unavailable\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        if (segment == NULL)
        {
            bc_log(Warning, "HLS initial payload not found for requested device: %d id(%u)", _device_id, _segment_id);
            std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        std::string response = std::string("HTTP/1.1 206 Partial Content\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s\r\n", "*");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: video/%s\r\n", content_type);
        std_string_append(response, "Cache-Control: no-cache\r\n");
        std_string_append(response, "Accept-Ranges: bytes\r\n");
        std_string_append(response, "Connection: close\r\n");
        std_string_append(response, "Server: bluechery\r\n");
        std_string_append(response, "Content-Length: %zu\r\n\r\n", segment->size());

        _tx_buffer.append((uint8_t*)response.c_str(), response.length());
        _tx_buffer.append(segment->data(), segment->size());

        delete segment;
        return true;
    }
    else if (_type == hls_session::request_type::payload)
    {
        hls_content *content = _listener->get_hls_content(_device_id);
        if (content == NULL)
        {
            bc_log(Warning, "HLS content not found for requested device: %d id(%u)", _device_id, _segment_id);
            std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        hls_segment *segment = content->get_segment(_segment_id);
        if (segment == NULL)
        {
            bc_log(Warning, "HLS payload not found for requested device: %d id(%u)", _device_id, _segment_id);
            std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        const char* content_type = content->_fmp4 ? "mp4" : "MP2T";
        std::string response = std::string("HTTP/1.1 206 Partial Content\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s\r\n", "*");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: video/%s\r\n", content_type);
        std_string_append(response, "Cache-Control: no-cache\r\n");
        std_string_append(response, "Accept-Ranges: bytes\r\n");
        std_string_append(response, "Connection: close\r\n");
        std_string_append(response, "Server: bluechery\r\n");
        std_string_append(response, "Content-Length: %zu\r\n\r\n", segment->size());

        _tx_buffer.append((uint8_t*)response.c_str(), response.length());
        _tx_buffer.append(segment->data(), segment->size());

        delete segment;
        return true;
    }
    else if (_type == hls_session::request_type::recording)
    {
        struct stat buffer;
        if (lstat(_recording.c_str(), &buffer) < 0)
        {
            bc_log(Warning, "Requested HLS recording is not found: %s", _recording.c_str());
            std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());
            return true;
        }

        hls_filestream *fstream = new hls_filestream;
        if (!fstream->open_file(_recording.c_str()))
        {
            bc_log(Error, "HLS Failed to open recording: %s (%s)", _recording.c_str(), strerror(errno));
            std::string response = std::string("HTTP/1.1 500 Internal server error\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");
            _tx_buffer.append((uint8_t*)response.c_str(), response.length());

            delete fstream;
            return true;
        }

        std::string response = std::string("HTTP/1.1 200 OK\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s\r\n", "*");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: video/mp4\r\n");
        std_string_append(response, "Cache-Control: no-cache\r\n");
        std_string_append(response, "Accept-Ranges: bytes\r\n");
        std_string_append(response, "Connection: close\r\n");
        std_string_append(response, "Server: bluechery\r\n");
        std_string_append(response, "Content-Length: %ld\r\n\r\n", fstream->get_size());
        _tx_buffer.append((uint8_t*)response.c_str(), response.length());

        set_fstream(fstream);
        return true;
    }

    return false;
}

bool hls_session::authenticate(const std::string &uri, const std::string &request)
{
    if (_type == request_type::payload ||
        !get_listener()->get_auth()) return true;

    if (uri.find("authtoken=") != std::string::npos)
    {
        std::string token = uri.substr(uri.find("authtoken=") + 10);
        token = token.substr(0, token.find('/'));  // Drop possible trailing '/streamid=0' etc.
        BC_DB_RES dbres = bc_db_get_table("SELECT * FROM HlsAuthTokens JOIN Users ON "
            "HlsAuthTokens.user_id = Users.id WHERE HlsAuthTokens.token='%s'", token.c_str());

        if (dbres)
        {
            if (!bc_db_fetch_row(dbres)) 
            {
                bc_db_free_table(dbres);
                _auth_token = token;
                return true;
            }

            bc_db_free_table(dbres);
        }
    }

    size_t posit = request.find("Authorization");
    if (posit == std::string::npos)
    {
        /* Try lowercase (lazy case sensitivity) */
        posit = request.find("authorization");
        if (posit == std::string::npos) return false;
    }

    posit = request.find(":", posit + 13) + 1;
    if (posit == std::string::npos) return false;

    size_t end_posit = request.find("\r\n", posit);
    if (end_posit == std::string::npos) return false;

    std::string auth = request.substr(posit, end_posit - posit);
    while (auth.length() && auth.at(0) == ' ') auth.erase(0, 1);
    if (!auth.length()) return false;

    if (auth.size() > 6 && auth.substr(0, 6) == "Basic ")
    {
        auth = auth.substr(6);
        char *buf = new char[auth.size()];
        int ret = av_base64_decode((uint8_t*)buf, auth.c_str(), auth.size());

        if (ret > 0)
        {
            buf[ret] = 0;
            char *password = buf;
            char *username = strsep(&password, ":");

            if (username && password &&
                bc_user_auth(username, password, ACCESS_REMOTE, _device_id) == 1)
            {
                delete[] buf;
                return true;
            }
        }

        delete[] buf;
    }

    return false;
}

bool hls_session::handle_request(const std::string &request)
{
    /* Get start position of URL */
    size_t start_posit = request.find("/hls");
    if (start_posit == std::string::npos)
    {
        start_posit = request.find("/");
        if (start_posit == std::string::npos) return false;
    }
    else start_posit += 4; // skip /hls

    /* Get end position of URL */
    size_t end_posit = request.find(" ", start_posit);
    if (end_posit == std::string::npos) return false;

    /* Calculate URL length */
    size_t url_length = end_posit - start_posit;
    std::string url = request.substr(start_posit, url_length);

    if (url.find("/index.m3u8") != std::string::npos)
    {
        size_t posit = url.find("recording=");
        if (posit != std::string::npos)
        {
            /* Requested recording playlist */
            posit += 10; // length of recording=
            _recording = url.substr(posit, std::string::npos);
            if (!_recording.length()) return create_response();
            _type = request_type::rec_playlist;
        }
        else
        {
            /* Requested live view index */
            _type = request_type::index;
        }
    }
    else if (url.find("/init.mp4") != std::string::npos)
    {
        /* Requested live view initial segment */
        _type = request_type::initial;
    }
    else if (url.find("/playlist.m3u8") != std::string::npos)
    {
        /* Requested live view playlist */
        size_t posit = url.find("/", 1);
        if (posit == std::string::npos) return create_response();

        size_t offset = posit + 1;
        posit = url.find("/", offset);
        if (posit == std::string::npos) return create_response();
        _stream_id = std::stoi(url.substr(offset, posit));
        _type = request_type::playlist;
    }
    else if ((url.find("/payload.m4s") != std::string::npos) ||
            (url.find("/payload.ts") != std::string::npos))
    {
        /* Requested live view payload */
        size_t posit = url.find("/", 1);
        if (posit == std::string::npos) return create_response();

        size_t offset = posit + 1;
        posit = url.find("/", offset);
        if (posit == std::string::npos) return create_response();
        _stream_id = std::stoi(url.substr(offset, posit));

        posit++;
        offset = url.find("/", posit);
        if (offset == std::string::npos) return create_response();

        size_t length = offset - posit;
        _segment_id = std::stoi(url.substr(posit, length));
        _type = request_type::payload;
    }
    else if (url.find(".mp4") != std::string::npos)
    {
        /* Requested recording file */
        size_t posit = url.find("?", 0);
        if (posit == std::string::npos) _recording = url;
        else _recording = url.substr(0, posit);
        _type = request_type::recording;
    }

    if (_type != request_type::rec_playlist &&
        _type != request_type::recording &&
        _type != request_type::invalid)
    {
        size_t posit = url.find("/", 1);
        if (posit == std::string::npos) return create_response();
        _device_id = std::stoi(url.substr(1, url.find("/", 1)));
    }

    /* Authenticate request */
    if (!authenticate(url, request))
    {
        _type = request_type::unauthorized;
        return create_response();
    }

    /* At this condition, live view request is valid */
    return create_response();
}

#ifdef BC_HLS_WITH_SSL
static void* hls_session_thread(void *ctx)
{
    pthread_setname_np(pthread_self(), "HLS_SESSION");
    hls_session *session = (hls_session*)ctx;
    hls_ssl *ssl = session->get_ssl();
    uint8_t rx_buffer[HLS_REQUEST_MAX];

    /* Read incomming message from the client */
    int bytes = ssl->ssl_read(rx_buffer, sizeof(rx_buffer), false);
    if (bytes <= 0)
    {
        bc_log(Error, "%s", ssl->get_last_error().c_str());
        return NULL;
    }

    /* Parse and handle the request */
    std::string request = std::string((char*)rx_buffer, bytes);
    if (!session->handle_request(request)) bc_log(Warning, "Rejecting HLS request: (%s) %s", session->get_addr(), request.c_str());
    else bc_log(Debug, "Received HLS request from: client(%s)[%d]: %s", session->get_addr(), ssl->get_fd(), request.c_str());

    /* Send response */
    hls_byte_buffer &tx_buffer = session->tx_buffer_get();
    if (tx_buffer.used() && ssl->ssl_write(tx_buffer.data(), tx_buffer.used()) <= 0)
    {
        bc_log(Error, "%s", ssl->get_last_error().c_str());
        delete session;
        return NULL;
    }

    /* Start recording file streaming if requested */
    hls_filestream *fstream = session->get_fstream();
    while (fstream != NULL)
    {
        uint8_t buffer[HLS_SERVER_CHUNK_MAX]; // Read data from file
        ssize_t size = fstream->read_data(buffer, sizeof(buffer));

        /* Send data to the client */
        if (size > 0 && (ssl->ssl_write(buffer, size) <= 0))
        {
            bc_log(Error, "%s", ssl->get_last_error().c_str());
            break;
        }

        /* Check if while file is streamed to client */
        if (fstream->eof_reached() || size <= 0) break;
    }

    /* Finish */
    delete session;
    return NULL;
}
#endif /* BC_HLS_WITH_SSL */

//////////////////////////////////////////////////
// HLS FILE STREAM
//////////////////////////////////////////////////

bool hls_filestream::open_file(const char *path)
{
    _fd = open(path, O_RDONLY);
    if (_fd < 0) return false;

    _size = lseek(_fd, 0, SEEK_END);
    lseek(_fd, 0, SEEK_SET);

    return true;
}

ssize_t hls_filestream::read_data(uint8_t *data, size_t size)
{
    if (!is_active()) return 0;

    ssize_t bytes = read(_fd, data, size);
    size_t posit = bytes < 0 ? 0 : bytes;
    data[posit] = '\0';

    _read += posit;
    _eof = (_read >= _size) ? true : false;

    return bytes;
}

void hls_filestream::finish()
{
    if (_fd >= 0)
    {
        close(_fd);
        _fd = -1;
    }
}

//////////////////////////////////////////////////
// BYTE BUFFER
//////////////////////////////////////////////////

void hls_byte_buffer::clear()
{
    if (_data != NULL)
    {
        free(_data);
        _data = NULL;
        _size = 0;
        _used = 0;
    }
}

size_t hls_byte_buffer::advance(size_t size)
{
    this->erase(0, size);
    this->resize(_used);
    return _size;
}

size_t hls_byte_buffer::erase(size_t posit, size_t size)
{
    if (!size || posit >= _used) return 0;
    size = ((posit + size) > _used) ? 
            _used - posit : size;

    size_t tail_offset = posit + size;
    if (tail_offset >= _used)
    {
        _used = posit;
        return _used;
    }

    size_t tail_size = _used - tail_offset;
    const uint8_t *tail = &_data[tail_offset];
    memmove(&_data[posit], tail, tail_size);

    _used -= size;
    return size;
}

size_t hls_byte_buffer::resize(size_t size)
{
    if (_data == NULL && size)
    {
        _data = (uint8_t*)malloc(size);
        if (_data == NULL)
        {
            bc_log(Error, "Failed to allocate memory for HLS byte buffer: %s", strerror(errno));
            return 0;
        }

        _size = size;
        _used = 0;
        return size;
    }
    else if (_data != NULL && !size)
    {
        this->clear();
        return 0;
    }

    if (!_size || _data == NULL) return 0;
    _used = (size < _used) ? size : _used;
    _data = (uint8_t*)realloc(_data, size);

    if (size && _data == NULL)
    {
        bc_log(Error, "Failed to reallocate memory for HLS byte buffer: %s", strerror(errno));
        _size = 0;
        _used = 0;
        return 0;
    }

    _size = size;
    return _size;
}

size_t hls_byte_buffer::append(uint8_t *data, size_t size)
{
    if (data == NULL || !size) return 0;

    if (_used + size > _size)
        this->resize(_used + size + 1);

    memcpy(&_data[_used], data, size);
    _used += size;
    return _used;
}

//////////////////////////////////////////////////
// HLS CONTENT
//////////////////////////////////////////////////

hls_content::hls_content(hls_config *cfg)
{
    pthread_mutexattr_t attr;

    if (pthread_mutexattr_init(&attr) ||
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) ||
        pthread_mutex_init(&_mutex, &attr) ||
        pthread_mutexattr_destroy(&attr))
    {
        bc_log(Error, "Can not init pthread mutex: %s", strerror(errno));
        return;
    }

    _config = *cfg;
    _init = true;
}

hls_content::~hls_content()
{
    if (!_init) return;

    // CRITICAL FIX: Safe cleanup with timeout protection
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 2; // 2 second timeout
    
    if (pthread_mutex_timedlock(&_mutex, &timeout) == 0) {
        clear_window();
        pthread_mutex_unlock(&_mutex);
    } else {
        bc_log(Error, "Failed to acquire mutex during HLS content destruction - forcing cleanup");
        // Force cleanup without mutex if timeout occurs
        while (_window.size()) {
            hls_segment *front = _window.front();
            _window.pop_front();
            if (front) delete front;
        }
        if (_init_segment) {
            delete _init_segment;
            _init_segment = NULL;
        }
    }
    
    pthread_mutex_destroy(&_mutex);
}

bool hls_content::clear_window()
{
    if (!lock_mutex()) {
        return false;
    }

    while (_window.size()) {
        hls_segment *front = _window.front();
        _window.pop_front();
        delete front;
    }

    if (_init_segment) {
        delete _init_segment;
        _init_segment = NULL;
    }

    _in_buffer.clear();
    _pts = 0;
    _cc = 0;

    return unlock_mutex();
}

bool hls_content::append_segment(hls_segment *segment)
{
    if (!lock_mutex()) {
        return false;
    }

    segment->_meta.id = ++this->_cc;
    _window.push_back(segment);

    _fmp4 = (segment->_meta.type == hls_segment::type::fmp4);
    this->update_pts(segment->_meta.pts);
    bool buffer_moved = false;

    if (_fmp4 && _use_initial && !_init_segment) 
        set_initial_segment(segment);

    while (_config.window_size && _window.size() > _config.window_size) {
        hls_segment *front = _window.front();
        delete front;

        _window.pop_front();
        buffer_moved = true;
    }

    if (_fmp4 && _use_initial && _window.size() && 
        (!_init_segment || buffer_moved)) {
        hls_segment *front = _window.front();
        if (front) set_initial_segment(front);
    }

    return unlock_mutex();
}

void hls_content::set_initial_segment(hls_segment *segment)
{
    if (_init_segment) delete _init_segment; 
    _init_segment = segment->copy();
}

hls_segment* hls_content::get_initial_segment()
{
    if (!_init_segment) return NULL; 
    return _init_segment->copy();
}

bool hls_content::finish_segment(int64_t pts)
{
    hls_segment *segment = new hls_segment;
    if (segment == NULL)
    {
        bc_log(Error, "Can not allocate data for HLS segment obj");
        clear_window();
        update_pts(pts);
        return false;
    }

    if (!segment->add_data(_in_buffer.data(), _in_buffer.used()))
    {
        clear_window();
        update_pts(pts);
        delete segment;
        return false;
    }

    _in_buffer.clear();
    segment->_meta = _meta;

    return append_segment(segment);
}

bool hls_content::add_data(uint8_t *data, size_t size, int64_t pts, hls_segment::type type, int flags)
{
    size_t segment_size = 0;
    double segment_duration = 0.;

    get_config(&segment_size, &segment_duration);
    if (!segment_size && segment_duration == 0.) return false;

    if (_append_criteria && (_meta.pts != pts))
    {
        _append_criteria = false;
        finish_segment(pts);
    }

    if (!_in_buffer.append(data, size)) return false;
    bool is_key = flags & AV_PKT_FLAG_KEY;
    int64_t last_pts = get_last_pts();
    if (last_pts <= 0) update_pts(pts);

    int64_t pts_diff = pts - get_last_pts();
    double duration = pts_diff > 0 ? (double)pts_diff / (double)90000 : 0;

    if (_in_buffer.used() > HLS_SEGMENT_SIZE_MAX || (is_key &&
        ((segment_duration > 0. && duration >= segment_duration) ||
        (segment_size > 0 && _in_buffer.used() >= segment_size))))
    {
        _append_criteria = true;
        _meta.duration = duration;
        _meta.is_key = is_key;
        _meta.type = type;
        _meta.pts = pts;
    }

    return true;
}

hls_segment* hls_content::get_segment(uint32_t id)
{
    hls_segment *segment = NULL;

    if (pthread_mutex_lock(&_mutex))
    {
        bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
        return NULL;
    }

    for (size_t i = 0; i < _window.size(); i++)
    {
        hls_segment *temp = _window[i];
        if (temp->_meta.id == id)
        {
            segment = temp->copy();
            break;
        }
    }

    if (pthread_mutex_unlock(&_mutex))
    {
        bc_log(Error, "Can not unlock pthread mutex: %s", strerror(errno));
        return NULL;
    }

    return segment;
}

size_t hls_content::get_segment_ids(hls_segments &segments)
{
    if (!lock_mutex()) {
        return 0;
    }

    for (size_t i = 0; i < _window.size(); i++) {
        hls_segment *temp = _window[i];
        segments.push_back(temp->_meta.id);
    }

    size_t count = segments.size();
    unlock_mutex();
    return count;
}

void hls_content::set_config(hls_config *config)
{
    if (!lock_mutex()) {
        return;
    }

    _config = *config;

    unlock_mutex();
}

void hls_content::get_config(size_t *segment_size, double *segment_duration)
{
    if (!lock_mutex()) {
        return;
    }

    *segment_size = _config.segment_size;
    *segment_duration = _config.segment_duration;

    unlock_mutex();
}

//////////////////////////////////////////////////
// HLS EVENTS
//////////////////////////////////////////////////

void hls_clear_event(hls_events::event_data *ev_data)
{
    if (ev_data->ptr != NULL)
    {
        if (ev_data->type == hls_events::type::session)
        {
            hls_session *session = (hls_session*)ev_data->ptr;
            if (session != NULL)
            {
                delete session;
                /*
                    This socket will be closed in the session destructor and we must
                    set -1 on it just because we don't need to close it twice below.
                */
                ev_data->fd = -1;
            }
        }

        ev_data->ptr = NULL;
    }

    /* Shutdown and close file descriptor */
    ev_data->fd = sock_shutdown(ev_data->fd);
}

int hls_read_event(hls_events *events, hls_events::event_data *ev_data)
{
    if (ev_data->type == hls_events::type::listener)
    {
        hls_listener *listener = (hls_listener*)ev_data->ptr;
        socklen_t len = sizeof(struct sockaddr);
        struct sockaddr_in addr;
    
        /* Accept to the new connection request */
        int client_fd = accept(ev_data->fd, (struct sockaddr*)&addr, &len);
        if (client_fd < 0)
        {
            bc_log(Error, "Can not accept to the socket: %s", strerror(errno));
            return 0;
        }

        /* Get active flags on the socket */
        int fl = fcntl(client_fd, F_GETFL);
        if (fl < 0)
        {
            bc_log(Error, "Failed fcntl(): %s", strerror(errno));
            client_fd = sock_shutdown(client_fd);
            return 0;
        }

        /* Set non-block flag */
        fl = fcntl(client_fd, F_SETFL, fl | O_NONBLOCK);
        if (fl < 0)
        {
            bc_log(Error, "Failed to non-block socket: %s", strerror(errno));
            client_fd = sock_shutdown(client_fd);
            return 0;
        }

        /* Initialize new session */
        hls_session *session = new hls_session;
        session->set_listener(listener);
        session->set_addr(addr.sin_addr);
        session->set_fd(client_fd);

        /* Register client into the event instance */
        hls_events::event_data *sess_data = events->register_event(session, client_fd, EPOLLIN, hls_events::type::session);
        if (sess_data == NULL)
        {
            delete session;
            return 0;
        }

        /* Give session access to the event engine */
        session->set_hls_event_data(sess_data);
        session->set_event_handler(events);

        bc_log(Debug, "Connected HLS client: %s (%d)", session->get_addr(), client_fd);
        return 1;
    }
    else if (ev_data->type == hls_events::type::session)
    {
        hls_session *session = (hls_session*)ev_data->ptr;
        int client_fd = ev_data->fd;
        char rx_buffer[HLS_REQUEST_MAX];

        /* Read incomming message from the client */
        int bytes = read(client_fd, rx_buffer, sizeof(rx_buffer));
        if (bytes <= 0)
        {
            if (!bytes) bc_log(Debug, "Disconnected HLS client: %s[%d]", session->get_addr(), client_fd);
            else bc_log(Error, "Can not read data from client: %s (%s)", session->get_addr(), strerror(errno));
            return -1;
        }

        /* Null terminate receive buffer */
        rx_buffer[bytes] = '\0';
        session->rx_buffer_append(rx_buffer);

        /* Get completed request */
        std::string request = session->get_request();
        if (!request.length()) return 0;

        /* Parse and handle the request */
        if (!session->handle_request(request)) bc_log(Warning, "Rejecting HLS request: (%s) %s", session->get_addr(), request.c_str());
        else bc_log(Debug, "Received HLS request from: client(%s)[%d]: %s", session->get_addr(), client_fd, request.c_str());

        /* Callback on writeable */
        return session->writeable();
    }

    return -1;
}

int hls_write_event(hls_events *events, hls_events::event_data *ev_data)
{
    hls_session *session = (hls_session*)ev_data->ptr;
    ssize_t data_left = session->tx_buffer_flush();

    if (data_left < 0) return -1;
    else if (data_left) return 1;

    hls_filestream *fstream = session->get_fstream();
    if (fstream == NULL) return -1;

    uint8_t buffer[HLS_SERVER_CHUNK_MAX];
    ssize_t size = fstream->read_data(buffer, sizeof(buffer));
    session->tx_buffer_append(buffer, size);

    if (fstream->eof_reached() || size <= 0)
    {
        delete fstream;
        session->set_fstream(NULL);
    }

    return session->tx_buffer_get().used() ? 1 : -1;
}

int hls_event_callback(void *events, void* data, int reason)
{
    hls_events::event_data *ev_data = (hls_events::event_data*)data;
    hls_events *pevents = (hls_events*)events;
    int server_fd = (*(int*)pevents->get_user_data());

    switch (reason)
    {
        case HLS_EVENT_READ:
            return hls_read_event(pevents, ev_data);
        case HLS_EVENT_WRITE:
            return hls_write_event(pevents, ev_data);
        case HLS_EVENT_CLEAR:
            hls_clear_event(ev_data);
            break;
        case HLS_EVENT_HUNGED:
            bc_log(Warning, "HLS Connection hunged: fd(%d)", ev_data->fd);
            pevents->remove(ev_data);
            break;
        case HLS_EVENT_CLOSED:
            bc_log(Debug, "HLS Connection closed: fd(%d)", ev_data->fd);
            pevents->remove(ev_data);
            break;
        case HLS_EVENT_USER:
            bc_log(Debug, "Finishing HLS session: fd(%d)", ev_data->fd);
            pevents->remove(ev_data);
            break;
        case HLS_EVENT_DESTROY:
            bc_log(Info, "HLS Service destroyed");
            close(server_fd);
            break;
        default:
            break;
    }

    return 0;
}

//////////////////////////////////////////////////
// HLS LISTENER
//////////////////////////////////////////////////

hls_listener::hls_listener()
{
    pthread_mutexattr_t attr;

    if (pthread_mutexattr_init(&attr) ||
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) ||
        pthread_mutex_init(&_mutex, &attr) ||
        pthread_mutexattr_destroy(&attr))
    {
        bc_log(Error, "Can not init pthread mutex: %s", strerror(errno));
        return;
    }

    _config.window_size = HLS_WINDOW_SIZE;
    _config.segment_size = HLS_SEGMENT_SIZE;
    _config.segment_duration = HLS_SEGMENT_DURATION;

    _init = true;
}

hls_listener::~hls_listener()
{
    if (!_init) return;
    
    // CRITICAL FIX: Safe cleanup with timeout protection
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 3; // 3 second timeout
    
    if (pthread_mutex_timedlock(&_mutex, &timeout) == 0) {
        // Clear all content safely
        hls_content_it it;
        for (it = _content.begin(); it != _content.end(); it++) {
            hls_content *content = it->second;
            if (content) {
                delete content;
            }
        }
        _content.clear();
        pthread_mutex_unlock(&_mutex);
    } else {
        bc_log(Error, "Failed to acquire mutex during HLS listener destruction - forcing cleanup");
        // Force cleanup without mutex if timeout occurs
        hls_content_it it;
        for (it = _content.begin(); it != _content.end(); it++) {
            hls_content *content = it->second;
            if (content) {
                delete content;
            }
        }
        _content.clear();
    }
    
    pthread_mutex_destroy(&_mutex);
    _fd = sock_shutdown(_fd);
}

void hls_listener::reconfigure(hls_config *config)
{
    if (pthread_mutex_lock(&_mutex))
    {
        bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
        return;
    }

    _config = *config;
    hls_content_it it;

    for (it = _content.begin(); it != _content.end(); it++)
    {
        hls_content *content = it->second;
        content->set_config(config);
    }

    if (pthread_mutex_unlock(&_mutex))
    {
        bc_log(Error, "Can not unlock pthread mutex: %s", strerror(errno));
        return;
    }
}

hls_content* hls_listener::get_hls_content(int id)
{
    if (pthread_mutex_lock(&_mutex))
    {
        bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
        return NULL;
    }

    hls_content *content = NULL;
    hls_content_it it = _content.find(id);

    if (it == _content.end())
    {
        content = new hls_content(&_config);
        if (!content->_init)
        {
            delete content;
            content = NULL;
        }
        else
        {
            content->set_id(id);
            _content[id] = content;
        }
    }
    else
    {
        content = it->second;
    }

    if (pthread_mutex_unlock(&_mutex))
    {
        bc_log(Error, "Can not unlock pthread mutex: %s", strerror(errno));
        return NULL;
    }

    return content;
}

bool hls_listener::create_socket(uint16_t port)
{
    if (!_init)
    {
        bc_log(Error, "HLS listener is not initialized");
        return false;
    }

     _port = port;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);;

    _fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    if (_fd < 0)
    {
        bc_log(Error, "Failed to create listener socket: (%s)", strerror(errno));
        return false;
    }

    const int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        bc_log(Error, "Failed to set SO_REUSEADDR on the HLS socket: (%s)", strerror(errno));
        _fd = sock_shutdown(_fd);
        return false;
    }

    /* Bind the socket on port */
    if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        bc_log(Error, "Failed to bind socket on port: %u (%s)", _port, strerror(errno));
        _fd = sock_shutdown(_fd);
        return false;
    }

    /* Listen to the socket */
    if (listen(_fd, HLS_EVENTS_MAX) < 0) 
    {
        bc_log(Error, "Failed to listen port: %u (%s)", _port, strerror(errno));
        _fd = sock_shutdown(_fd);
        return false;
    }

    bc_log(Info, "HLS server started listen to port: %d (SSL=%d)", _port, _use_ssl ? 1 : 0);
    return true;
}

void hls_listener::set_ssl_ctx(const char *key, const char* crt, const char *ca)
{
#ifdef BC_HLS_WITH_SSL
    /* Mandatory */
    if (crt != NULL && key != NULL)
    {
        struct stat buffer;
        if (lstat(key, &buffer) < 0)
        {
            bc_log(Error, "Can not find SSL key: %s", key);
            return;
        }

        if (lstat(crt, &buffer) < 0)
        {
            bc_log(Error, "Can not find SSL cert: %s", crt);
            return;
        }

        _ssl_cert.cert_path = std::string(crt);
        _ssl_cert.key_path = std::string(key);
        _use_ssl = true;
    }

    /* Optional */
    if (ca != NULL) _ssl_cert.ca_path = std::string(ca);
#else
    bc_log(Error, "No SSL support!");
#endif
}

bool hls_listener::register_listener(uint16_t port)
{
    /* Create socket and start listen */
    if (!this->create_socket(port)) return false;

    /* Initialize ssl server context */
    if (_use_ssl)
    {
#ifdef BC_HLS_WITH_SSL
        bool status = _ssl.init_server(_fd, &_ssl_cert);
        if (!status) bc_log(Error, "%s", _ssl.get_last_error().c_str());
        return status;
#else
        bc_log(Error, "Requested HTTPS listener without SSL support");
        return false;
#endif
    }

    /* Create event instance and add listener socket to the instance  */
    if (!_events.create(0, &_fd, hls_event_callback) ||
        !_events.register_event(this, _fd, EPOLLIN, hls_events::type::listener)) return false;

    return true;
}

bool hls_listener::ssl_service()
{
#ifdef BC_HLS_WITH_SSL
    struct sockaddr_in inaddr;

    /* Accept to the new SSL connection request */
    hls_ssl *client = _ssl.accept_new(&inaddr);
    if (client == NULL)
    {
        bc_log(Error, "%s", _ssl.get_last_error().c_str());
        sleep(1); // maybe server is too busy, try again
        return true;
    }

    /* Initialize new session */
    hls_session *session = new hls_session;
    session->set_listener(this);
    session->set_addr(inaddr.sin_addr);
    session->set_ssl(client);

    bc_log(Debug, "Connected HLS (SSL) client: %s (%d)", session->get_addr(), client->get_fd());

    pthread_attr_t attr;
    pthread_t thread_id;

    if (pthread_attr_init(&attr) ||
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) ||
        pthread_create(&thread_id, &attr, hls_session_thread, session) ||
        pthread_attr_destroy(&attr))
    {
        bc_log(Error, "Can not create HLS session thread: %s", strerror(errno));
        delete session;
        return false;
    }

    return true;
#endif /* BC_HLS_WITH_SSL */

    bc_log(Error, "No SSL support");
    return false;
}

void hls_listener::run()
{
    pthread_setname_np(pthread_self(), "HLS_SERVER");

    if (!_init)
    {
        bc_log(Error, "HLS listener is not initialized");
        return;
    }

    bool status = true;
    if (_use_ssl) while(status) status = ssl_service();
    else while (status) status = _events.service(100);

    /* Thats all */
    close(_fd);
    _fd = -1;
}
