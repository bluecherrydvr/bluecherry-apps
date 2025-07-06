#ifndef __BC_STATS__
#define __BC_STATS__

#include <unordered_map>
#include <string>
#include <vector>
#include <thread>
#include <deque>
#include <mutex>
#include <chrono>

#include <inttypes.h>
#include <pthread.h>

#define _BC_ALIGNED_ __attribute__((aligned))

// Forward declaration for use in bc_stats
struct stats_history_entry;

class bc_stats 
{
public:
    struct memory {
        uint64_t _BC_ALIGNED_ resident = 0;
        uint64_t _BC_ALIGNED_ virt = 0;
        uint64_t _BC_ALIGNED_ avail = 0;
        uint64_t _BC_ALIGNED_ total = 0;
        uint64_t _BC_ALIGNED_ free = 0;
        uint64_t _BC_ALIGNED_ swap = 0;
        uint64_t _BC_ALIGNED_ buff = 0;
    };

    struct storage_path {
        std::string path = std::string("");
        uint64_t total_size = 0;      // Total size in bytes
        uint64_t used_size = 0;       // Used size in bytes
        uint64_t free_size = 0;       // Free size in bytes
        uint32_t usage_percent = 0;   // Usage percentage (0-100)
        std::string filesystem = std::string(""); // Filesystem type
    };

    struct cpu_info {
        int cpu_id = 0; // -1 for sum

        /* Calculated percents */
        uint32_t _BC_ALIGNED_ soft_ints = 0;
        uint32_t _BC_ALIGNED_ hard_ints = 0;
        uint32_t _BC_ALIGNED_ user_niced = 0;
        uint32_t _BC_ALIGNED_ kernel_space = 0;
        uint32_t _BC_ALIGNED_ user_space = 0;
        uint32_t _BC_ALIGNED_ idle_time = 0;
        uint32_t _BC_ALIGNED_ io_wait = 0;

        /* Raw information */
        uint32_t _BC_ALIGNED_ soft_ints_raw = 0;
        uint32_t _BC_ALIGNED_ hard_ints_raw = 0;
        uint32_t _BC_ALIGNED_ user_niced_raw = 0;
        uint32_t _BC_ALIGNED_ kernel_space_raw = 0;
        uint32_t _BC_ALIGNED_ user_space_raw = 0;
        uint32_t _BC_ALIGNED_ idle_time_raw = 0;
        uint32_t _BC_ALIGNED_ io_wait_raw = 0;
        uint64_t _BC_ALIGNED_ total_raw = 0;
    };

    struct net_iface {
        std::string hwaddr = std::string("");
        std::string ipaddr = std::string("");
        std::string name = std::string("");

        uint64_t bytes_recv_per_sec = 0;
        uint64_t bytes_sent_per_sec = 0;
        uint64_t pkts_recv_per_sec = 0;
        uint64_t pkts_sent_per_sec = 0;

        int64_t bytes_recv = 0;
        int64_t bytes_sent = 0;
        int64_t pkts_recv = 0;
        int64_t pkts_sent = 0;
        int32_t type = 0;
    };

    static void copy_cpu_info(bc_stats::cpu_info *dst, bc_stats::cpu_info *src);
    static uint64_t parse_info(char *buffer, size_t size, const char *field);
    static int load_file(const char *path, char *buffer, size_t size);

    static uint32_t bc_float_to_u32(float value);
    static float bc_u32_to_float(uint32_t value);

    typedef std::unordered_map<std::string, net_iface>::iterator network_it;
    typedef std::unordered_map<std::string, net_iface>::const_iterator network_const_it;
    typedef std::unordered_map<std::string, net_iface> network;
    typedef std::vector<cpu_info> cpu_infos;

    struct cpu {
        struct process {
            uint32_t _BC_ALIGNED_ kernel_space_usg = 0;
            uint32_t _BC_ALIGNED_ user_space_usg = 0;
            uint64_t _BC_ALIGNED_ kernel_space = 0;
            uint64_t _BC_ALIGNED_ user_space = 0;
            uint64_t _BC_ALIGNED_ total_time = 0;
            int64_t _BC_ALIGNED_ kernel_childs = 0;
            int64_t _BC_ALIGNED_ user_childs = 0;
        } proc;

        uint16_t _BC_ALIGNED_ core_count = 0;
        uint32_t _BC_ALIGNED_ load_avg[3];

        cpu_infos cores;
        cpu_info sum;
    };

    void start_monithoring();
    void stop_monithoring();
    void display();

    void get_proc_usage(bc_stats::cpu::process *proc);
    void get_net_info(bc_stats::network *net);
    void get_mem_info(bc_stats::memory *mem);
    bool get_cpu_info(bc_stats::cpu *cpu);
    void get_storage_info(std::vector<storage_path> *storage_paths);

    // Historical data methods
    void add_history_entry();
    std::vector<stats_history_entry> get_history_entries(size_t count = 60);
    void clear_old_history();
    size_t get_history_size() const;

private:
    void monithoring_thread();
    bool update_net_info();
    bool update_mem_info();
    bool update_cpu_info();
    bool update_storage_info();

    uint8_t _BC_ALIGNED_ _active = 0;
    uint8_t _BC_ALIGNED_ _cancel = 0;
    uint8_t _monitoring_interval = 1;
    pthread_mutex_t     _mutex;
    std::thread _thread;

    bc_stats::network   _network;
    bc_stats::memory    _memory;
    bc_stats::cpu       _cpu;
    std::vector<storage_path> _storage_paths;

    // Historical data storage
    std::deque<stats_history_entry> _history;
    mutable std::mutex _history_mutex;
    static const size_t MAX_HISTORY_ENTRIES = 300; // 10 minutes at 2-second intervals
};

// Now define the struct after bc_stats
struct stats_history_entry {
    std::chrono::system_clock::time_point timestamp;
    bc_stats::memory memory_data;
    bc_stats::cpu cpu_data;
    bc_stats::network network_data;
    std::vector<bc_stats::storage_path> storage_data;
};

#endif /* __BC_STATS__ */
