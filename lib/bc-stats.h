#ifndef __BC_STATS__
#define __BC_STATS__

#include <unordered_map>
#include <inttypes.h>

#define _BC_ALIGNED_ __attribute__((aligned))

class bc_stats 
{
public:
    static uint64_t parse_info(char *buffer, size_t size, const char *field);
    static char* load_file(const char *path, size_t *size);

    struct memory {
        uint64_t _BC_ALIGNED_ resident = 0;
        uint64_t _BC_ALIGNED_ virt = 0;
        uint64_t _BC_ALIGNED_ avail = 0;
        uint64_t _BC_ALIGNED_ total = 0;
        uint64_t _BC_ALIGNED_ free = 0;
        uint64_t _BC_ALIGNED_ swap = 0;
        uint64_t _BC_ALIGNED_ buff = 0;
    };

    void get_mem_info(bc_stats::memory *mem);

private:
    bool update_mem_info();

    bc_stats::memory _memory;
};

#endif __BC_STATS__