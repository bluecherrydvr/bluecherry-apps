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

// Constants for cleanup configuration
constexpr int NORMAL_CLEANUP_INTERVAL = 1800;  // 30 minutes
constexpr int HIGH_PRIORITY_INTERVAL = 300;    // 5 minutes
constexpr int MAX_CLEANUP_TIME = 300;         // 5 minutes
constexpr int MAX_RETRY_COUNT = 5;
constexpr int RETRY_BACKOFF_BASE = 5;         // 5 seconds
constexpr float CRITICAL_STORAGE_THRESHOLD = 95.0f;

// Batch size configuration - optimized for large systems
constexpr int MIN_BATCH_SIZE = 50;           // Reduced from 100 for better responsiveness
constexpr int MAX_BATCH_SIZE = 2000;         // Reduced from 5000 to prevent long transactions
constexpr int DEFAULT_BATCH_SIZE = 200;      // Default batch size
#define BATCH_SIZE_MULTIPLIER 50             // Reduced from 100 - files per TB of storage

// New constants for load monitoring and adaptive behavior
constexpr int MAX_SYSTEM_LOAD = 10;          // Maximum system load before pausing cleanup
constexpr int MAX_MYSQL_CONNECTIONS = 50;    // Maximum MySQL connections before pausing
constexpr int BATCH_DELAY_MS = 100;          // Delay between batches in milliseconds
constexpr int SYNC_INTERVAL_BATCHES = 5;     // Sync filesystem every N batches
constexpr int TRANSACTION_TIMEOUT_SEC = 30;  // Maximum transaction time
constexpr int LOAD_CHECK_INTERVAL = 5;       // Check system load every N batches

// Cleanup statistics structure
struct cleanup_stats_report {
    int files_processed = 0;
    int files_deleted = 0;
    int errors = 0;
    size_t bytes_freed = 0;
    int retries = 0;
    int batches_processed = 0;
    int load_pauses = 0;
    int mysql_pauses = 0;
    double avg_batch_time = 0.0;
    mutable std::mutex stats_mutex;

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
        batches_processed = other.batches_processed;
        load_pauses = other.load_pauses;
        mysql_pauses = other.mysql_pauses;
        avg_batch_time = other.avg_batch_time;
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
            batches_processed = other.batches_processed;
            load_pauses = other.load_pauses;
            mysql_pauses = other.mysql_pauses;
            avg_batch_time = other.avg_batch_time;
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

    void increment_batches() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        batches_processed++;
    }

    void increment_load_pauses() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        load_pauses++;
    }

    void increment_mysql_pauses() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        mysql_pauses++;
    }

    void update_avg_batch_time(double batch_time) {
        std::lock_guard<std::mutex> lock(stats_mutex);
        if (batches_processed > 0) {
            avg_batch_time = ((avg_batch_time * (batches_processed - 1)) + batch_time) / batches_processed;
        } else {
            avg_batch_time = batch_time;
        }
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
        copy.batches_processed = batches_processed;
        copy.load_pauses = load_pauses;
        copy.mysql_pauses = mysql_pauses;
        copy.avg_batch_time = avg_batch_time;
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
    mutable std::mutex queue_mutex;
    
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
    mutable std::mutex scheduler_mutex;
    bool startup_cleanup_done;
    
public:
    CleanupScheduler();
    bool should_run_cleanup();
    void update_last_run();
    void set_interval(std::chrono::seconds new_interval);
    void set_high_priority_interval(std::chrono::seconds new_interval);
    bool needs_startup_cleanup() const;
    void mark_startup_cleanup_done();
};

// System load monitoring class
class LoadMonitor {
private:
    static double get_system_load();
    static int get_mysql_connections();
    
public:
    static bool is_system_overloaded();
    static bool is_mysql_overloaded();
    static void wait_if_overloaded(int max_wait_seconds = 30);
};

// Main cleanup manager class
class CleanupManager {
private:
    std::mutex cleanup_mutex;
    bool cleanup_in_progress;
    std::unique_ptr<CleanupRetryManager> retry_manager;
    std::unique_ptr<ParallelCleanup> parallel_cleanup;
    std::unique_ptr<CleanupScheduler> scheduler;
    std::chrono::system_clock::time_point last_cleanup_time;
    cleanup_stats_report stats;
    std::mutex stats_mutex;

    // Helper functions
    bool delete_media_file(const std::string& filepath, int id);
    bool check_storage_status();
    int get_adaptive_batch_size();
    bool process_batch(int batch_size, double target_threshold, int& total_deleted);
    void batch_delete_files(const std::vector<std::string>& files, int& deleted_count, size_t& bytes_freed);
    void commit_batch_changes(const std::vector<std::string>& deleted_files);
    int sync_database_with_filesystem();

public:
    CleanupManager();
    int run_cleanup();
    int run_database_sync();
    cleanup_stats_report get_stats_report() const;
    int calculate_batch_size();
    bool should_continue_cleanup(time_t start_time);
    void update_stats(const cleanup_stats_report& new_stats);
    bool should_run_cleanup() const;
    bool needs_startup_cleanup() const;
    void mark_startup_cleanup_done();
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
void scan_directory_for_files(const std::string& basepath, std::vector<std::string>& files, int& batch_size, int& files_found);
double bc_get_storage_usage();
double bc_get_total_storage_size();
std::vector<std::string> bc_get_media_files(int batch_size);