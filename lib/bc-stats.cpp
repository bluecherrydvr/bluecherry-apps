#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>

#include "bc-stats.h"
#include "logc.h"

#define BC_FILE_SELFSTATUS       "/proc/self/status"
#define BC_FILE_SELFSTAT         "/proc/self/stat"
#define BC_FILE_MEMINFO          "/proc/meminfo"
#define BC_FILE_LOADAVG          "/proc/loadavg"
#define BC_FILE_STAT             "/proc/stat"

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

char* bc_stats::load_file(const char *path, size_t *size)
{
    /* Open target file for reading only */
	int fd = open(path, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
	if (fd < 0) return 0;

    /* Get file size */
    size_t length = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET); // rewind back

    char *buffer = (char*)malloc(length + 1);
    if (buffer == NULL)
    {
        bc_log(Error, "Can not allocate memory for reading file: %s", path);
        close(fd);
        return NULL;
    }

    /* Read whole file buffer from descriptor */
	int bytes = read(fd, buffer, length);
    if (bytes <= 0)
    {
        bc_log(Error, "Can not read file: %s (%s)", path, strerror(errno));
        free(buffer);
        close(fd);
        return NULL;
    }

    /* Null terminate buffer */
	buffer[bytes] = '\0';
	close(fd);

    *size = bytes;
	return buffer;
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
    size_t file_size = 0;
    char *buffer = NULL;

    /* Load /proc/meminfo file */
    buffer = load_file(BC_FILE_MEMINFO, &file_size);
    if (buffer == NULL) return false;

    /* Parse memory statistics */
    __sync_lock_test_and_set(&_memory.total, parse_info(buffer, file_size, "MemTotal"));
    __sync_lock_test_and_set(&_memory.free, parse_info(buffer, file_size, "MemFree"));
    __sync_lock_test_and_set(&_memory.avail, parse_info(buffer, file_size, "MemAvailable"));
    __sync_lock_test_and_set(&_memory.buff, parse_info(buffer, file_size, "Buffers"));
    __sync_lock_test_and_set(&_memory.swap, parse_info(buffer, file_size, "SwapCached"));

    free(buffer); /* Load /proc/self/status file */
    buffer = load_file(BC_FILE_SELFSTATUS, &file_size);
    if (buffer == NULL) return false;

    /* Parse memory statistics for current process */
    __sync_lock_test_and_set(&_memory.resident, parse_info(buffer, file_size, "VmRSS"));
    __sync_lock_test_and_set(&_memory.virt, parse_info(buffer, file_size, "VmSize"));

    free(buffer);
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
    dst->soft_ints_raw = __sync_add_and_fetch(&src->soft_ints_raw, 0);
    dst->hard_ints_raw = __sync_add_and_fetch(&src->hard_ints_raw, 0);
    dst->kernel_space = __sync_add_and_fetch(&src->kernel_space, 0);
    dst->user_niced = __sync_add_and_fetch(&src->user_niced, 0);
    dst->user_space = __sync_add_and_fetch(&src->user_space, 0);
    dst->idle_time = __sync_add_and_fetch(&src->idle_time, 0);
    dst->io_wait = __sync_add_and_fetch(&src->io_wait, 0);
    dst->cpu_id = __sync_add_and_fetch(&src->cpu_id, 0);
}

bool bc_stats::update_cpu_info()
{
    size_t file_size = 0;
    char *buffer = NULL;

    /* Load /proc/stat file */
    buffer = load_file(BC_FILE_STAT, &file_size);
    if (buffer == NULL) return false;

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
            bc_stats::cpu_info *pinfo = new bc_stats::cpu_info;
            if (pinfo != NULL)
            {
                memcpy(pinfo, &info, sizeof(bc_stats::cpu_info));
                _cpu.cores.push_back(pinfo);
            }
        }
        else
        {
            bc_stats::cpu_info last_info, *curr_info;

            if (info.cpu_id < 0) curr_info = &_cpu.sum;
            else curr_info = _cpu.cores[info.cpu_id];

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

    free(buffer);
    __sync_lock_test_and_set(&_cpu.core_count, _cpu.cores.size());

    buffer = load_file(BC_FILE_SELFSTAT, &file_size);
    if (buffer == NULL) return false;

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

    free(buffer);
    buffer = load_file(BC_FILE_LOADAVG, &file_size);
    if (buffer == NULL) return false;

    float one_min_int, five_min_int, ten_min_int;
    sscanf(buffer, "%f %f %f", &one_min_int, &five_min_int, &ten_min_int);

    __sync_lock_test_and_set(&_cpu.load_avg[0], bc_float_to_u32(one_min_int));
    __sync_lock_test_and_set(&_cpu.load_avg[1], bc_float_to_u32(five_min_int));
    __sync_lock_test_and_set(&_cpu.load_avg[2], bc_float_to_u32(ten_min_int));

    free(buffer);
    return true;
}