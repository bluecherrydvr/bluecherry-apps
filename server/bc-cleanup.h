#ifndef BC_CLEANUP_H
#define BC_CLEANUP_H

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <memory>

// Constants for cleanup configuration
#define CLEANUP_BATCH_SIZE 50  // Reduced from 200 to be more I/O friendly
#define MAX_CLEANUP_TIME 300  // 5 minutes
#define MAX_RETRY_COUNT 5
#define RETRY_BACKOFF_BASE 5  // seconds
#define HIGH_PRIORITY_BATCH_SIZE 200  // Reduced from 10000 to be more I/O friendly
#define FILE_DELETE_DELAY_MS 100  // 100ms delay between file deletions
#define MAX_IO_LOAD 80  // Maximum I/O load percentage before pausing

// Cleanup statistics structure
struct cleanup_stats {
    std::atomic<int> files_processed{0};
    std::atomic<int> files_deleted{0};
    std::atomic<int> errors{0};
    std::atomic<size_t> bytes_freed{0};
    std::atomic<int> retries{0};
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
    std::mutex queue_mutex;
    
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
    cleanup_stats stats;
    
public:
    ParallelCleanup();
    ~ParallelCleanup();
    
    void start_cleanup(int num_threads);
    void stop_cleanup();
    void add_work(const std::string& filepath);
    cleanup_stats get_stats() const;
    
private:
    void worker_thread();
    std::string get_next_file();
    void process_file(const std::string& filepath);
};

// Cleanup scheduler class
class CleanupScheduler {
private:
    std::chrono::system_clock::time_point last_run;
    std::chrono::minutes interval;
    std::chrono::minutes high_priority_interval;  // New interval for critical storage
    std::mutex scheduler_mutex;
    
public:
    CleanupScheduler(std::chrono::minutes interval = std::chrono::minutes(5));
    bool should_run_cleanup();
    void update_last_run();
    void set_interval(std::chrono::minutes new_interval);
};

// Main cleanup manager class
class CleanupManager {
private:
    std::unique_ptr<CleanupRetryManager> retry_manager;
    std::unique_ptr<ParallelCleanup> parallel_cleanup;
    std::unique_ptr<CleanupScheduler> scheduler;
    cleanup_stats stats;
    std::mutex stats_mutex;
    
public:
    CleanupManager();
    ~CleanupManager();
    
    int run_cleanup();
    void update_stats(const cleanup_stats& new_stats);
    cleanup_stats get_stats() const;
    void reset_stats();
    
private:
    int process_batch(const std::vector<std::string>& files);
    bool should_continue_cleanup(time_t start_time);
};

#endif // BC_CLEANUP_H 