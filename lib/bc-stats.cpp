#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <thread>
#include <sys/statvfs.h>
#include <mntent.h>
#include <set>
#include <map>
#include <cstdlib> // For system()

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "bc-stats.h"
#include "logc.h"
#include "libbluecherry.h"

#define BC_DIR_NETWORK           "/sys/class/net/"
#define BC_FILE_SELFSTATUS       "/proc/self/status"
#define BC_FILE_SELFSTAT         "/proc/self/stat"
#define BC_FILE_MEMINFO          "/proc/meminfo"
#define BC_FILE_LOADAVG          "/proc/loadavg"
#define BC_FILE_STAT             "/proc/stat"
#define BC_FILE_MAX              8192

static std::string get_iface_ip_addr(const char *iface)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) std::string("");

    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name, iface, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    char *ip_addr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    return std::string(ip_addr);
}

uint32_t bc_stats::bc_float_to_u32(float value)
{
    uint16_t integral = (uint16_t)floor(value);
    float balance = value - (float)integral;
    uint16_t decimal = (uint16_t)(balance * 100);

    uint32_t ret_val;
    ret_val = (uint32_t)integral;
    ret_val <<= 16;
    ret_val += (uint32_t)decimal;
    return ret_val;
}

float bc_stats::bc_u32_to_float(uint32_t value)
{
    uint16_t integral = (uint16_t)(value >> 16);
    uint16_t decimal = (uint16_t)(value & 0xFF);
    float balance = (float)decimal / (float)100;
    return (float)((float)integral + balance);
}

int bc_stats::load_file(const char *path, char *buffer, size_t size)
{
    /* Open target file for reading only */
	int fd = open(path, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
	if (fd < 0) return 0;

    /* Read whole file buffer from descriptor */
	int bytes = read(fd, buffer, size);
    if (bytes <= 0)
    {
        bc_log(Error, "Can not read file: %s (%s): %u", path, strerror(errno), bytes);
        close(fd);
        return 0;
    }

    /* Null terminate buffer */
	buffer[bytes] = '\0';
	close(fd);

	return bytes;
}

uint64_t bc_stats::parse_info(char *buffer, size_t size, const char *field)
{
    const char *end = buffer + size;
    char *offset = strstr(buffer, field);
    if (offset == NULL) return 0;
    offset += strlen(field) + 1;
    if (offset >= end) return 0;
    return atoll(offset);
}

void bc_stats::get_mem_info(bc_stats::memory *mem)
{
    mem->resident = __sync_add_and_fetch(&_memory.resident, 0);
    mem->virt = __sync_add_and_fetch(&_memory.virt, 0);
    mem->avail = __sync_add_and_fetch(&_memory.avail, 0);
    mem->total = __sync_add_and_fetch(&_memory.total, 0);
    mem->free = __sync_add_and_fetch(&_memory.free, 0);
    mem->swap = __sync_add_and_fetch(&_memory.swap, 0);
    mem->buff = __sync_add_and_fetch(&_memory.buff, 0);
}

bool bc_stats::update_mem_info()
{
    char buffer[BC_FILE_MAX]; /* Load /proc/meminfo file */
    int length = load_file(BC_FILE_MEMINFO, buffer, sizeof(buffer));
    if (length <= 0) return false;

    /* Parse memory statistics */
    __sync_lock_test_and_set(&_memory.total, parse_info(buffer, length, "MemTotal"));
    __sync_lock_test_and_set(&_memory.free, parse_info(buffer, length, "MemFree"));
    __sync_lock_test_and_set(&_memory.avail, parse_info(buffer, length, "MemAvailable"));
    __sync_lock_test_and_set(&_memory.buff, parse_info(buffer, length, "Buffers"));
    __sync_lock_test_and_set(&_memory.swap, parse_info(buffer, length, "SwapCached"));
    
    /* Calculate memory usage percentage using MemAvailable (consistent with web UI) */
    uint64_t total = __sync_add_and_fetch(&_memory.total, 0);
    uint64_t available = __sync_add_and_fetch(&_memory.avail, 0);
    if (total > 0) {
        uint32_t mem_usage_percent = (uint32_t)(100 - (available * 100 / total));
        __sync_lock_test_and_set(&_memory.usage_percent, mem_usage_percent);
    }

    /* Load /proc/self/status file */
    length = load_file(BC_FILE_SELFSTATUS, buffer, sizeof(buffer));
    if (length <= 0) return false;

    /* Parse memory statistics for current process */
    __sync_lock_test_and_set(&_memory.resident, parse_info(buffer, length, "VmRSS"));
    __sync_lock_test_and_set(&_memory.virt, parse_info(buffer, length, "VmSize"));

    return true;
}

void bc_stats::get_proc_usage(bc_stats::cpu::process *proc)
{
    proc->user_childs = __sync_add_and_fetch(&_cpu.proc.user_childs, 0);
    proc->kernel_childs = __sync_add_and_fetch(&_cpu.proc.kernel_childs, 0);
    proc->user_space = __sync_add_and_fetch(&_cpu.proc.user_space, 0);
    proc->kernel_space = __sync_add_and_fetch(&_cpu.proc.kernel_space, 0);
    proc->total_time = __sync_add_and_fetch(&_cpu.proc.total_time, 0);
    proc->user_space_usg = __sync_add_and_fetch(&_cpu.proc.user_space_usg, 0);
    proc->kernel_space_usg = __sync_add_and_fetch(&_cpu.proc.kernel_space_usg, 0);
}

void bc_stats::copy_cpu_info(bc_stats::cpu_info *dst, bc_stats::cpu_info *src)
{
    dst->soft_ints_raw = __sync_add_and_fetch(&src->soft_ints_raw, 0);
    dst->hard_ints_raw = __sync_add_and_fetch(&src->hard_ints_raw, 0);
    dst->kernel_space_raw = __sync_add_and_fetch(&src->kernel_space_raw, 0);
    dst->user_niced_raw = __sync_add_and_fetch(&src->user_niced_raw, 0);
    dst->user_space_raw = __sync_add_and_fetch(&src->user_space_raw, 0);
    dst->idle_time_raw = __sync_add_and_fetch(&src->idle_time_raw, 0);
    dst->io_wait_raw = __sync_add_and_fetch(&src->io_wait_raw, 0);
    dst->total_raw = __sync_add_and_fetch(&src->total_raw, 0);
    dst->soft_ints = __sync_add_and_fetch(&src->soft_ints, 0);
    dst->hard_ints = __sync_add_and_fetch(&src->hard_ints, 0);
    dst->kernel_space = __sync_add_and_fetch(&src->kernel_space, 0);
    dst->user_niced = __sync_add_and_fetch(&src->user_niced, 0);
    dst->user_space = __sync_add_and_fetch(&src->user_space, 0);
    dst->idle_time = __sync_add_and_fetch(&src->idle_time, 0);
    dst->io_wait = __sync_add_and_fetch(&src->io_wait, 0);
    dst->cpu_id = __sync_add_and_fetch(&src->cpu_id, 0);
}

bool bc_stats::update_cpu_info()
{
    char buffer[BC_FILE_MAX]; /* Load /proc/stat file */
    if (load_file(BC_FILE_STAT, buffer, sizeof(buffer)) <= 0) return false;

    /* Get last CPU usage by process */
    bc_stats::cpu::process last_usage;
    get_proc_usage(&last_usage);

    int core_count = __sync_add_and_fetch(&_cpu.core_count, 0);
    char *save_ptr = NULL;
    int cpu_id = -1;

    char *ptr = strtok_r(buffer, "\n", &save_ptr);
    while (ptr != NULL && !strncmp(ptr, "cpu", 3))
    {
        bc_stats::cpu_info info;
        memset(&info, 0, sizeof(bc_stats::cpu_info));

        sscanf(ptr, "%*s %u %u %u %u %u %u %u", &info.user_space_raw, 
            &info.user_niced_raw, &info.kernel_space_raw, &info.idle_time_raw, 
            &info.io_wait_raw, &info.hard_ints_raw, &info.soft_ints_raw);

        info.total_raw = info.kernel_space_raw + info.user_space_raw + info.user_niced_raw;
        info.total_raw += info.hard_ints_raw + info.soft_ints_raw;
        info.total_raw += info.idle_time_raw + info.io_wait_raw;
        info.cpu_id = cpu_id++;

        if (!core_count && info.cpu_id >= 0)
        {
            _cpu.cores.push_back(info);
        }
        else
        {
            bc_stats::cpu_info last_info, *curr_info;

            if (info.cpu_id < 0) curr_info = &_cpu.sum;
            else curr_info = &_cpu.cores[info.cpu_id];

            copy_cpu_info(&last_info, curr_info);
            uint32_t total_diff = info.total_raw - last_info.total_raw;

            float fHardInterrupts = ((info.hard_ints_raw - last_info.hard_ints_raw) / (float)total_diff) * 100;
            float fSoftInterrupts = ((info.soft_ints_raw - last_info.soft_ints_raw) / (float)total_diff) * 100;
            float fKernelSpace = ((info.kernel_space_raw - last_info.kernel_space_raw) / (float)total_diff) * 100;
            float fUserSpace = ((info.user_space_raw - last_info.user_space_raw) / (float)total_diff) * 100;
            float fUserNiced = ((info.user_niced_raw - last_info.user_niced_raw) / (float)total_diff) * 100;
            float fIdleTime = ((info.idle_time_raw - last_info.idle_time_raw) / (float)total_diff) * 100;
            float fIOWait = ((info.io_wait_raw - last_info.io_wait_raw) / (float)total_diff) * 100;

            __sync_lock_test_and_set(&curr_info->hard_ints, bc_float_to_u32(fHardInterrupts));
            __sync_lock_test_and_set(&curr_info->soft_ints, bc_float_to_u32(fSoftInterrupts));
            __sync_lock_test_and_set(&curr_info->kernel_space, bc_float_to_u32(fKernelSpace));
            __sync_lock_test_and_set(&curr_info->user_space, bc_float_to_u32(fUserSpace));
            __sync_lock_test_and_set(&curr_info->user_niced, bc_float_to_u32(fUserNiced));
            __sync_lock_test_and_set(&curr_info->idle_time, bc_float_to_u32(fIdleTime));
            __sync_lock_test_and_set(&curr_info->io_wait, bc_float_to_u32(fIOWait));

            /* Raw information about CPU usage for later percentage calculations */
            __sync_lock_test_and_set(&curr_info->hard_ints_raw, info.hard_ints_raw);
            __sync_lock_test_and_set(&curr_info->soft_ints_raw, info.soft_ints_raw);
            __sync_lock_test_and_set(&curr_info->kernel_space_raw, info.kernel_space_raw);
            __sync_lock_test_and_set(&curr_info->user_space_raw, info.user_space_raw);
            __sync_lock_test_and_set(&curr_info->user_niced_raw, info.user_niced_raw);
            __sync_lock_test_and_set(&curr_info->idle_time_raw, info.idle_time_raw);
            __sync_lock_test_and_set(&curr_info->io_wait_raw, info.io_wait_raw);
            __sync_lock_test_and_set(&curr_info->total_raw, info.total_raw);
            __sync_lock_test_and_set(&curr_info->cpu_id, info.cpu_id);
        }

        ptr = strtok_r(NULL, "\n", &save_ptr);
    }

    __sync_lock_test_and_set(&_cpu.core_count, _cpu.cores.size());
    if (load_file(BC_FILE_SELFSTAT, buffer, sizeof(buffer)) <= 0) return false;

    bc_stats::cpu::process curr_usage;
    sscanf(buffer, "%*u %*s %*c %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %lu %lu %ld %ld",
                    &curr_usage.user_space, &curr_usage.kernel_space, 
                    &curr_usage.user_childs, &curr_usage.kernel_childs);

    curr_usage.total_time = __sync_add_and_fetch(&_cpu.sum.total_raw, 0);
    uint64_t total_diff = curr_usage.total_time - last_usage.total_time;

    float nUserCPU = 100 * (((curr_usage.user_space + curr_usage.user_childs) - 
        (last_usage.user_space + last_usage.user_childs)) / (float)total_diff);

    float nSystemCPU = 100 * (((curr_usage.kernel_space + curr_usage.kernel_childs) - 
        (last_usage.kernel_space + last_usage.kernel_childs)) / (float)total_diff);

    __sync_lock_test_and_set(&_cpu.proc.user_childs, curr_usage.user_childs);
    __sync_lock_test_and_set(&_cpu.proc.kernel_childs, curr_usage.user_childs);
    __sync_lock_test_and_set(&_cpu.proc.user_space, curr_usage.user_space);
    __sync_lock_test_and_set(&_cpu.proc.kernel_space, curr_usage.kernel_space);
    __sync_lock_test_and_set(&_cpu.proc.total_time, curr_usage.total_time);
    __sync_lock_test_and_set(&_cpu.proc.user_space_usg, bc_float_to_u32(nUserCPU));
    __sync_lock_test_and_set(&_cpu.proc.kernel_space_usg, bc_float_to_u32(nSystemCPU));

    float one_min_int, five_min_int, ten_min_int;
    if (load_file(BC_FILE_LOADAVG, buffer, sizeof(buffer)) <= 0) return false;
    sscanf(buffer, "%f %f %f", &one_min_int, &five_min_int, &ten_min_int);

    __sync_lock_test_and_set(&_cpu.load_avg[0], bc_float_to_u32(one_min_int));
    __sync_lock_test_and_set(&_cpu.load_avg[1], bc_float_to_u32(five_min_int));
    __sync_lock_test_and_set(&_cpu.load_avg[2], bc_float_to_u32(ten_min_int));

    return true;
}

bool bc_stats::get_cpu_info(bc_stats::cpu *cpu)
{
    cpu->load_avg[0] = cpu->load_avg[1] = cpu->load_avg[2] = 0;
    int i, core_count = __sync_add_and_fetch(&_cpu.core_count, 0);
    if (core_count <= 0) return false;

    get_proc_usage(&cpu->proc);
    copy_cpu_info(&cpu->sum, &_cpu.sum);

    for (i = 0; i < core_count; i++)
    {
        bc_stats::cpu_info dst_info;
        copy_cpu_info(&dst_info, &_cpu.cores[i]);
        cpu->cores.push_back(dst_info);
    }

    cpu->core_count = cpu->cores.size();
    cpu->load_avg[0] = __sync_add_and_fetch(&_cpu.load_avg[0], 0);
    cpu->load_avg[1] = __sync_add_and_fetch(&_cpu.load_avg[1], 0);
    cpu->load_avg[2] = __sync_add_and_fetch(&_cpu.load_avg[2], 0);
    return cpu->core_count ? true : false;
}

void bc_stats::get_net_info(bc_stats::network *net)
{
    pthread_mutex_lock(&_mutex);
    *net = _network;
    pthread_mutex_unlock(&_mutex);
}

bool bc_stats::update_net_info()
{
    pthread_mutex_lock(&_mutex);

    DIR *net_dir = opendir(BC_DIR_NETWORK);
    if (net_dir == NULL)
    {
        pthread_mutex_unlock(&_mutex);
        return false;
    }

    struct dirent *dir_entry = readdir(net_dir);
    while(dir_entry != NULL) 
    {
        /* Found an entry, but ignore . and .. */
        if (!strcmp(".", dir_entry->d_name) || 
            !strcmp("..", dir_entry->d_name))
        {
            dir_entry = readdir(net_dir);
            continue;
        }

        char buffer[BC_FILE_MAX];
        char iface_path[PATH_MAX];

        net_iface iface;
        iface.name = std::string(dir_entry->d_name);

        snprintf(iface_path, sizeof(iface_path), "%s%s/address", BC_DIR_NETWORK, iface.name.c_str());
        if (load_file(iface_path, buffer, sizeof(buffer)) > 0)
        {
            char *saveptr = NULL;
            strtok_r(buffer, "\n", &saveptr);
            if (buffer[0] == '\n') buffer[0] = 0;
            iface.hwaddr = std::string(buffer);
        }

        snprintf(iface_path, sizeof(iface_path), "%s%s/type", BC_DIR_NETWORK, iface.name.c_str());
        if (load_file(iface_path, buffer, sizeof(buffer)) > 0) iface.type = atol(buffer);

        snprintf(iface_path, sizeof(iface_path), "%s%s/statistics/rx_bytes", BC_DIR_NETWORK, iface.name.c_str());
        if (load_file(iface_path, buffer, sizeof(buffer)) > 0) iface.bytes_recv = atol(buffer);

        snprintf(iface_path, sizeof(iface_path), "%s%s/statistics/tx_bytes", BC_DIR_NETWORK, iface.name.c_str());
        if (load_file(iface_path, buffer, sizeof(buffer)) > 0) iface.bytes_sent = atol(buffer);

        snprintf(iface_path, sizeof(iface_path), "%s%s/statistics/rx_packets", BC_DIR_NETWORK, iface.name.c_str());
        if (load_file(iface_path, buffer, sizeof(buffer)) > 0) iface.pkts_recv = atol(buffer);

        snprintf(iface_path, sizeof(iface_path), "%s%s/statistics/tx_packets", BC_DIR_NETWORK, iface.name.c_str());
        if (load_file(iface_path, buffer, sizeof(buffer)) > 0) iface.pkts_sent = atol(buffer);

        network_it it = _network.find(iface.name.c_str());
        if (it != _network.end())
        {
            if (iface.bytes_recv > it->second.bytes_recv && it->second.bytes_recv > 0)
                iface.bytes_recv_per_sec = (iface.bytes_recv - it->second.bytes_recv) / _monitoring_interval;
    
            if (iface.pkts_recv > it->second.pkts_recv && it->second.pkts_recv > 0)
                iface.pkts_recv_per_sec = (iface.pkts_recv - it->second.pkts_recv) / _monitoring_interval;
    
            if (iface.bytes_sent > it->second.bytes_sent && it->second.bytes_sent > 0)
                iface.bytes_sent_per_sec = (iface.bytes_sent - it->second.bytes_sent) / _monitoring_interval;
    
            if (iface.pkts_sent > it->second.pkts_sent && it->second.pkts_sent > 0)
                iface.pkts_sent_per_sec = (iface.pkts_sent - it->second.pkts_sent) / _monitoring_interval;

            iface.ipaddr = get_iface_ip_addr(iface.name.c_str());
            it->second = iface;
        }
        else
        {
            /* Insert new value into unordered map */
            iface.ipaddr = get_iface_ip_addr(iface.name.c_str());
            _network.insert(std::pair<std::string,net_iface>(iface.name.c_str(), iface));
        }

        dir_entry = readdir(net_dir);
    }

    closedir(net_dir);
    pthread_mutex_unlock(&_mutex);

    return true;
}

void bc_stats::display()
{
    bc_stats::memory mem;
    get_mem_info(&mem);

    bc_log(Debug, "memory: avail(%lu), total(%lu), free(%lu), swap(%lu), buff(%lu)",
        mem.avail, mem.total, mem.free, mem.swap, mem.buff);

    bc_stats::cpu cpu;
    get_cpu_info(&cpu);

    bc_log(Debug, "process: mem-res(%lu), mem-virt(%lu), cpu-us(%.2f), cpu-ks(%.2f)", 
        mem.resident, mem.virt,
        bc_u32_to_float(cpu.proc.user_space_usg), 
        bc_u32_to_float(cpu.proc.kernel_space_usg));

    bc_log(Debug, "loadavg: 5m(%.2f), 10m(%.2f), 15m(%.2f),\n", bc_u32_to_float(cpu.load_avg[0]), 
        bc_u32_to_float(cpu.load_avg[1]), bc_u32_to_float(cpu.load_avg[2]));
    
    bc_log(Debug, "core(s): us(%.2f), un(%.2f), ks(%.2f), idl(%.2f), si(%.2f), hi(%.2f), io(%.2f)", 
        bc_u32_to_float(cpu.sum.user_space), bc_u32_to_float(cpu.sum.user_niced), 
        bc_u32_to_float(cpu.sum.kernel_space), bc_u32_to_float(cpu.sum.idle_time), 
        bc_u32_to_float(cpu.sum.hard_ints), bc_u32_to_float(cpu.sum.soft_ints), 
        bc_u32_to_float(cpu.sum.io_wait));

    for (uint i = 0; i < cpu.cores.size(); i++)
    {
        bc_stats::cpu_info *core = &cpu.cores[i];
        bc_log(Debug, "core(%d): us(%.2f), un(%.2f), ks(%.2f), idl(%.2f), si(%.2f), hi(%.2f), io(%.2f)", 
            core->cpu_id, bc_u32_to_float(core->user_space), bc_u32_to_float(core->user_niced), 
            bc_u32_to_float(core->kernel_space), bc_u32_to_float(core->idle_time), 
            bc_u32_to_float(core->hard_ints), bc_u32_to_float(core->soft_ints), 
            bc_u32_to_float(core->io_wait));
    }
}

void bc_stats::get_storage_info(std::vector<storage_path> *storage_paths)
{
    pthread_mutex_lock(&_mutex);
    *storage_paths = _storage_paths;
    pthread_mutex_unlock(&_mutex);
}

bool bc_stats::update_storage_info()
{
    pthread_mutex_lock(&_mutex);
    
    std::vector<storage_path> new_storage_paths;
    std::set<std::string> unique_mounts;

    // Always include the default storage path
    std::vector<std::string> storage_paths = {"/var/lib/bluecherry/recordings"};

    // Query the Storage table for user-defined paths
    BC_DB_RES dbres = bc_db_get_table("SELECT path FROM Storage");
    if (dbres) {
        while (!bc_db_fetch_row(dbres)) {
            const char *path = bc_db_get_val(dbres, "path", NULL);
            if (path && *path) {
                storage_paths.push_back(std::string(path));
            }
        }
        bc_db_free_table(dbres);
    }

    // Open /proc/mounts to map mount points
    FILE *mtab = setmntent("/proc/mounts", "r");
    if (!mtab) {
        pthread_mutex_unlock(&_mutex);
        return false;
    }

    // Build a map from mount point to filesystem type
    std::map<std::string, std::string> mount_map;
    struct mntent *entry;
    while ((entry = getmntent(mtab)) != NULL) {
        // Skip non-local filesystems and special filesystems
        if (strcmp(entry->mnt_type, "proc") == 0 || 
            strcmp(entry->mnt_type, "sysfs") == 0 ||
            strcmp(entry->mnt_type, "devpts") == 0 ||
            strcmp(entry->mnt_type, "tmpfs") == 0 ||
            strcmp(entry->mnt_type, "devtmpfs") == 0 ||
            strcmp(entry->mnt_type, "cgroup") == 0 ||
            strncmp(entry->mnt_type, "fuse.", 5) == 0) {
            continue;
        }
        mount_map[std::string(entry->mnt_dir)] = std::string(entry->mnt_type);
    }
    endmntent(mtab);

    // For each storage path, resolve its mount point
    for (const auto& path : storage_paths) {
        char resolved[PATH_MAX];
        if (!realpath(path.c_str(), resolved)) {
            continue;
        }
        std::string best_mount = "/";
        size_t best_len = 0;
        for (const auto& m : mount_map) {
            if (strncmp(resolved, m.first.c_str(), m.first.length()) == 0) {
                if (m.first.length() > best_len) {
                    best_mount = m.first;
                    best_len = m.first.length();
                }
            }
        }
        unique_mounts.insert(best_mount);
    }

    // Always include root mount
    unique_mounts.insert("/");

    // For each unique mount, collect stats
    for (const auto& mount : unique_mounts) {
        struct statvfs vfs;
        if (statvfs(mount.c_str(), &vfs) == 0) {
            storage_path path_info;
            path_info.path = mount;
            path_info.filesystem = mount_map.count(mount) ? mount_map[mount] : "unknown";
            uint64_t block_size = vfs.f_frsize ? vfs.f_frsize : vfs.f_bsize;
            path_info.total_size = (uint64_t)vfs.f_blocks * block_size;
            path_info.free_size = (uint64_t)vfs.f_bavail * block_size;
            path_info.used_size = path_info.total_size - path_info.free_size;
            if (path_info.total_size > 0) {
                path_info.usage_percent = (uint32_t)((path_info.used_size * 100) / path_info.total_size);
            } else {
                path_info.usage_percent = 0;
            }
            new_storage_paths.push_back(path_info);
        }
    }

    // Update the storage paths
    _storage_paths = new_storage_paths;
    
    pthread_mutex_unlock(&_mutex);
    return true;
}

void bc_stats::monithoring_thread()
{
    pthread_setname_np(pthread_self(), "MONITORING");
    int history_counter = 0;
    int storage_counter = 0;
    int rrd_counter = 0;  // Counter for RRD updates
    
    while (!__sync_add_and_fetch(&_cancel, 0))
    {
        update_mem_info();
        update_cpu_info();
        update_net_info();
        
        // Update storage info every 10 seconds (less frequent due to statvfs calls)
        storage_counter++;
        if (storage_counter >= 10) {
            update_storage_info();
            storage_counter = 0;
        }

        // Add to history every 2 seconds (for historical data)
        history_counter++;
        if (history_counter >= 2) {
            add_history_entry();
            history_counter = 0;
        }

        // Update RRD every 10 seconds (less frequent to reduce I/O)
        rrd_counter++;
        if (rrd_counter >= 10) {
            update_rrd_data();
            rrd_counter = 0;
        }

        //display();
        sleep(1);
    }

    __sync_lock_test_and_set(&_active, 0);
}

void bc_stats::start_monithoring()
{
    pthread_mutex_init(&_mutex, NULL);
    __sync_lock_test_and_set(&_active, 1);

    // Initialize RRD file if it doesn't exist
    initialize_rrd_file();

    _thread = std::thread(&bc_stats::monithoring_thread, this);
}

void bc_stats::stop_monithoring()
{
    /* Notify thread about finish processing */
    __sync_lock_test_and_set(&_cancel, 1);

    _thread.join();
    /* Destroy mutex */
    pthread_mutex_destroy(&_mutex);
}

// Historical data methods implementation
void bc_stats::add_history_entry()
{
    std::lock_guard<std::mutex> lock(_history_mutex);
    
    stats_history_entry entry;
    entry.timestamp = std::chrono::system_clock::now();
    
    // Copy current memory data
    entry.memory_data = _memory;
    
    // Copy current CPU data
    entry.cpu_data = _cpu;
    
    // Copy current network data
    pthread_mutex_lock(&_mutex);
    entry.network_data = _network;
    entry.storage_data = _storage_paths;
    pthread_mutex_unlock(&_mutex);
    
    // Add to history
    _history.push_back(entry);
    
    // Remove old entries if we exceed the maximum
    if (_history.size() > MAX_HISTORY_ENTRIES) {
        _history.pop_front();
    }
}

std::vector<stats_history_entry> bc_stats::get_history_entries(size_t count)
{
    std::lock_guard<std::mutex> lock(_history_mutex);
    
    std::vector<stats_history_entry> result;
    size_t available = _history.size();
    size_t to_return = (count < available) ? count : available;
    
    if (to_return == 0) {
        return result;
    }
    
    // Get the most recent entries
    auto start_it = _history.end() - to_return;
    result.assign(start_it, _history.end());
    
    return result;
}

void bc_stats::clear_old_history()
{
    std::lock_guard<std::mutex> lock(_history_mutex);
    
    // Keep only the last MAX_HISTORY_ENTRIES
    while (_history.size() > MAX_HISTORY_ENTRIES) {
        _history.pop_front();
    }
}

size_t bc_stats::get_history_size() const
{
    std::lock_guard<std::mutex> lock(_history_mutex);
    return _history.size();
}

void bc_stats::initialize_rrd_file()
{
    // Try multiple possible RRD locations
    const char* rrd_paths[] = {
        "/var/lib/bluecherry/monitor.rrd",
        "/tmp/bluecherry-monitor.rrd",
        "/var/tmp/bluecherry-monitor.rrd"
    };
    
    const char* rrd_path = nullptr;
    
    // Find the first writable location
    for (const char* path : rrd_paths) {
        FILE* test_file = fopen(path, "r");
        if (test_file) {
            fclose(test_file);
            bc_log(Info, "RRD file %s already exists", path);
            return;
        }
        
        // Try to create the directory if it doesn't exist
        char dir_path[256];
        snprintf(dir_path, sizeof(dir_path), "%s", path);
        char* last_slash = strrchr(dir_path, '/');
        if (last_slash) {
            *last_slash = '\0';
            mkdir(dir_path, 0755); // Ignore errors
        }
        
        // Test if we can write to this location
        FILE* test_write = fopen(path, "w");
        if (test_write) {
            fclose(test_write);
            unlink(path); // Remove the test file
            rrd_path = path;
            break;
        }
    }
    
    if (!rrd_path) {
        bc_log(Error, "No writable location found for RRD file");
        return;
    }
    
    // Create RRD file with appropriate data sources and archives (10-second step)
    char create_cmd[512];
    snprintf(create_cmd, sizeof(create_cmd),
        "rrdtool create %s --step 10 "
        "DS:cpu:GAUGE:20:0:100 "
        "DS:mem:GAUGE:20:0:100 "
        "DS:disk:GAUGE:20:0:100 "
        "RRA:AVERAGE:0.5:1:360 "     /* 1 hour of 10-second data */
        "RRA:AVERAGE:0.5:6:1440 "    /* 24 hours of 1-minute data */
        "RRA:AVERAGE:0.5:30:288 "    /* 1 day of 5-minute data */
        "RRA:AVERAGE:0.5:360:168 "   /* 1 week of 1-hour data */
        "RRA:AVERAGE:0.5:8640:30",   /* 1 month of 1-day data */
        rrd_path);
    
    int result = system(create_cmd);
    if (result == 0) {
        bc_log(Info, "Successfully created RRD file %s", rrd_path);
    } else {
        bc_log(Error, "Failed to create RRD file %s (exit code: %d)", rrd_path, result);
    }
}

void bc_stats::update_rrd_data()
{
    // Prepare values for RRD update (CPU, memory, disk usage)
    float cpu_percent = 0.0f;
    if (_cpu.cores.size() > 0) {
        // Convert encoded CPU values to percentages
        float user_space = bc_u32_to_float(_cpu.cores[0].user_space);
        float kernel_space = bc_u32_to_float(_cpu.cores[0].kernel_space);
        float user_niced = bc_u32_to_float(_cpu.cores[0].user_niced);
        cpu_percent = user_space + kernel_space + user_niced;
    }
    float mem_percent = 0.0f;
    if (_memory.total > 0) {
        // Use MemAvailable for consistency with web UI
        mem_percent = 100.0f * (float)(_memory.total - _memory.avail) / (float)_memory.total;
    }
    float disk_percent = 0.0f;
    if (!_storage_paths.empty()) {
        disk_percent = (float)_storage_paths[0].usage_percent;
    }
    
    // Try multiple possible RRD locations
    const char* rrd_paths[] = {
        "/var/lib/bluecherry/monitor.rrd",
        "/tmp/bluecherry-monitor.rrd",
        "/var/tmp/bluecherry-monitor.rrd"
    };
    
    bool updated = false;
    for (const char* rrd_path : rrd_paths) {
        char rrd_cmd[256];
        snprintf(rrd_cmd, sizeof(rrd_cmd),
            "rrdtool update %s N:%.2f:%.2f:%.2f 2>/dev/null",
            rrd_path, cpu_percent, mem_percent, disk_percent);
        
        int result = system(rrd_cmd);
        if (result == 0) {
            updated = true;
            break;
        }
    }
    
    if (!updated) {
        bc_log(Debug, "Failed to update RRD file (all locations failed)");
    }
}
