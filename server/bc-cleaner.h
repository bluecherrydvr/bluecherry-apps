#ifndef __BC_CLEANER__
#define __BC_CLEANER__

#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <memory>

struct bc_time {
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int min = 0;
	int sec = 0;
};

// Cleanup statistics structure
struct cleanup_stats_report {
    int files_processed = 0;
    int files_deleted = 0;
    int errors = 0;
    size_t bytes_freed = 0;
    int retries = 0;
};

// Main cleanup manager class
class CleanupManager {
public:
    CleanupManager();
    ~CleanupManager();
    
    int run_cleanup();
    cleanup_stats_report get_stats_report() const;
    
private:
    cleanup_stats_report stats;
    int process_batch(const std::vector<std::string>& files);
    bool should_continue_cleanup(time_t start_time);
};

std::string bc_get_directory_path(const std::string& filePath);
std::string bc_get_file_name(const std::string& filePath);

bool bc_is_leap_year(int year);
int bc_days_in_month(int year, int month);
bool bc_compare_time(const bc_time& tm1, const bc_time& tm2);

bool bc_is_base_path(std::string dir_path);
bool bc_is_current_path(const std::string& path);

std::string bc_get_parent_path(const std::string& path);
std::string bc_get_dir_path(const std::string& path);

bool bc_is_dir_empty(const std::string& path);
bool bc_remove_dir_if_empty(const std::string& path);
int bc_remove_directory(const std::string& path);

#endif /* __BC_CLEANER__ */