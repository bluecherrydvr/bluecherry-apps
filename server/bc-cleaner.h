#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include "bc-time.h"  // For bc_time struct definition

// Forward declarations
double bc_get_storage_usage();
std::vector<std::string> bc_get_media_files(int batch_size);
double bc_get_total_storage_size();

// Constants for cleanup configuration
constexpr int NORMAL_CLEANUP_INTERVAL = 300;  // 5 minutes
constexpr int HIGH_PRIORITY_INTERVAL = 60;    // 1 minute
constexpr int MAX_CLEANUP_TIME = 300;         // 5 minutes
constexpr int MAX_RETRY_COUNT = 5;
constexpr int RETRY_BACKOFF_BASE = 5;         // 5 seconds
constexpr float CRITICAL_STORAGE_THRESHOLD = 95.0f;

// Batch size configuration
constexpr int MIN_BATCH_SIZE = 100;
constexpr int MAX_BATCH_SIZE = 5000;
#define BATCH_SIZE_MULTIPLIER 100    // Files per TB of storage

// Cleanup statistics structure
struct cleanup_stats_report {
    int files_processed = 0;
    int files_deleted = 0;
    int errors = 0;
    size_t bytes_freed = 0;
    int retries = 0;
    mutable std::mutex stats_mutex;  // Made mutable to allow locking in const methods

    // Delete copy constructor and assignment
    cleanup_stats_report(const cleanup_stats_report&) = delete;
    cleanup_stats_report& operator=(const cleanup_stats_report&) = delete;

    // Add move constructor and assignment
    cleanup_stats_report(cleanup_stats_report&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.stats_mutex);
        files_processed = other.files_processed;
        files_deleted = other.files_deleted;
        errors = other.errors;
        bytes_freed = other.bytes_freed;
        retries = other.retries;
    }

    cleanup_stats_report& operator=(cleanup_stats_report&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(stats_mutex);
            std::lock_guard<std::mutex> lock_other(other.stats_mutex);
            files_processed = other.files_processed;
            files_deleted = other.files_deleted;
            errors = other.errors;
            bytes_freed = other.bytes_freed;
            retries = other.retries;
        }
        return *this;
    }

    // Default constructor
    cleanup_stats_report() = default;

    void increment_processed() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        files_processed++;
    }

    void increment_deleted() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        files_deleted++;
    }

    void increment_errors() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        errors++;
    }

    void add_bytes_freed(size_t bytes) {
        std::lock_guard<std::mutex> lock(stats_mutex);
        bytes_freed += bytes;
    }

    void increment_retries() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        retries++;
    }

    // Create a new stats report with current values
    cleanup_stats_report get_copy() const {
        cleanup_stats_report copy;
        std::lock_guard<std::mutex> lock(stats_mutex);
        copy.files_processed = files_processed;
        copy.files_deleted = files_deleted;
        copy.errors = errors;
        copy.bytes_freed = bytes_freed;
        copy.retries = retries;
        return copy;
    }
};

// Retry entry structure
struct cleanup_retry_entry {
    std::string filepath;
    time_t last_attempt;
    int attempt_count;
    std::string error_reason;
};

// Cleanup retry manager class
class CleanupRetryManager {
private:
    std::map<std::string, cleanup_retry_entry> retry_queue;
    mutable std::mutex queue_mutex;  // Made mutable for const methods
    
public:
    void add_retry(const std::string& filepath, const std::string& error);
    void process_retries();
    size_t get_retry_count() const;
    void clear_retries();
};

// Parallel cleanup worker class
class ParallelCleanup {
private:
    std::vector<std::thread> workers;
    std::queue<std::string> work_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    bool should_stop;
    cleanup_stats_report stats;
    
public:
    ParallelCleanup();
    ~ParallelCleanup();
    
    void start_cleanup(int num_threads);
    void stop_cleanup();
    void add_work(const std::string& filepath);
    cleanup_stats_report get_stats() const;  // Will use get_copy() internally
    
private:
    void worker_thread();
    std::string get_next_file();
    void process_file(const std::string& filepath);
};

// Cleanup scheduler class
class CleanupScheduler {
private:
    std::chrono::system_clock::time_point last_run;
    std::chrono::seconds interval;
    std::chrono::seconds high_priority_interval;
    mutable std::mutex scheduler_mutex;  // Made mutable for const methods
    
public:
    CleanupScheduler();
    bool should_run_cleanup();
    void update_last_run();
    void set_interval(std::chrono::seconds new_interval);
    void set_high_priority_interval(std::chrono::seconds new_interval);
};

// Main cleanup manager class
class CleanupManager {
public:
    CleanupManager();
    ~CleanupManager();
    
    int run_cleanup();
    cleanup_stats_report get_stats_report() const;  // Will use get_copy() internally
    
private:
    std::unique_ptr<CleanupRetryManager> retry_manager;
    std::unique_ptr<ParallelCleanup> parallel_cleanup;
    std::unique_ptr<CleanupScheduler> scheduler;
    cleanup_stats_report stats;
    mutable std::mutex stats_mutex;  // Made mutable for const methods
    
    int process_batch(const std::vector<std::string>& files);
    bool should_continue_cleanup(time_t start_time);
    int calculate_batch_size();
    void update_stats(const cleanup_stats_report& new_stats);
};

// Utility functions
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