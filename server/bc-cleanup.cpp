#include "bc-cleanup.h"
#include "bc-server.h"
#include "bc-db.h"
#include "bc-log.h"
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <algorithm>
#include <cmath>

// CleanupRetryManager implementation
void CleanupRetryManager::add_retry(const std::string& filepath, const std::string& error) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    auto it = retry_queue.find(filepath);
    
    if (it == retry_queue.end()) {
        cleanup_retry_entry entry;
        entry.filepath = filepath;
        entry.last_attempt = time(NULL);
        entry.attempt_count = 1;
        entry.error_reason = error;
        retry_queue[filepath] = entry;
    } else {
        it->second.last_attempt = time(NULL);
        it->second.attempt_count++;
        it->second.error_reason = error;
    }
}

void CleanupRetryManager::process_retries() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    time_t now = time(NULL);
    
    for (auto it = retry_queue.begin(); it != retry_queue.end();) {
        int backoff = RETRY_BACKOFF_BASE * std::pow(2, it->second.attempt_count - 1);
        
        if (now - it->second.last_attempt >= backoff) {
            if (it->second.attempt_count >= MAX_RETRY_COUNT) {
                bc_log(Error, "Failed to delete file after %d attempts: %s",
                       it->second.attempt_count, it->second.filepath.c_str());
                it = retry_queue.erase(it);
            } else {
                if (unlink(it->second.filepath.c_str()) == 0) {
                    bc_log(Info, "Successfully deleted file on retry: %s",
                           it->second.filepath.c_str());
                    it = retry_queue.erase(it);
                } else {
                    it->second.last_attempt = now;
                    ++it;
                }
            }
        } else {
            ++it;
        }
    }
}

size_t CleanupRetryManager::get_retry_count() const {
    std::lock_guard<std::mutex> lock(queue_mutex);
    return retry_queue.size();
}

void CleanupRetryManager::clear_retries() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    retry_queue.clear();
}

// ParallelCleanup implementation
ParallelCleanup::ParallelCleanup() : should_stop(false) {}

ParallelCleanup::~ParallelCleanup() {
    stop_cleanup();
}

void ParallelCleanup::start_cleanup(int num_threads) {
    should_stop = false;
    for (int i = 0; i < num_threads; i++) {
        workers.emplace_back(&ParallelCleanup::worker_thread, this);
    }
}

void ParallelCleanup::stop_cleanup() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        should_stop = true;
    }
    queue_cv.notify_all();
    
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers.clear();
}

void ParallelCleanup::add_work(const std::string& filepath) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        work_queue.push(filepath);
    }
    queue_cv.notify_one();
}

cleanup_stats ParallelCleanup::get_stats() const {
    return stats;
}

void ParallelCleanup::worker_thread() {
    while (true) {
        std::string filepath = get_next_file();
        if (filepath.empty()) {
            break;
        }
        process_file(filepath);
    }
}

std::string ParallelCleanup::get_next_file() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue_cv.wait(lock, [this] { 
        return should_stop || !work_queue.empty(); 
    });
    
    if (should_stop && work_queue.empty()) {
        return "";
    }
    
    std::string filepath = work_queue.front();
    work_queue.pop();
    return filepath;
}

void ParallelCleanup::process_file(const std::string& filepath) {
    struct stat st;
    if (stat(filepath.c_str(), &st) == 0) {
        if (unlink(filepath.c_str()) == 0) {
            stats.files_deleted++;
            stats.bytes_freed += st.st_size;
        } else {
            stats.errors++;
        }
    }
    stats.files_processed++;
}

// CleanupScheduler implementation
CleanupScheduler::CleanupScheduler(std::chrono::minutes interval)
    : interval(interval), high_priority_interval(std::chrono::minutes(1)) {
    last_run = std::chrono::system_clock::now();
}

bool CleanupScheduler::should_run_cleanup() {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    auto now = std::chrono::system_clock::now();
    
    // Check if storage is critically full
    bool is_critically_full = false;
    for (int i = 0; i < MAX_STOR_LOCS && media_stor[i].max_thresh; i++) {
        float used = path_used_percent(media_stor[i].path);
        if (used >= 95.0) {
            is_critically_full = true;
            break;
        }
    }
    
    // Use shorter interval for critical storage
    auto current_interval = is_critically_full ? high_priority_interval : interval;
    return (now - last_run) >= current_interval;
}

void CleanupScheduler::update_last_run() {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    last_run = std::chrono::system_clock::now();
}

void CleanupScheduler::set_interval(std::chrono::minutes new_interval) {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    interval = new_interval;
}

// CleanupManager implementation
CleanupManager::CleanupManager() {
    retry_manager = std::make_unique<CleanupRetryManager>();
    parallel_cleanup = std::make_unique<ParallelCleanup>();
    scheduler = std::make_unique<CleanupScheduler>();
}

CleanupManager::~CleanupManager() = default;

int CleanupManager::run_cleanup() {
    if (!scheduler->should_run_cleanup()) {
        return 0;
    }
    
    time_t start_time = time(NULL);
    BC_DB_RES dbres;
    std::vector<std::string> batch;
    
    // Check if storage is critically full (above 95%)
    bool is_critically_full = false;
    float highest_usage = 0.0;
    for (int i = 0; i < MAX_STOR_LOCS && media_stor[i].max_thresh; i++) {
        float used = path_used_percent(media_stor[i].path);
        if (used > highest_usage) {
            highest_usage = used;
        }
        if (used >= 95.0) {
            is_critically_full = true;
            break;
        }
    }
    
    // Get files to clean up with appropriate batch size
    int batch_size = is_critically_full ? HIGH_PRIORITY_BATCH_SIZE : CLEANUP_BATCH_SIZE;
    
    if (is_critically_full) {
        bc_log(Info, "Storage critically full (%.1f%%), entering high priority cleanup mode. Batch size: %d files", 
               highest_usage, batch_size);
    }
    
    dbres = bc_db_get_table("SELECT * FROM Media WHERE archive=0 AND "
                           "filepath!='' ORDER BY id ASC LIMIT %d",
                           batch_size);
    
    if (!dbres) {
        bc_log(Error, "Database error during media cleanup");
        return -1;
    }
    
    while (!bc_db_fetch_row(dbres) && should_continue_cleanup(start_time)) {
        const char *filepath = bc_db_get_val(dbres, "filepath", NULL);
        if (filepath && *filepath) {
            batch.push_back(filepath);
        }
    }
    
    bc_db_free_table(dbres);
    
    if (is_critically_full) {
        bc_log(Info, "High priority cleanup: Found %zu files to process", batch.size());
    }
    
    // Process the batch
    int result = process_batch(batch);
    
    // Update scheduler
    scheduler->update_last_run();
    
    return result;
}

void CleanupManager::update_stats(const cleanup_stats& new_stats) {
    std::lock_guard<std::mutex> lock(stats_mutex);
    stats.files_processed += new_stats.files_processed;
    stats.files_deleted += new_stats.files_deleted;
    stats.errors += new_stats.errors;
    stats.bytes_freed += new_stats.bytes_freed;
    stats.retries += new_stats.retries;
}

cleanup_stats CleanupManager::get_stats() const {
    std::lock_guard<std::mutex> lock(stats_mutex);
    return stats;
}

void CleanupManager::reset_stats() {
    std::lock_guard<std::mutex> lock(stats_mutex);
    stats = cleanup_stats();
}

int CleanupManager::process_batch(const std::vector<std::string>& files) {
    if (files.empty()) {
        return 0;
    }
    
    // Start parallel cleanup
    parallel_cleanup->start_cleanup(std::thread::hardware_concurrency());
    
    // Add files to work queue
    for (const auto& file : files) {
        parallel_cleanup->add_work(file);
    }
    
    // Wait for completion
    parallel_cleanup->stop_cleanup();
    
    // Get stats before updating
    cleanup_stats new_stats = parallel_cleanup->get_stats();
    
    // Log cleanup results
    bc_log(Info, "Cleanup completed: %d files processed, %d deleted, %d errors, %.2f GB freed",
           new_stats.files_processed.load(),
           new_stats.files_deleted.load(),
           new_stats.errors.load(),
           new_stats.bytes_freed.load() / (1024.0 * 1024.0 * 1024.0));
    
    // Update stats
    update_stats(new_stats);
    
    return 0;
}

bool CleanupManager::should_continue_cleanup(time_t start_time) {
    return (time(NULL) - start_time) < MAX_CLEANUP_TIME;
} 