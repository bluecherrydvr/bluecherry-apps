#include "hls.h"
#include "../lib/logging.h"
#include <arpa/inet.h>
#include <string.h>

void hls_session::set_addr(const struct in_addr addr)
{
    char address[64];
    int length = snprintf(address, sizeof(address), "%d.%d.%d.%d",
        (int)((addr.s_addr & 0x000000FF)),
        (int)((addr.s_addr & 0x0000FF00)>>8),
        (int)((addr.s_addr & 0x00FF0000)>>16),
        (int)((addr.s_addr & 0xFF000000)>>24));
    _address = std::string(address, length);
}

std::string hls_session::get_request()
{
    std::string request;
    size_t posit = _rx_buffer.find("\r\n\r\n");
    if (posit != std::string::npos) {
        request = _rx_buffer.substr(0, posit + 4);
        rx_buffer_advance(request.length());
    }
    return request;
}

bool hls_session::authenticate(const std::string &uri, const std::string &request)
{
    if (!_listener || !_listener->get_auth()) {
        return true;
    }

    // Extract auth token from request
    size_t auth_pos = request.find("Authorization: Bearer ");
    if (auth_pos == std::string::npos) {
        return false;
    }

    auth_pos += 21; // length of "Authorization: Bearer "
    size_t end_pos = request.find("\r\n", auth_pos);
    if (end_pos == std::string::npos) {
        return false;
    }

    _auth_token = request.substr(auth_pos, end_pos - auth_pos);
    return !_auth_token.empty();
}

bool hls_session::create_response()
{
    std::string response;
    switch (_type) {
        case request_type::unauthorized:
            response = "HTTP/1.1 401 Unauthorized\r\n"
                      "Content-Type: text/plain\r\n"
                      "Content-Length: 12\r\n\r\n"
                      "Unauthorized";
            break;
        case request_type::invalid:
            response = "HTTP/1.1 400 Bad Request\r\n"
                      "Content-Type: text/plain\r\n"
                      "Content-Length: 11\r\n\r\n"
                      "Bad Request";
            break;
        default:
            return false;
    }

    tx_buffer_append((uint8_t*)response.c_str(), response.length());
    return true;
}

bool hls_session::handle_request(const std::string &request)
{
    /* Get start position of URL */
    size_t start_posit = request.find("/hls");
    if (start_posit == std::string::npos) {
        start_posit = request.find("/");
        if (start_posit == std::string::npos) return false;
    } else {
        start_posit += 4; // skip /hls
    }

    /* Get end position of URL */
    size_t end_posit = request.find(" ", start_posit);
    if (end_posit == std::string::npos) return false;

    /* Calculate URL length */
    size_t url_length = end_posit - start_posit;
    std::string url = request.substr(start_posit, url_length);

    if (url.find("/index.m3u8") != std::string::npos) {
        size_t posit = url.find("recording=");
        if (posit != std::string::npos) {
            /* Requested recording playlist */
            posit += 10; // length of recording=
            _recording = url.substr(posit, std::string::npos);
            if (!_recording.length()) return create_response();
            _type = request_type::rec_playlist;
        } else {
            /* Requested live view index */
            _type = request_type::index;
        }
    } else if (url.find("/init.mp4") != std::string::npos) {
        /* Requested live view initial segment */
        _type = request_type::initial;
    } else if (url.find("/playlist.m3u8") != std::string::npos) {
        /* Requested live view playlist */
        size_t posit = url.find("/", 1);
        if (posit == std::string::npos) return create_response();

        size_t offset = posit + 1;
        posit = url.find("/", offset);
        if (posit == std::string::npos) return create_response();
        _stream_id = std::stoi(url.substr(offset, posit));
        _type = request_type::playlist;
    } else if ((url.find("/payload.m4s") != std::string::npos) ||
            (url.find("/payload.ts") != std::string::npos)) {
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
    } else if (url.find(".mp4") != std::string::npos) {
        /* Requested recording file */
        size_t posit = url.find("?", 0);
        if (posit == std::string::npos) _recording = url;
        else _recording = url.substr(0, posit);
        _type = request_type::recording;
    }

    if (_type != request_type::rec_playlist &&
        _type != request_type::recording &&
        _type != request_type::invalid) {
        size_t posit = url.find("/", 1);
        if (posit == std::string::npos) return create_response();
        _device_id = std::stoi(url.substr(1, url.find("/", 1)));
    }

    /* Authenticate request */
    if (!authenticate(url, request)) {
        _type = request_type::unauthorized;
        return create_response();
    }

    /* At this condition, live view request is valid */
    return create_response();
}

ssize_t hls_session::tx_buffer_flush()
{
    if (!_tx_buffer.used()) return 0;

    ssize_t sent = send(_fd, _tx_buffer.data(), _tx_buffer.used(), MSG_NOSIGNAL);
    if (sent < 0) {
        bc_log(Error, "Can not send data to HLS client: %s (%s)", get_addr(), strerror(errno));
        return sent;
    }

    _tx_buffer.advance(sent);
    return _tx_buffer.used();
}

void hls_session::tx_buffer_append(uint8_t *data, size_t size)
{
    if (data && size > 0) {
        _tx_buffer.append(data, size);
    }
} 