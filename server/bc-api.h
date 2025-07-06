#ifndef __BC_API__
#define __BC_API__

#include <sys/epoll.h>
#include <string>

#include "bc-stats.h"

#define BC_REQUEST_MAX             4096
#define BC_EVENTS_MAX              120000

#define BC_EVENT_ERROR             1
#define BC_EVENT_USER              2
#define BC_EVENT_READ              3
#define BC_EVENT_WRITE             4
#define BC_EVENT_HUNGED            5
#define BC_EVENT_CLOSED            6
#define BC_EVENT_CLEAR             7
#define BC_EVENT_DESTROY           8
#define BC_EVENT_EXCEPTION         9

typedef int(*event_callback_t)(void *events, void* data, int reason);

class bc_events
{
public:

    typedef enum {
        listener = (int)0,
        session,
        fstream
    } type;

    typedef struct {
        bc_events::type type;   // Event type
        void *ptr;              // Data pointer
        int events;             // Ready events
        int fd;                 // Socket descriptor
    } event_data;

    ~bc_events();

    bool create(size_t max, void *userptr, event_callback_t callBack);
    bc_events::event_data* register_event(void *ctx, int fd, int events, bc_events::type type);

    bool add(bc_events::event_data* data, int events);
    bool modify(bc_events::event_data *data, int events);
    bool remove(bc_events::event_data *data);
    bool service(int timeout_ms);

    void service_callback(bc_events::event_data *data);
    void clear_callback(bc_events::event_data *data);
    void *get_user_data() { return user_data; }

private:
    event_callback_t        event_callback = NULL;  /* Service callback */
    struct epoll_event*     event_array = NULL;     /* EPOLL event array */
    void*                   user_data = NULL;       /* User data pointer */
    uint32_t                events_max = 0;         /* Max allowed file descriptors */
    int                     event_fd = -1;          /* EPOLL File decriptor */
};

class bc_api;

class api_session
{
public:
    ~api_session();

    int writeable();

    std::string get_request();
    bool authenticate(const std::string &request);
    bool handle_request(const std::string &request);

    std::string& tx_buffer_get() { return _tx_buffer; }
    void tx_buffer_append(const char *data, size_t size);
    size_t tx_buffer_advance(size_t size);
    ssize_t tx_buffer_flush();

    void rx_buffer_append(const char *data) { _rx_buffer.append(data); }
    void rx_buffer_advance(size_t size) { _rx_buffer.erase(0, size); }

    void set_api_event_data(bc_events::event_data *data) { _ev_data = data; }
    void set_event_handler(bc_events *events) { _events = events; }
    bc_events* get_event_handler() { return _events; }

    void set_listener(bc_api *listener) { _api = listener; } 
    bc_api* get_listener() { return _api; }

    const char *get_addr() { return _address.c_str(); }
    void set_addr(const struct in_addr addr);
    void set_fd(int fd) { _fd = fd; }
    int get_fd() { return _fd; }

private:
    /* Objects */
    bc_events::event_data*  _ev_data = NULL;
    bc_events*              _events = NULL;
    bc_api*                 _api = NULL;

    /* rx/tx */
    std::string     _api_token;
    std::string     _tx_buffer;
    std::string     _rx_buffer;
    std::string     _address;
    int             _fd = -1;
};

class bc_api {
public:
    ~bc_api();

    void run();

    bool create_socket(uint16_t port);
    bool start_listener(uint16_t port);
    void set_stats(bc_stats* stats) { _stats = stats; }

    void get_cpu_stats(std::string &outout);
    void get_memory_stats(std::string &outout);
    void get_network_stats(std::string &outout);
    void get_storage_stats(std::string &outout);
    void get_overall_stats(std::string &outout);

    // Historical data methods
    void get_cpu_history(std::string &outout, size_t count = 60);
    void get_memory_history(std::string &outout, size_t count = 60);
    void get_network_history(std::string &outout, size_t count = 60);
    void get_storage_history(std::string &outout, size_t count = 60);
    void get_overall_history(std::string &outout, size_t count = 60);

    void create_http_response(std::string &outout, const std::string &body);

private:
    bc_stats*       _stats;
    bc_events       _events;
    uint16_t        _port = 0;
    int             _fd = -1;
};

#endif /* __BC_API__ */