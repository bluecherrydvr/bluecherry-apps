#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "bc-stats.h"
#include "logc.h"

#define BC_FILE_MEMINFO          "/proc/meminfo"
#define BC_FILE_SELFSTAT         "/proc/self/status"

char* bc_stats::load_file(const char *path, size_t *size)
{
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

	int bytes = read(fd, buffer, length);
    if (bytes <= 0)
    {
        bc_log(Error, "Can not read file: %s (%s)", path, strerror(errno));
        free(buffer);
        close(fd);
        return NULL;
    }

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

    buffer = load_file(BC_FILE_MEMINFO, &file_size);
    if (buffer == NULL) return false;

    __sync_lock_test_and_set(&_memory.total, parse_info(buffer, file_size, "MemTotal"));
    __sync_lock_test_and_set(&_memory.free, parse_info(buffer, file_size, "MemFree"));
    __sync_lock_test_and_set(&_memory.avail, parse_info(buffer, file_size, "MemAvailable"));
    __sync_lock_test_and_set(&_memory.buff, parse_info(buffer, file_size, "Buffers"));
    __sync_lock_test_and_set(&_memory.swap, parse_info(buffer, file_size, "SwapCached"));

    free(buffer);
    buffer = load_file(BC_FILE_SELFSTAT, &file_size);
    if (buffer == NULL) return false;

    __sync_lock_test_and_set(&_memory.resident, parse_info(buffer, file_size, "VmRSS"));
    __sync_lock_test_and_set(&_memory.virt, parse_info(buffer, file_size, "VmSize"));

    return true;
}