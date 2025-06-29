#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "bc-api.h"
#include "bc-stats.h"
#include "libbluecherry.h"

extern "C" {
#include <libavutil/avstring.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/base64.h>
}

static int api_sock_shutdown(int fd)
{
    if (fd >= 0)
    {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }

    return -1;
}

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
// BC EVENT HANDLER
//////////////////////////////////////////////////

bc_events::~bc_events()
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
        event_callback(this, NULL, BC_EVENT_DESTROY);
}

bool bc_events::create(size_t max, void *userptr, event_callback_t callback)
{
    events_max = max > 0 ? max : BC_EVENTS_MAX;
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

void bc_events::service_callback(bc_events::event_data *data)
{
    /* Check error condition */
    if (data->events & EPOLLRDHUP) { event_callback(this, data, BC_EVENT_CLOSED); return; }
    if (data->events & EPOLLHUP) { event_callback(this, data, BC_EVENT_HUNGED); return; }
    if (data->events & EPOLLERR) { event_callback(this, data, BC_EVENT_ERROR); return; }
    if (data->events & EPOLLPRI) { event_callback(this, data, BC_EVENT_EXCEPTION); return; }

    /* Callback on writeable */
    if (data->events & EPOLLOUT && event_callback(this, data, BC_EVENT_WRITE) < 0)
        { event_callback(this, data, BC_EVENT_USER); return; } // User requested callback

    /* Callback on readable */
    if (data->events & EPOLLIN && event_callback(this, data, BC_EVENT_READ) < 0)
        { event_callback(this, data, BC_EVENT_USER); return; } // User requested callback
}

void bc_events::clear_callback(bc_events::event_data *data)
{
    if (data != NULL)
    {
        event_callback(this, data, BC_EVENT_CLEAR);
        free(data);
    }
}

bc_events::event_data* bc_events::register_event(void *ctx, int fd, int events, bc_events::type type)
{
    /* Allocate memory for event data */
    bc_events::event_data* data = (bc_events::event_data*)malloc(sizeof(bc_events::event_data));
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

bool bc_events::add(bc_events::event_data* data, int events)
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

bool bc_events::modify(bc_events::event_data *data, int events)
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

bool bc_events::remove(bc_events::event_data *data)
{
    bool status = true;
    if (data->fd >= 0 && epoll_ctl(event_fd, EPOLL_CTL_DEL, data->fd, NULL) < 0) status = false;
    clear_callback(data);
    return status;
}

bool bc_events::service(int timeout_ms)
{
    int count; /* Wait for ready events */
    do count = epoll_wait(event_fd, event_array, events_max, timeout_ms);
    while (errno == EINTR);

    for (int i = 0; i < count; i++)
    {
        /* Call callback for each ready event */
        bc_events::event_data *data = (bc_events::event_data*)event_array[i].data.ptr;
        data->events = event_array[i].events;
        service_callback(data);
    }

    return (count < 0) ? false : true;
}

//////////////////////////////////////////////////
// BC API SESSION
//////////////////////////////////////////////////

std::string api_session::get_request() 
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

api_session::~api_session()
{
    _fd = api_sock_shutdown(_fd);
}

int api_session::writeable()
{
    int events = EPOLLRDHUP | EPOLLOUT;
    return _events->modify(_ev_data, events) ? 1 : -1;
}

void api_session::set_addr(const struct in_addr addr)
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

void api_session::tx_buffer_append(const char *data, size_t size) 
{
    _tx_buffer.append(data, size);
}

ssize_t api_session::tx_buffer_flush()
{
    if (!_tx_buffer.size()) return 0;

    ssize_t sent = send(_fd, _tx_buffer.c_str(), _tx_buffer.size(), MSG_NOSIGNAL);
    if (sent < 0)
    {
        bc_log(Error, "Can not send data to HLS client: %s (%s)", get_addr(), strerror(errno));
        return sent;
    }

    _tx_buffer.erase(0, sent);
    return _tx_buffer.size();
}

bool api_session::authenticate(const std::string &request)
{
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
                bc_user_auth(username, password, ACCESS_REMOTE, -1) == 1)
            {
                delete[] buf;
                return true;
            }
        }

        delete[] buf;
    }

    return false;
}

bool api_session::handle_request(const std::string &request)
{
    /* Authenticate request */
    if (!authenticate(request))
    {
        bc_log(Debug, "Authorization failure for API request");

        _tx_buffer = std::string("HTTP/1.1 401 Unauthorized\r\n");
        std_string_append(_tx_buffer, "Access-Control-Allow-Origin: %s\r\n", "*");
        std_string_append(_tx_buffer, "WWW-Authenticate: Basic realm=\"HLS Server\"\r\n");
        std_string_append(_tx_buffer, "User-Agent: bluechery/%s\r\n", __VERSION__);
        std_string_append(_tx_buffer, "Content-Length: 0\r\n\r\n");
        return true;
    }

    /* Get start position of URL */
    size_t start_posit = request.find("/api");
    if (start_posit == std::string::npos)
    {
        start_posit = request.find("/");
        if (start_posit == std::string::npos) return false;
    }
    else start_posit += 4; // skip /api

    /* Get end position of URL */
    size_t end_posit = request.find(" ", start_posit);
    if (end_posit == std::string::npos) return false;

    /* Calculate URL length */
    size_t url_length = end_posit - start_posit;
    std::string url = request.substr(start_posit, url_length);

    if (!url.compare(0, 10, "/stats/cpu"))
    {
        std::string body;
        _api->get_cpu_stats(body);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }
    else if (!url.compare(0, 13, "/stats/memory"))
    {
        std::string body;
        _api->get_memory_stats(body);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }
    else if (!url.compare(0, 14, "/stats/network"))
    {
        std::string body;
        _api->get_network_stats(body);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }
    else if (!url.compare(0, 15, "/stats/storage"))
    {
        std::string body;
        _api->get_storage_stats(body);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }
    else if (!url.compare(0, 14, "/stats/overall"))
    {
        std::string body;
        _api->get_overall_stats(body);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }
    else if (!url.compare(0, 15, "/stats/cpu/history"))
    {
        std::string body;
        size_t count = 60; // Default to 60 entries (2 minutes)
        
        // Parse count parameter if present
        size_t param_pos = url.find("?count=");
        if (param_pos != std::string::npos) {
            std::string count_str = url.substr(param_pos + 7);
            count = std::stoul(count_str);
            if (count > 300) count = 300; // Cap at 300 entries (10 minutes)
        }
        
        _api->get_cpu_history(body, count);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }
    else if (!url.compare(0, 18, "/stats/memory/history"))
    {
        std::string body;
        size_t count = 60; // Default to 60 entries (2 minutes)
        
        // Parse count parameter if present
        size_t param_pos = url.find("?count=");
        if (param_pos != std::string::npos) {
            std::string count_str = url.substr(param_pos + 7);
            count = std::stoul(count_str);
            if (count > 300) count = 300; // Cap at 300 entries (10 minutes)
        }
        
        _api->get_memory_history(body, count);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }
    else if (!url.compare(0, 19, "/stats/network/history"))
    {
        std::string body;
        size_t count = 60; // Default to 60 entries (2 minutes)
        
        // Parse count parameter if present
        size_t param_pos = url.find("?count=");
        if (param_pos != std::string::npos) {
            std::string count_str = url.substr(param_pos + 7);
            count = std::stoul(count_str);
            if (count > 300) count = 300; // Cap at 300 entries (10 minutes)
        }
        
        _api->get_network_history(body, count);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }
    else if (!url.compare(0, 20, "/stats/storage/history"))
    {
        std::string body;
        size_t count = 60; // Default to 60 entries (2 minutes)
        
        // Parse count parameter if present
        size_t param_pos = url.find("?count=");
        if (param_pos != std::string::npos) {
            std::string count_str = url.substr(param_pos + 7);
            count = std::stoul(count_str);
            if (count > 300) count = 300; // Cap at 300 entries (10 minutes)
        }
        
        _api->get_storage_history(body, count);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }
    else if (!url.compare(0, 19, "/stats/overall/history"))
    {
        std::string body;
        size_t count = 60; // Default to 60 entries (2 minutes)
        
        // Parse count parameter if present
        size_t param_pos = url.find("?count=");
        if (param_pos != std::string::npos) {
            std::string count_str = url.substr(param_pos + 7);
            count = std::stoul(count_str);
            if (count > 300) count = 300; // Cap at 300 entries (10 minutes)
        }
        
        _api->get_overall_history(body, count);
        _api->create_http_response(_tx_buffer, body);
        return true;
    }

    bc_log(Warning, "Invalid API request: %s", url.c_str());
    _tx_buffer = std::string("HTTP/1.1 501 Not Implemented\r\n");
    std_string_append(_tx_buffer, "User-Agent: bluechery/%s\r\n", __VERSION__);
    std_string_append(_tx_buffer, "Content-Length: 0\r\n\r\n");
    return true;
}

//////////////////////////////////////////////////
// BC API CALLBACKS
//////////////////////////////////////////////////

void api_clear_event(bc_events::event_data *ev_data)
{
    if (ev_data->ptr != NULL)
    {
        if (ev_data->type == bc_events::type::session)
        {
            api_session *session = (api_session*)ev_data->ptr;
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
    ev_data->fd = api_sock_shutdown(ev_data->fd);
}

int api_read_event(bc_events *events, bc_events::event_data *ev_data)
{
    if (ev_data->type == bc_events::type::listener)
    {
        bc_api *listener = (bc_api*)ev_data->ptr;
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
            client_fd = api_sock_shutdown(client_fd);
            return 0;
        }

        /* Set non-block flag */
        fl = fcntl(client_fd, F_SETFL, fl | O_NONBLOCK);
        if (fl < 0)
        {
            bc_log(Error, "Failed to non-block socket: %s", strerror(errno));
            client_fd = api_sock_shutdown(client_fd);
            return 0;
        }

        /* Initialize new session */
        api_session *session = new api_session;
        session->set_listener(listener);
        session->set_addr(addr.sin_addr);
        session->set_fd(client_fd);

        /* Register client into the event instance */
        bc_events::event_data *sess_data = events->register_event(session, client_fd, EPOLLIN, bc_events::type::session);
        if (sess_data == NULL)
        {
            delete session;
            return 0;
        }

        /* Give session access to the event engine */
        session->set_api_event_data(sess_data);
        session->set_event_handler(events);

        bc_log(Debug, "Connected API client: %s (%d)", session->get_addr(), client_fd);
        return 1;
    }
    else if (ev_data->type == bc_events::type::session)
    {
        api_session *session = (api_session*)ev_data->ptr;
        int client_fd = ev_data->fd;
        char rx_buffer[BC_REQUEST_MAX];

        /* Read incomming message from the client */
        int bytes = read(client_fd, rx_buffer, sizeof(rx_buffer));
        if (bytes <= 0)
        {
            if (!bytes) bc_log(Debug, "Disconnected API client: %s[%d]", session->get_addr(), client_fd);
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
        if (!session->handle_request(request)) bc_log(Warning, "Rejecting API request: (%s) %s", session->get_addr(), request.c_str());
        else bc_log(Debug, "Received API request from: client(%s)[%d]: %s", session->get_addr(), client_fd, request.c_str());

        /* Callback on writeable */
        return session->writeable();
    }

    return -1;
}

void bc_api::create_http_response(std::string &outout, const std::string &body)
{
    outout = std::string("HTTP/1.1 200 OK\r\n");
    std_string_append(outout, "Access-Control-Allow-Origin: %s\r\n", "*");
    std_string_append(outout, "User-Agent: bluechery/%s\r\n", __VERSION__);
    std_string_append(outout, "Content-Type: %s\r\n", "application/json");
    std_string_append(outout, "Cache-Control: %s\r\n", "no-cache");
    std_string_append(outout, "Content-Length: %zu\r\n\r\n", body.length());
    outout.append(body);
}

void bc_api::get_cpu_stats(std::string &outout)
{
    bc_stats::cpu cpu;
    _stats->get_cpu_info(&cpu);

    char response[BC_REQUEST_MAX];
	int length = snprintf(response, sizeof(response),
		"{\"load_average\":["
			"{\"interval\":\"1m\",\"value\":%.2f},"
			"{\"interval\":\"5m\",\"value\":%.2f},"
			"{\"interval\":\"10m\",\"value\":%.2f}"
		"],"
		"\"usage\":{"
			"\"bluecherry\":{"
				"\"user_space\":%.2f,"
				"\"kernel_space\":%.2f"
			"},"
			"\"idle\":%.2f,"
			"\"user_space\":%.2f,"
			"\"kernel_space\":%.2f,"
			"\"user_niced\":%.2f,"
			"\"soft_ints\":%.2f,"
			"\"hard_ints\":%.2f,"
			"\"io_wait\":%.2f"
		"}}",
		bc_stats::bc_u32_to_float(cpu.load_avg[0]),
		bc_stats::bc_u32_to_float(cpu.load_avg[1]),
		bc_stats::bc_u32_to_float(cpu.load_avg[2]),
		bc_stats::bc_u32_to_float(cpu.proc.user_space_usg), 
		bc_stats::bc_u32_to_float(cpu.proc.kernel_space_usg),
		bc_stats::bc_u32_to_float(cpu.sum.idle_time),
		bc_stats::bc_u32_to_float(cpu.sum.user_space),
		bc_stats::bc_u32_to_float(cpu.sum.kernel_space),
		bc_stats::bc_u32_to_float(cpu.sum.user_niced),
		bc_stats::bc_u32_to_float(cpu.sum.soft_ints),
		bc_stats::bc_u32_to_float(cpu.sum.hard_ints),
		bc_stats::bc_u32_to_float(cpu.sum.io_wait));

    outout.append(response, length);
}

void bc_api::get_memory_stats(std::string &outout)
{
    bc_stats::memory meminfo;
    _stats->get_mem_info(&meminfo);

    char response[BC_REQUEST_MAX];
	int length = snprintf(response, sizeof(response), 
		"{"
			"\"bluecherry\":{"
				"\"resident\":%lu,"
				"\"virtual\":%lu"
			"},"
			"\"system\":{"
				"\"available\":%lu,"
				"\"buffers\":%lu,"
				"\"total\":%lu,"
				"\"swap\":%lu,"
				"\"free\":%lu"
			"}"
		"}",
		meminfo.resident,
		meminfo.virt,
		meminfo.avail,
		meminfo.buff,
		meminfo.total,
		meminfo.swap,
		meminfo.free
    );

    outout.append(response, length);
}

void bc_api::get_network_stats(std::string &outout)
{
    bc_stats::network netstat;
    _stats->get_net_info(&netstat);
	outout = std::string("[");

	for (bc_stats::network_it it = netstat.begin(); it != netstat.end(); it++)
	{
		bc_stats::net_iface iface = it->second;
		char buffer[BC_REQUEST_MAX];

		int length = snprintf(buffer, sizeof(buffer),
			"{"
				"\"name\":\"%s\","
				"\"type\":%d,"
				"\"hw_addr\":\"%s\","
				"\"ip_addr\":\"%s\","
				"\"bytes_received\":%ld,"
				"\"bytes_sent\":%ld,"
				"\"packets_received\":%ld,"
				"\"packets_sent\":%ld,"
				"\"bytes_received_per_sec\":%lu,"
				"\"bytes_sent_per_sec\":%lu,"
				"\"packets_received_per_sec\":%lu,"
				"\"packets_sent_per_sec\":%lu"
            "}",
			iface.name.c_str(),
			iface.type,
			iface.hwaddr.c_str(),
			iface.ipaddr.c_str(),
			iface.bytes_recv,
			iface.bytes_sent,
			iface.pkts_recv,
			iface.pkts_sent,
			iface.bytes_recv_per_sec,
			iface.bytes_sent_per_sec,
			iface.pkts_recv_per_sec,
			iface.pkts_sent_per_sec);

		outout.append(buffer, length);
        if (std::next(it) != netstat.end()) outout.append(",");
    }

    outout.append("]");
}

void bc_api::get_storage_stats(std::string &outout)
{
    std::vector<bc_stats::storage_path> storage_paths;
    _stats->get_storage_info(&storage_paths);
    
    outout = std::string("[");
    
    for (size_t i = 0; i < storage_paths.size(); i++)
    {
        const bc_stats::storage_path& path = storage_paths[i];
        char buffer[BC_REQUEST_MAX];
        
        int length = snprintf(buffer, sizeof(buffer),
            "{"
                "\"path\":\"%s\","
                "\"filesystem\":\"%s\","
                "\"total_size\":%lu,"
                "\"used_size\":%lu,"
                "\"free_size\":%lu,"
                "\"usage_percent\":%u"
            "}",
            path.path.c_str(),
            path.filesystem.c_str(),
            path.total_size,
            path.used_size,
            path.free_size,
            path.usage_percent);
        
        outout.append(buffer, length);
        if (i < storage_paths.size() - 1) outout.append(",");
    }
    
    outout.append("]");
}

void bc_api::get_overall_stats(std::string &outout)
{
    outout = std::string("{\"cpu\":");

    std::string stats;
    get_cpu_stats(stats);
    outout.append(stats);
    stats.clear();

    outout.append(",\"memory\":");
    get_memory_stats(stats);
    outout.append(stats);
    stats.clear();

    outout.append(",\"network\":");
    get_network_stats(stats);
    outout.append(stats);
    stats.clear();
    
    outout.append(",\"storage\":");
    get_storage_stats(stats);
    outout.append(stats);
    outout.append("}");
}

int api_write_event(bc_events *events, bc_events::event_data *ev_data)
{
    api_session *session = (api_session*)ev_data->ptr;
    ssize_t data_left = session->tx_buffer_flush();
    return (data_left <= 0) ? -1 : 1;
}

int bc_api_event_callback(void *events, void* data, int reason)
{
    bc_events::event_data *ev_data = (bc_events::event_data*)data;
    bc_events *pevents = (bc_events*)events;
    int server_fd = (*(int*)pevents->get_user_data());

    switch (reason)
    {
        case BC_EVENT_READ:
            return api_read_event(pevents, ev_data);
        case BC_EVENT_WRITE:
            return api_write_event(pevents, ev_data);
        case BC_EVENT_CLEAR:
            api_clear_event(ev_data);
            break;
        case BC_EVENT_HUNGED:
            bc_log(Warning, "API Connection hunged: fd(%d)", ev_data->fd);
            pevents->remove(ev_data);
            break;
        case BC_EVENT_CLOSED:
            bc_log(Debug, "API Connection closed: fd(%d)", ev_data->fd);
            pevents->remove(ev_data);
            break;
        case BC_EVENT_USER:
            bc_log(Debug, "Finishing API session: fd(%d)", ev_data->fd);
            pevents->remove(ev_data);
            break;
        case BC_EVENT_DESTROY:
            bc_log(Info, "API Service destroyed");
            close(server_fd);
            break;
        default:
            break;
    }

    return 0;
}

//////////////////////////////////////////////////
// BC API LISTENER
//////////////////////////////////////////////////

bc_api::~bc_api()
{
    _fd = api_sock_shutdown(_fd);
}

bool bc_api::create_socket(uint16_t port)
{
     _port = port;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);;

    _fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    if (_fd < 0)
    {
        bc_log(Error, "Failed to create API listener socket: (%s)", strerror(errno));
        return false;
    }

    const int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        bc_log(Error, "Failed to set SO_REUSEADDR on the API socket: (%s)", strerror(errno));
        _fd = api_sock_shutdown(_fd);
        return false;
    }

    /* Bind the socket on port */
    if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        bc_log(Error, "Failed to bind socket on port: %u (%s)", _port, strerror(errno));
        _fd = api_sock_shutdown(_fd);
        return false;
    }

    /* Listen to the socket */
    if (listen(_fd, BC_EVENTS_MAX) < 0) 
    {
        bc_log(Error, "Failed to listen port: %u (%s)", _port, strerror(errno));
        _fd = api_sock_shutdown(_fd);
        return false;
    }

    bc_log(Info, "API server started listen to port: %d", _port);
    return true;
}

bool bc_api::start_listener(uint16_t port)
{
    /* Create socket and start listen */
    if (!this->create_socket(port)) return false;

    /* Create event instance and add listener socket to the instance  */
    if (!_events.create(0, &_fd, bc_api_event_callback) ||
        !_events.register_event(this, _fd, EPOLLIN, bc_events::type::listener)) return false;

    return true;
}

void bc_api::run()
{
    pthread_setname_np(pthread_self(), "API");
    bool status = true;
    while (status) status = _events.service(100);

    /* Thats all */
    close(_fd);
    _fd = -1;
}

// Historical data methods implementation
void bc_api::get_cpu_history(std::string &outout, size_t count)
{
    std::vector<stats_history_entry> history = _stats->get_history_entries(count);
    
    outout = std::string("{\"history\":[");
    
    for (size_t i = 0; i < history.size(); i++)
    {
        const stats_history_entry& entry = history[i];
        auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
        
        char response[BC_REQUEST_MAX];
        int length = snprintf(response, sizeof(response),
            "{"
                "\"timestamp\":%ld,"
                "\"load_average\":["
                    "{\"interval\":\"1m\",\"value\":%.2f},"
                    "{\"interval\":\"5m\",\"value\":%.2f},"
                    "{\"interval\":\"10m\",\"value\":%.2f}"
                "],"
                "\"usage\":{"
                    "\"bluecherry\":{"
                        "\"user_space\":%.2f,"
                        "\"kernel_space\":%.2f"
                    "},"
                    "\"idle\":%.2f,"
                    "\"user_space\":%.2f,"
                    "\"kernel_space\":%.2f,"
                    "\"user_niced\":%.2f,"
                    "\"soft_ints\":%.2f,"
                    "\"hard_ints\":%.2f,"
                    "\"io_wait\":%.2f"
                "}"
            "}",
            time_t,
            bc_stats::bc_u32_to_float(entry.cpu_data.load_avg[0]),
            bc_stats::bc_u32_to_float(entry.cpu_data.load_avg[1]),
            bc_stats::bc_u32_to_float(entry.cpu_data.load_avg[2]),
            bc_stats::bc_u32_to_float(entry.cpu_data.proc.user_space_usg), 
            bc_stats::bc_u32_to_float(entry.cpu_data.proc.kernel_space_usg),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.idle_time),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.user_space),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.kernel_space),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.user_niced),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.soft_ints),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.hard_ints),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.io_wait));

        outout.append(response, length);
        if (i < history.size() - 1) outout.append(",");
    }
    
    outout.append("]}");
}

void bc_api::get_memory_history(std::string &outout, size_t count)
{
    std::vector<stats_history_entry> history = _stats->get_history_entries(count);
    
    outout = std::string("{\"history\":[");
    
    for (size_t i = 0; i < history.size(); i++)
    {
        const stats_history_entry& entry = history[i];
        auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
        
        char response[BC_REQUEST_MAX];
        int length = snprintf(response, sizeof(response),
            "{"
                "\"timestamp\":%ld,"
                "\"bluecherry\":{"
                    "\"resident\":%lu,"
                    "\"virtual\":%lu"
                "},"
                "\"system\":{"
                    "\"available\":%lu,"
                    "\"buffers\":%lu,"
                    "\"total\":%lu,"
                    "\"swap\":%lu,"
                    "\"free\":%lu"
                "}"
            "}",
            time_t,
            entry.memory_data.resident,
            entry.memory_data.virt,
            entry.memory_data.avail,
            entry.memory_data.buff,
            entry.memory_data.total,
            entry.memory_data.swap,
            entry.memory_data.free);

        outout.append(response, length);
        if (i < history.size() - 1) outout.append(",");
    }
    
    outout.append("]}");
}

void bc_api::get_network_history(std::string &outout, size_t count)
{
    std::vector<stats_history_entry> history = _stats->get_history_entries(count);
    
    outout = std::string("{\"history\":[");
    
    for (size_t i = 0; i < history.size(); i++)
    {
        const stats_history_entry& entry = history[i];
        auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
        
        outout.append("{");
        outout.append("\"timestamp\":");
        outout.append(std::to_string(time_t));
        outout.append(",\"interfaces\":[");
        
        bool first_interface = true;
        for (bc_stats::network_const_it it = entry.network_data.begin(); it != entry.network_data.end(); it++)
        {
            if (!first_interface) outout.append(",");
            first_interface = false;
            
            const bc_stats::net_iface& iface = it->second;
            char buffer[BC_REQUEST_MAX];

            int length = snprintf(buffer, sizeof(buffer),
                "{"
                    "\"name\":\"%s\","
                    "\"type\":%d,"
                    "\"hw_addr\":\"%s\","
                    "\"ip_addr\":\"%s\","
                    "\"bytes_received\":%ld,"
                    "\"bytes_sent\":%ld,"
                    "\"packets_received\":%ld,"
                    "\"packets_sent\":%ld,"
                    "\"bytes_received_per_sec\":%lu,"
                    "\"bytes_sent_per_sec\":%lu,"
                    "\"packets_received_per_sec\":%lu,"
                    "\"packets_sent_per_sec\":%lu"
                "}",
                iface.name.c_str(),
                iface.type,
                iface.hwaddr.c_str(),
                iface.ipaddr.c_str(),
                iface.bytes_recv,
                iface.bytes_sent,
                iface.pkts_recv,
                iface.pkts_sent,
                iface.bytes_recv_per_sec,
                iface.bytes_sent_per_sec,
                iface.pkts_recv_per_sec,
                iface.pkts_sent_per_sec);

            outout.append(buffer, length);
        }
        
        outout.append("]}");
        if (i < history.size() - 1) outout.append(",");
    }
    
    outout.append("]}");
}

void bc_api::get_storage_history(std::string &outout, size_t count)
{
    std::vector<stats_history_entry> history = _stats->get_history_entries(count);
    
    outout = std::string("{\"history\":[");
    
    for (size_t i = 0; i < history.size(); i++)
    {
        const stats_history_entry& entry = history[i];
        auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
        
        outout.append("{");
        outout.append("\"timestamp\":");
        outout.append(std::to_string(time_t));
        outout.append(",\"paths\":[");
        
        bool first_path = true;
        for (const auto& path : entry.storage_data)
        {
            if (!first_path) outout.append(",");
            first_path = false;
            
            char buffer[BC_REQUEST_MAX];
            int length = snprintf(buffer, sizeof(buffer),
                "{"
                    "\"path\":\"%s\","
                    "\"filesystem\":\"%s\","
                    "\"total_size\":%lu,"
                    "\"used_size\":%lu,"
                    "\"free_size\":%lu,"
                    "\"usage_percent\":%u"
                "}",
                path.path.c_str(),
                path.filesystem.c_str(),
                path.total_size,
                path.used_size,
                path.free_size,
                path.usage_percent);
            
            outout.append(buffer, length);
        }
        
        outout.append("]}");
        if (i < history.size() - 1) outout.append(",");
    }
    
    outout.append("]}");
}

void bc_api::get_overall_history(std::string &outout, size_t count)
{
    std::vector<stats_history_entry> history = _stats->get_history_entries(count);
    
    outout = std::string("{\"history\":[");
    
    for (size_t i = 0; i < history.size(); i++)
    {
        const stats_history_entry& entry = history[i];
        auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
        
        outout.append("{");
        outout.append("\"timestamp\":");
        outout.append(std::to_string(time_t));
        outout.append(",\"cpu\":");
        
        // CPU data
        char cpu_response[BC_REQUEST_MAX];
        int cpu_length = snprintf(cpu_response, sizeof(cpu_response),
            "{"
                "\"load_average\":["
                    "{\"interval\":\"1m\",\"value\":%.2f},"
                    "{\"interval\":\"5m\",\"value\":%.2f},"
                    "{\"interval\":\"10m\",\"value\":%.2f}"
                "],"
                "\"usage\":{"
                    "\"bluecherry\":{"
                        "\"user_space\":%.2f,"
                        "\"kernel_space\":%.2f"
                    "},"
                    "\"idle\":%.2f,"
                    "\"user_space\":%.2f,"
                    "\"kernel_space\":%.2f,"
                    "\"user_niced\":%.2f,"
                    "\"soft_ints\":%.2f,"
                    "\"hard_ints\":%.2f,"
                    "\"io_wait\":%.2f"
                "}"
            "}",
            bc_stats::bc_u32_to_float(entry.cpu_data.load_avg[0]),
            bc_stats::bc_u32_to_float(entry.cpu_data.load_avg[1]),
            bc_stats::bc_u32_to_float(entry.cpu_data.load_avg[2]),
            bc_stats::bc_u32_to_float(entry.cpu_data.proc.user_space_usg), 
            bc_stats::bc_u32_to_float(entry.cpu_data.proc.kernel_space_usg),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.idle_time),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.user_space),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.kernel_space),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.user_niced),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.soft_ints),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.hard_ints),
            bc_stats::bc_u32_to_float(entry.cpu_data.sum.io_wait));
        
        outout.append(cpu_response, cpu_length);
        
        // Memory data
        outout.append(",\"memory\":");
        char mem_response[BC_REQUEST_MAX];
        int mem_length = snprintf(mem_response, sizeof(mem_response),
            "{"
                "\"bluecherry\":{"
                    "\"resident\":%lu,"
                    "\"virtual\":%lu"
                "},"
                "\"system\":{"
                    "\"available\":%lu,"
                    "\"buffers\":%lu,"
                    "\"total\":%lu,"
                    "\"swap\":%lu,"
                    "\"free\":%lu"
                "}"
            "}",
            entry.memory_data.resident,
            entry.memory_data.virt,
            entry.memory_data.avail,
            entry.memory_data.buff,
            entry.memory_data.total,
            entry.memory_data.swap,
            entry.memory_data.free);
        
        outout.append(mem_response, mem_length);
        
        // Network data
        outout.append(",\"network\":[");
        bool first_interface = true;
        for (bc_stats::network_const_it it = entry.network_data.begin(); it != entry.network_data.end(); it++)
        {
            if (!first_interface) outout.append(",");
            first_interface = false;
            
            const bc_stats::net_iface& iface = it->second;
            char net_buffer[BC_REQUEST_MAX];

            int net_length = snprintf(net_buffer, sizeof(net_buffer),
                "{"
                    "\"name\":\"%s\","
                    "\"type\":%d,"
                    "\"hw_addr\":\"%s\","
                    "\"ip_addr\":\"%s\","
                    "\"bytes_received\":%ld,"
                    "\"bytes_sent\":%ld,"
                    "\"packets_received\":%ld,"
                    "\"packets_sent\":%ld,"
                    "\"bytes_received_per_sec\":%lu,"
                    "\"bytes_sent_per_sec\":%lu,"
                    "\"packets_received_per_sec\":%lu,"
                    "\"packets_sent_per_sec\":%lu"
                "}",
                iface.name.c_str(),
                iface.type,
                iface.hwaddr.c_str(),
                iface.ipaddr.c_str(),
                iface.bytes_recv,
                iface.bytes_sent,
                iface.pkts_recv,
                iface.pkts_sent,
                iface.bytes_recv_per_sec,
                iface.bytes_sent_per_sec,
                iface.pkts_recv_per_sec,
                iface.pkts_sent_per_sec);

            outout.append(net_buffer, net_length);
        }
        outout.append("]");
        
        // Storage data
        outout.append(",\"storage\":[");
        bool first_path = true;
        for (const auto& path : entry.storage_data)
        {
            if (!first_path) outout.append(",");
            first_path = false;
            
            char storage_buffer[BC_REQUEST_MAX];
            int storage_length = snprintf(storage_buffer, sizeof(storage_buffer),
                "{"
                    "\"path\":\"%s\","
                    "\"filesystem\":\"%s\","
                    "\"total_size\":%lu,"
                    "\"used_size\":%lu,"
                    "\"free_size\":%lu,"
                    "\"usage_percent\":%u"
                "}",
                path.path.c_str(),
                path.filesystem.c_str(),
                path.total_size,
                path.used_size,
                path.free_size,
                path.usage_percent);
            
            outout.append(storage_buffer, storage_length);
        }
        outout.append("]");
        
        if (i < history.size() - 1) outout.append(",");
    }
    
    outout.append("]}");
}
