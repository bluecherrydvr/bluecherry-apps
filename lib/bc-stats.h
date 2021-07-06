#ifndef __BC_STATS__
#define __BC_STATS__

#include <unordered_map>
#include <vector>

#include <inttypes.h>

#define _BC_ALIGNED_ __attribute__((aligned))

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

    static void copy_cpu_info(bc_stats::cpu_info *dst, bc_stats::cpu_info *src);
    static uint64_t parse_info(char *buffer, size_t size, const char *field);
    static char* load_file(const char *path, size_t *size);

    static uint32_t bc_float_to_u32(float value);
    static float bc_u32_to_float(uint32_t value);

    typedef std::vector<cpu_info*> cpu_infos;

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

        uint32_t _BC_ALIGNED_ load_avg[3];
        uint16_t _BC_ALIGNED_ core_count;

        cpu_infos cores;
        cpu_info sum;
    };

    void get_proc_usage(bc_stats::cpu::process *proc);
    void get_mem_info(bc_stats::memory *mem);
    void get_cpu_info(bc_stats::cpu *cpu);

private:
    bool update_mem_info();
    bool update_cpu_info();

    bc_stats::memory    _memory;
    bc_stats::cpu       _cpu;
};

#endif /* __BC_STATS__ */