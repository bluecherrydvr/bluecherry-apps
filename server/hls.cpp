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
#include <arpa/inet.h>

#include <string>
#include <vector>

#include "libbluecherry.h"
#include "decoder.h"
#include "hls.h"

#ifndef PTHREAD_MUTEX_RECURSIVE
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif

#define HLS_SEGMENT_SIZE (188 * 7 * 1024)

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

void hls_events::service_callback(hls_event_data *data)
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

void hls_events::clear_callback(hls_event_data *data)
{
    if (data != NULL)
    {
        event_callback(this, data, HLS_EVENT_CLEAR);
        free(data);
    }
}

hls_event_data* hls_events::register_event(void *ctx, int fd, int events, int type)
{
    /* Allocate memory for event data */
    hls_event_data* data = (hls_event_data*)malloc(sizeof(hls_event_data));
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

bool hls_events::add(hls_event_data* data, int events)
{
    struct epoll_event event;
    event.data.ptr = data;
    event.events = events;

    if (epoll_ctl(event_fd, EPOLL_CTL_ADD, data->fd, &event) < 0)
    {
        bc_log(Error, "epoll_ctl() failed: %s", strerror(errno));
        return false;
    }

    return true;
}

bool hls_events::modify(hls_event_data *data, int events)
{
    struct epoll_event event;
    event.data.ptr = data;
    event.events = events;

    if (epoll_ctl(event_fd, EPOLL_CTL_MOD, data->fd, &event) < 0)
    {
        bc_log(Error, "epoll_ctl() failed: %s", strerror(errno));
        return false;
    }

    return true;
}

bool hls_events::remove(hls_event_data *data)
{
    int efd = data->fd;
    clear_callback(data);

    if (epoll_ctl(event_fd, EPOLL_CTL_DEL, efd, NULL) < 0) return false;
    return true;
}

bool hls_events::service(int timeout_ms)
{
    int count; /* Wait for ready events */
    do count = epoll_wait(event_fd, event_array, events_max, timeout_ms);
    while (errno == EINTR);

    for (int i = 0; i < count; i++)
    {
        /* Call callback for each ready event */
        hls_event_data *data = (hls_event_data*)event_array[i].data.ptr;
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
    segment->set_duration(duration());
    segment->set_first(is_first());
    segment->set_last(is_last());
    segment->set_key(is_key());
    segment->set_pts(pts());
    segment->set_id(id());
    return segment->add_data(data(), size()) ? segment : NULL;
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
    if (_fd >= 0)
    {
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
        _fd = -1;
    }
}

int hls_session::writeable()
{
    int events = EPOLLRDHUP | EPOLLIN | EPOLLOUT;
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
    size_t old_size = _tx_buffer.size();
    _tx_buffer.resize(old_size + size);
    memcpy(_tx_buffer.data() + old_size, data, size);
}

size_t hls_session::tx_buffer_advance(size_t size)
{
    _tx_buffer.erase(_tx_buffer.begin(), _tx_buffer.begin() + size);
    return _tx_buffer.size();
}

size_t hls_session::tx_buffer_flush()
{
    if (!_tx_buffer.size()) return 0;

    int sent = send(_fd, _tx_buffer.data(), _tx_buffer.size(), MSG_NOSIGNAL);
    if (sent <= 0)
    {
        bc_log(Error, "Can not send data to HLS client: %s (%s)", get_addr(), strerror(errno));
        return 0;
    }

    tx_buffer_advance(sent);
    return _tx_buffer.size();
}

bool hls_session::create_response()
{
    if (_type == hls_session::request_type::invalid)
    {
        bc_log(Warning, "Invalid HLS request for device: %d id(%u)", _device_id, _segment_id);
        std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Length: 0\r\n\r\n");

        _tx_buffer.resize(response.length());
        memcpy(_tx_buffer.data(), response.c_str(), response.length());
        return true;
    }
    else if (_type == hls_session::request_type::index)
    {
        std::string body;
        std_string_append(body, "#EXTM3U\n");
        std_string_append(body, "#EXT-X-INDEPENDENT-SEGMENTS\n");
        std_string_append(body, "#EXT-X-STREAM-INF:BANDWIDTH=%d\n", 90000);
        std_string_append(body, "0/playlist.m3u8\n");

        std::string response = std::string("HTTP/1.1 200 OK\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s", "*\r\n");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: %s", "application/vnd.apple.mpegurl\r\n");
        std_string_append(response, "Cache-Control: %s", "no-cache\r\n");
        std_string_append(response, "Content-Length: %zu\r\n\r\n", body.length());
        response.append(body);

        _tx_buffer.resize(response.length());
        memcpy(_tx_buffer.data(), response.c_str(), response.length());

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

            _tx_buffer.resize(response.length());
            memcpy(_tx_buffer.data(), response.c_str(), response.length());
            return true;
        }

        if (pthread_mutex_lock(&content->_mutex))
        {
            bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
            return false;
        }

        uint32_t sequence = content->_window.size() ? content->_window[0]->id() : 0;
        double duration = content->_window.size() ? content->_window[0]->duration() : 0.;

        std::string body;
        std_string_append(body, "#EXTM3U\n");
        std_string_append(body, "#EXT-X-VERSION: 5\n");
        std_string_append(body, "#EXT-X-ALLOW-CACHE: NO\n");
        std_string_append(body, "#EXT-X-TARGETDURATION: %.f\n", duration + 1);
        std_string_append(body, "#EXT-X-MEDIA=SEQUENCE: %u\n", sequence);
        std_string_append(body, "#EXT-X-INDEPENDENT-SEGMENTS\n");

        if (content->have_initial_segment())
            std_string_append(body, "#EXT-X-MAP:URI=\"init.mp4\"\n");

        for (size_t i = 0; i < content->_window.size(); i++)
        {
            std_string_append(body, "#EXTINF:%.4f,\n", content->_window[i]->duration());
            std_string_append(body, "%u/payload.m4s\n", content->_window[i]->id());
        }

        if (pthread_mutex_unlock(&content->_mutex))
        {
            bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
            return false;
        }

        hls_date date;
        char curr_date[128], exp_date[128];
        date.to_http(curr_date, sizeof(curr_date));
        date.inc_sec(5);
        date.to_http(exp_date, sizeof(exp_date));

        std::string response = std::string("HTTP/1.1 200 OK\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s", "*\r\n");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: %s", "application/vnd.apple.mpegurl\r\n");
        std_string_append(response, "Cache-Control: %s", "no-cache\r\n");
        std_string_append(response, "Date: %s\r\n", curr_date);
        std_string_append(response, "Expires: %s\r\n", exp_date);
        std_string_append(response, "Content-Length: %zu\r\n\r\n", body.length());
        response.append(body);

        _tx_buffer.resize(response.length());
        memcpy(_tx_buffer.data(), response.c_str(), response.length());

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

            _tx_buffer.resize(response.length());
            memcpy(_tx_buffer.data(), response.c_str(), response.length());
            return true;
        }

        if (pthread_mutex_lock(&content->_mutex))
        {
            bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
            return false;
        }

        hls_segment *segment = content->get_initial_segment();

        if (pthread_mutex_unlock(&content->_mutex))
        {
            bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
            return false;
        }        

        if (segment == NULL)
        {
            bc_log(Warning, "HLS initial payload not found for requested device: %d id(%u)", _device_id, _segment_id);
            std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");

            _tx_buffer.resize(response.length());
            memcpy(_tx_buffer.data(), response.c_str(), response.length());
            return true;
        }

        std::string response = std::string("HTTP/1.1 206 Partial Content\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s", "*\r\n");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: %s", "video/mp4\r\n");
        std_string_append(response, "Cache-Control: %s", "no-cache\r\n");
        std_string_append(response, "Accept-Ranges: bytes\r\n");
        std_string_append(response, "Connection: close\r\n");
        std_string_append(response, "Server: bluechery\r\n");
        std_string_append(response, "Content-Length: %zu\r\n\r\n", segment->size());

        size_t response_size = response.length() + segment->size();
        _tx_buffer.resize(response_size);

        memcpy(_tx_buffer.data(), response.c_str(), response.length());
        memcpy(_tx_buffer.data() + response.length(), segment->data(), segment->size());

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

            _tx_buffer.resize(response.length());
            memcpy(_tx_buffer.data(), response.c_str(), response.length());
            return true;
        }

        hls_segment *segment = content->get_segment(_segment_id);
        if (segment == NULL)
        {
            bc_log(Warning, "HLS payload not found for requested device: %d id(%u)", _device_id, _segment_id);
            std::string response = std::string("HTTP/1.1 404 Not Fount\r\n");
            std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
            std_string_append(response, "Content-Length: 0\r\n\r\n");

            _tx_buffer.resize(response.length());
            memcpy(_tx_buffer.data(), response.c_str(), response.length());
            return true;
        }

        std::string response = std::string("HTTP/1.1 206 Partial Content\r\n");
        std_string_append(response, "Access-Control-Allow-Origin: %s", "*\r\n");
        std_string_append(response, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(response, "Content-Type: %s", "video/mp4\r\n");
        std_string_append(response, "Cache-Control: %s", "no-cache\r\n");
        std_string_append(response, "Accept-Ranges: bytes\r\n");
        std_string_append(response, "Connection: close\r\n");
        std_string_append(response, "Server: bluechery\r\n");
        std_string_append(response, "Content-Length: %zu\r\n\r\n", segment->size());

        size_t response_size = response.length() + segment->size();
        _tx_buffer.resize(response_size);

        memcpy(_tx_buffer.data(), response.c_str(), response.length());
        memcpy(_tx_buffer.data() + response.length(), segment->data(), segment->size());

        delete segment;
        return true;
    }

    return false;
}

bool hls_session::handle_request(const std::string &request)
{
	size_t start_posit = request.find("/");
	if (start_posit == std::string::npos) return false;

	size_t end_posit = request.find(" ", start_posit);
	if (end_posit == std::string::npos) return false;

	size_t url_length = end_posit - start_posit;
	std::string url = request.substr(start_posit, url_length);

    size_t posit = url.find("/", 1);
    if (posit == std::string::npos) return create_response();
    _device_id = std::stoi(url.substr(1, posit));

    if (url.find("/index.m3u8") != std::string::npos)
    {
        _type = request_type::index;
    }
    if (url.find("/init.mp4") != std::string::npos)
    {
        _type = request_type::initial;
    }
    else if (url.find("/playlist.m3u8") != std::string::npos)
    {
        size_t offset = posit + 1;
        posit = url.find("/", offset);
        if (posit == std::string::npos) return create_response();
        _stream_id = std::stoi(url.substr(offset, posit));
        _type = request_type::playlist;
    }
    else if (url.find("/payload.m4s") != std::string::npos)
    {
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

    return create_response();
}

//////////////////////////////////////////////////
// HLS CONTENT
//////////////////////////////////////////////////

hls_content::hls_content()
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

    set_window_size(20);
    _init = true;
}

hls_content::~hls_content()
{
    if (!_init) return;

    clear_window();
    pthread_mutex_destroy(&_mutex);
}

bool hls_content::clear_window()
{
    if (pthread_mutex_lock(&_mutex))
    {
        bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
        return false;
    }

    while (_window.size())
    {
        hls_segment *front = _window.front();
        _window.pop_front();
        delete front;
    }

    if (_init_segment)
    {
        delete _init_segment;
        _init_segment = NULL;
    }

    if (pthread_mutex_unlock(&_mutex))
    {
        bc_log(Error, "Can not unlock pthread mutex: %s", strerror(errno));
        return false;
    }

    return true;
}

bool hls_content::append_segment(hls_segment *segment)
{
    if (pthread_mutex_lock(&_mutex))
    {
        bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
        return false;
    }

    _cc++; // Continuity counter
    segment->set_id(_cc);
    _window.push_back(segment);

    this->update_pts(segment->pts());
    bool buffer_moved = false;

    if (_use_initial && !_init_segment) 
        set_initial_segment(segment);

    while (_window_size && _window.size() > _window_size)
    {
        hls_segment *front = _window.front();
        delete front;

        _window.pop_front();
        buffer_moved = true;
    }

    if (_use_initial && _window.size() && 
        (!_init_segment || buffer_moved))
    {
        hls_segment *front = _window.front();
        if (front) set_initial_segment(front);
    }

    if (pthread_mutex_unlock(&_mutex))
    {
        bc_log(Error, "Can not unlock pthread mutex: %s", strerror(errno));
        return false;
    }

    return true;
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

bool hls_content::add_data(uint8_t *data, size_t size, int64_t pts, int flags)
{
    bool is_key = flags & AV_PKT_FLAG_KEY;
    int64_t pts_diff = (pts - get_last_pts());
    double duration = (double)pts_diff / (double)90000;

    size_t buff_size = _in_buffer.size();
    _in_buffer.resize(buff_size + size);
    uint8_t *offset = _in_buffer.data();
    memcpy(offset + buff_size, data, size);

    //if (is_key && _in_buffer.size() >= HLS_SEGMENT_SIZE)
    if (is_key && pts_diff >= 90000) // 1 sec in pts terms 
    {
        hls_segment *segment = new hls_segment;
        segment->add_data(_in_buffer.data(), _in_buffer.size());
        segment->set_duration(duration);
        segment->set_key(is_key);
        segment->set_pts(pts);

        _in_buffer.clear();
        return append_segment(segment);

    }

    return false;
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
        if (temp->id() == id)
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
    if (pthread_mutex_lock(&_mutex))
    {
        bc_log(Error, "Can not lock pthread mutex: %s", strerror(errno));
        return 0;
    }

    for (size_t i = 0; i < _window.size(); i++)
    {
        hls_segment *temp = _window[i];
        segments.push_back(temp->id());
    }

    if (pthread_mutex_unlock(&_mutex))
    {
        bc_log(Error, "Can not unlock pthread mutex: %s", strerror(errno));
        return 0;
    }

    return segments.size();
}

//////////////////////////////////////////////////
// HLS EVENTS
//////////////////////////////////////////////////

void hls_clear_event(hls_event_data *ev_data)
{
    if (ev_data != NULL)
    {
        if (ev_data->ptr != NULL)
        {
            free(ev_data->ptr);
            ev_data->ptr = NULL;
        }

        if (ev_data->fd >= 0) 
        {
            shutdown(ev_data->fd, SHUT_RDWR);
            close(ev_data->fd);
            ev_data->fd = -1;
        }
    }
}

int hls_read_event(hls_events *events, hls_event_data *ev_data)
{
    /* Get server socket descriptor */
    int server_fd = (*(int*)events->get_user_data());

    if (server_fd == ev_data->fd)
    {
        hls_listener *listener = (hls_listener*)ev_data->ptr;
        socklen_t len = sizeof(struct sockaddr);
        struct sockaddr_in addr;
    
        /* Acceot to the new connection request */
        int client_fd = accept(server_fd, (struct sockaddr*)&addr, &len);
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
            shutdown(client_fd, SHUT_RDWR);
            close(client_fd);
            return 0;
        }

        /* Set non-block flag */
        fl = fcntl(client_fd, F_SETFL, fl | O_NONBLOCK);
        if (fl < 0)
        {
            bc_log(Error, "Failed to non-block socket: %s", strerror(errno));
            shutdown(client_fd, SHUT_RDWR);
            close(client_fd);
            return 0;
        }

        /* Initialize new session */
        hls_session *session = new hls_session;
        session->set_listener(listener);
        session->set_addr(addr.sin_addr);
        session->set_fd(client_fd);

        /* Register client into the event instance */
        hls_event_data *sess_data = events->register_event(session, client_fd, EPOLLIN, 0);
        if (sess_data == NULL)
        {
            delete session;
            return 0;
        }

        /* Give session access to the event engine */
        session->set_hls_event_data(sess_data);
        session->set_event_handler(events);

        bc_log(Debug, "Connected HLS client: %s (%d)", session->get_addr(), client_fd);
    }
    else
    {
        hls_session *session = (hls_session*)ev_data->ptr;
        int client_fd = ev_data->fd;
        char rx_buffer[HLS_REQUEST_MAX];

        /* Read incomming message from the client */
        int bytes = read(client_fd, rx_buffer, sizeof(rx_buffer));
        if (bytes <= 0)
        {
            if (!bytes) bc_log(Debug, "Disconnected client: %s[%d]", session->get_addr(), client_fd);
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
        else bc_log(Debug, "Received request from: client(%s)[%d]: %s", session->get_addr(), client_fd, request.c_str());

        /* Callback on writeable */
        return session->writeable();
    }

    return 1;
}

int hls_write_event(hls_events *events, hls_event_data *ev_data)
{
    hls_session *session = (hls_session*)ev_data->ptr;
    return session->tx_buffer_flush() ? 1 : -1;
}

int hls_event_callback(void *events, void* data, int reason)
{
    hls_event_data *ev_data = (hls_event_data*)data;
    hls_events *pevents = (hls_events*)events;
    int server_fd = (*(int*)pevents->get_user_data());

    switch(reason)
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

    _init = true;
}

hls_listener::~hls_listener()
{
    if (!_init) return;

//    clear_window();
    pthread_mutex_destroy(&_mutex);

    if (_fd >= 0)
    {
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
        _fd = -1;
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
        content = new hls_content;
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

    _fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_fd < 0)
    {
        bc_log(Error, "Failed to create listener socket: (%s)", strerror(errno));
        return false;
    }

	const int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        bc_log(Error, "Failed to set SO_REUSEADDR on the HLS socket: (%s)", strerror(errno));
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
        _fd = -1;
        return false;
    }

    /* Bind the socket on port */
    if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        bc_log(Error, "Failed to bind socket on port: %u (%s)", _port, strerror(errno));
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
        _fd = -1;
        return false;
    }

    /* Listen to the socket */
    if (listen(_fd, HLS_EVENTS_MAX) < 0) 
    {
        bc_log(Error, "Failed to listen port: %u (%s)", _port, strerror(errno));
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
        _fd = -1;
        return false;
    }

    bc_log(Info, "HLS server started listen to port: %d", _port);
    return true;
}

bool hls_listener::register_listener(uint16_t port)
{
    /* Create socket and start listen */
    if (!this->create_socket(port)) return false;

    /* Create event instance and add listener socket to the instance  */
    if (!_events.create(0, &_fd, hls_event_callback) ||
        !_events.register_event(this, _fd, EPOLLIN, 0)) return false;

    return true;
}

void hls_listener::run()
{
    if (!_init)
    {
        bc_log(Error, "HLS listener is not initialized");
        return;
    }

    bool status = true;
    while (status) /* Main service loop */
        status = _events.service(100);

    /* Thats all */
    close(_fd);
    _fd = -1;
}
