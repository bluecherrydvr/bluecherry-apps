#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <numeric>  // For std::accumulate
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <mysql/mysql.h>

#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bsd/string.h>

#include "bc-cleaner.h"
#include "bc-server.h"
#include "bc-syslog.h"
#include "../lib/libbluecherry.h"
#include "../lib/logging.h"
#include "../lib/logc.h"
#include <algorithm>
#include <cmath>

// Constants for cleanup configuration
#define CLEANUP_BATCH_SIZE 100
#define MAX_CLEANUP_TIME 300  // 5 minutes
#define MAX_RETRY_COUNT 3
#define RETRY_BACKOFF_BASE 1
#define MIN_SPACE_TO_FREE_GB 5  // Minimum space to free in GB
#define SAFETY_MARGIN_PERCENT 5  // Safety margin to prevent over-cleaning

// LoadMonitor implementation
double LoadMonitor::get_system_load() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        bc_log(Error, "Failed to get system load");
        return 0.0;
    }
    
    // Convert load average to a more readable format
    double load_avg = static_cast<double>(info.loads[0]) / (1 << SI_LOAD_SHIFT);
    return load_avg;
}

int LoadMonitor::get_mysql_connections() {
    // Try to get MySQL connection count from the database
    const char* sql = "SHOW STATUS LIKE 'Threads_connected'";
    BC_DB_RES res = bc_db_get_table("%s", sql);
    if (!res) {
        bc_log(Debug, "Could not get MySQL connection count");
        return 0;
    }
    
    int connections = 0;
    if (!bc_db_fetch_row(res)) {
        size_t len;
        const char* value = bc_db_get_val(res, "Value", &len);
        if (value) {
            connections = atoi(value);
        }
    }
    bc_db_free_table(res);
    return connections;
}

bool LoadMonitor::is_system_overloaded() {
    double load = get_system_load();
    return load > MAX_SYSTEM_LOAD;
}

bool LoadMonitor::is_mysql_overloaded() {
    int connections = get_mysql_connections();
    return connections > MAX_MYSQL_CONNECTIONS;
}

void LoadMonitor::wait_if_overloaded(int max_wait_seconds) {
    time_t start_time = time(nullptr);
    int wait_count = 0;
    
    while ((time(nullptr) - start_time) < max_wait_seconds) {
        if (is_system_overloaded()) {
            if (wait_count == 0) {
                bc_log(Info, "System load high (%.2f), pausing cleanup", get_system_load());
            }
            usleep(100000); // 100ms
            wait_count++;
            continue;
        }
        
        if (is_mysql_overloaded()) {
            if (wait_count == 0) {
                bc_log(Info, "MySQL connections high (%d), pausing cleanup", get_mysql_connections());
            }
            usleep(100000); // 100ms
            wait_count++;
            continue;
        }
        
        if (wait_count > 0) {
            bc_log(Info, "System load normalized, resuming cleanup after %d ms pause", wait_count * 100);
        }
        return;
    }
    
    bc_log(Warning, "System remained overloaded for %d seconds, continuing cleanup", max_wait_seconds);
}

// CleanupRetryManager implementation
void CleanupRetryManager::add_retry(const std::string& filepath, const std::string& error) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    cleanup_retry_entry entry;
    entry.filepath = filepath;
    entry.last_attempt = time(nullptr);
    entry.attempt_count = 1;
    entry.error_reason = error;
    retry_queue[filepath] = entry;
}

void CleanupRetryManager::process_retries() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    time_t now = time(nullptr);
    
    for (auto it = retry_queue.begin(); it != retry_queue.end();) {
        auto& entry = it->second;
        if (entry.attempt_count >= MAX_RETRY_COUNT) {
            bc_log(Error, "Failed to delete %s after %d attempts: %s",
                   entry.filepath.c_str(), entry.attempt_count, entry.error_reason.c_str());
            it = retry_queue.erase(it);
            continue;
        }
        
        if (now - entry.last_attempt >= RETRY_BACKOFF_BASE * std::pow(2, entry.attempt_count - 1)) {
            if (unlink(entry.filepath.c_str()) == 0) {
                it = retry_queue.erase(it);
            } else {
                entry.last_attempt = now;
                entry.attempt_count++;
                ++it;
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
    for (int i = 0; i < num_threads; ++i) {
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

cleanup_stats_report ParallelCleanup::get_stats() const {
    return stats.get_copy();
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
    queue_cv.wait(lock, [this] { return !work_queue.empty() || should_stop; });
    
    if (should_stop && work_queue.empty()) {
        return "";
    }
    
    std::string filepath = work_queue.front();
    work_queue.pop();
    return filepath;
}

void ParallelCleanup::process_file(const std::string& filepath) {
    struct stat st;
    bool file_exists = (stat(filepath.c_str(), &st) == 0);
    
    if (!file_exists) {
        // File doesn't exist, just mark it as archived in database
        char query[2048];
        snprintf(query, sizeof(query), 
                "UPDATE Media SET archive = 1 WHERE filepath = '%s'", 
                filepath.c_str());
        bc_log(Debug, "File doesn't exist, marking as archived: %s", query);
        
        if (bc_db_query("%s", query) == 0) {
            // Also mark associated events as archived
            char events_query[2048];
            snprintf(events_query, sizeof(events_query),
                    "UPDATE EventsCam SET archive = 1 WHERE media_id IN (SELECT id FROM Media WHERE filepath = '%s')",
                    filepath.c_str());
            bc_log(Debug, "Marking associated events as archived: %s", events_query);
            
            if (bc_db_query("%s", events_query) == 0) {
                stats.increment_deleted();
                // Don't log individual file deletions
            } else {
                bc_log(Error, "Failed to mark associated events as archived: %s", filepath.c_str());
                stats.increment_errors();
            }
        } else {
            bc_log(Error, "Failed to mark non-existent file as archived in database: %s", filepath.c_str());
            stats.increment_errors();
        }
        return;
    }
    
    stats.increment_processed();
    
    // Try to delete the file
    if (unlink(filepath.c_str()) == 0) {
        // Mark file as archived in database
        char query[2048];
        snprintf(query, sizeof(query), 
                "UPDATE Media SET archive = 1 WHERE filepath = '%s'", 
                filepath.c_str());
        bc_log(Debug, "Marking file as archived: %s", query);
        
        if (bc_db_query("%s", query) == 0) {
            // Also mark associated events as archived
            char events_query[2048];
            snprintf(events_query, sizeof(events_query),
                    "UPDATE EventsCam SET archive = 1 WHERE media_id IN (SELECT id FROM Media WHERE filepath = '%s')",
                    filepath.c_str());
            bc_log(Debug, "Marking associated events as archived: %s", events_query);
            
            if (bc_db_query("%s", events_query) == 0) {
                stats.increment_deleted();
                stats.add_bytes_freed(st.st_size);
                // Don't log individual file deletions
                
                // Try to remove empty parent directories
                std::string dir_path = bc_get_parent_path(filepath);
                while (!dir_path.empty() && bc_remove_dir_if_empty(dir_path)) {
                    dir_path = bc_get_parent_path(dir_path);
                }
            } else {
                bc_log(Error, "Failed to mark associated events as archived: %s", filepath.c_str());
                stats.increment_errors();
            }
        } else {
            bc_log(Error, "Failed to mark file as archived in database: %s", filepath.c_str());
            stats.increment_errors();
        }
    } else {
        bc_log(Error, "Failed to delete file %s: %s", filepath.c_str(), strerror(errno));
        stats.increment_errors();
    }
}

// CleanupScheduler implementation
CleanupScheduler::CleanupScheduler() 
    : last_run(std::chrono::system_clock::now())
    , interval(std::chrono::seconds(NORMAL_CLEANUP_INTERVAL))
    , high_priority_interval(std::chrono::seconds(HIGH_PRIORITY_INTERVAL))
    , startup_cleanup_done(false) {}

bool CleanupScheduler::needs_startup_cleanup() const {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    return !startup_cleanup_done;
}

void CleanupScheduler::mark_startup_cleanup_done() {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    startup_cleanup_done = true;
}

bool CleanupScheduler::should_run_cleanup() {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    
    // If startup cleanup hasn't been done, run it
    if (!startup_cleanup_done) {
        return true;
    }
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_run);
    
    // Check if any storage location needs cleanup based on its thresholds
    bool needs_cleanup = false;
    const char* storage_sql = "SELECT path, max_thresh FROM Storage ORDER BY priority";
    BC_DB_RES storage_res = bc_db_get_table("%s", storage_sql);
    if (storage_res) {
        while (!bc_db_fetch_row(storage_res)) {  // Note: returns 0 on success
            size_t len;
            const char* path = bc_db_get_val(storage_res, "path", &len);
            const char* max_thresh_str = bc_db_get_val(storage_res, "max_thresh", &len);
            
            if (path && max_thresh_str) {
                struct statvfs stat;
                if (statvfs(path, &stat) == 0) {
                    double total = static_cast<double>(stat.f_blocks) * stat.f_frsize;
                    double free = static_cast<double>(stat.f_bavail) * stat.f_frsize;
                    double used = total - free;
                    double usage = (used / total) * 100.0;
                    double max_thresh = atof(max_thresh_str);
                    
                    bc_log(Debug, "Storage path %s: usage=%.1f%%, max_thresh=%.1f%%", 
                           path, usage, max_thresh);
                    
                    // Use a small epsilon to handle floating point comparison
                    const double EPSILON = 0.0001;
                    if (usage > max_thresh + EPSILON) {  // Changed to > and added EPSILON to max_thresh
                        needs_cleanup = true;
                        bc_log(Info, "Storage path %s needs cleanup: %.1f%% > %.1f%%", path, usage, max_thresh);
                        // If we're above threshold, run cleanup immediately
                        bc_db_free_table(storage_res);
                        return true;
                    } else {
                        bc_log(Info, "Storage path %s below threshold: %.1f%% <= %.1f%%", path, usage, max_thresh);
                    }
                }
            }
        }
        bc_db_free_table(storage_res);
    }
    
    // If no storage is critically full, use normal interval
    return elapsed >= interval;
}

void CleanupScheduler::update_last_run() {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    last_run = std::chrono::system_clock::now();
}

void CleanupScheduler::set_interval(std::chrono::seconds new_interval) {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    interval = new_interval;
}

void CleanupScheduler::set_high_priority_interval(std::chrono::seconds new_interval) {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    high_priority_interval = new_interval;
}

// CleanupManager implementation
CleanupManager::CleanupManager() 
    : cleanup_in_progress(false)
    , retry_manager(std::make_unique<CleanupRetryManager>())
    , parallel_cleanup(std::make_unique<ParallelCleanup>())
    , scheduler(std::make_unique<CleanupScheduler>()) {}

bool CleanupManager::delete_media_file(const std::string& filepath, int id) {
    // Get the base path without extension
    std::string base_path = filepath;
    size_t last_dot = base_path.find_last_of('.');
    if (last_dot != std::string::npos) {
        base_path = base_path.substr(0, last_dot);
    }
    
    // Delete the main file
    bool main_file_deleted = (unlink(filepath.c_str()) == 0);
    
    // Delete sidecar files (jpg, jpeg, etc)
    std::vector<std::string> sidecar_extensions = {".jpg", ".jpeg", ".png", ".txt"};
    bool sidecar_deleted = true;
    
    for (const auto& ext : sidecar_extensions) {
        std::string sidecar_path = base_path + ext;
        if (unlink(sidecar_path.c_str()) == 0) {
            bc_log(Info, "Deleted sidecar file %s", sidecar_path.c_str());
        } else if (errno != ENOENT) {  // Ignore if file doesn't exist
            bc_log(Error, "Failed to delete sidecar file %s: %s", sidecar_path.c_str(), strerror(errno));
            sidecar_deleted = false;
        }
    }
    
    if (main_file_deleted) {
        // Mark file as archived in database
        char query[2048];
        snprintf(query, sizeof(query), 
                "UPDATE Media SET archive = 1 WHERE filepath = '%s'", 
                filepath.c_str());
        
        if (bc_db_query("%s", query) == 0) {
            // Also mark associated events as archived
            char events_query[2048];
            snprintf(events_query, sizeof(events_query),
                    "UPDATE EventsCam SET archive = 1 WHERE media_id IN (SELECT id FROM Media WHERE filepath = '%s')",
                    filepath.c_str());
            
            if (bc_db_query("%s", events_query) == 0) {
                return true;
            }
        }
    }
    return false;
}

bool CleanupManager::check_storage_status() {
    // Check if any storage location needs cleanup based on its thresholds
    const char* storage_sql = "SELECT path, max_thresh FROM Storage ORDER BY priority";
    BC_DB_RES storage_res = bc_db_get_table("%s", storage_sql);
    if (storage_res) {
        while (!bc_db_fetch_row(storage_res)) {
            size_t len;
            const char* path = bc_db_get_val(storage_res, "path", &len);
            const char* max_thresh_str = bc_db_get_val(storage_res, "max_thresh", &len);
            
            if (path && max_thresh_str) {
                struct statvfs stat;
                if (statvfs(path, &stat) == 0) {
                    double total = static_cast<double>(stat.f_blocks) * stat.f_frsize;
                    double free = static_cast<double>(stat.f_bavail) * stat.f_frsize;
                    double used = total - free;
                    double usage = (used / total) * 100.0;
                    double max_thresh = atof(max_thresh_str);
                    double target_usage = max_thresh - 1.0; // Target is 1% below max_thresh
                    
                    bc_log(Info, "Storage path %s: usage=%.1f%%, max_thresh=%.1f%%, target=%.1f%%", 
                           path, usage, max_thresh, target_usage);
                    
                    // Use a small epsilon to handle floating point comparison
                    const double EPSILON = 0.0001;
                    if (usage > target_usage + EPSILON) {
                        bc_db_free_table(storage_res);
                        return false;  // Still need cleanup
                    }
                }
            }
        }
        bc_db_free_table(storage_res);
    }
    return true;  // No cleanup needed
}

int CleanupManager::get_adaptive_batch_size() {
    // Start with default batch size
    int batch_size = DEFAULT_BATCH_SIZE;
    
    // Check system load and adjust
    if (LoadMonitor::is_system_overloaded()) {
        batch_size = std::max(MIN_BATCH_SIZE, batch_size / 2);
        bc_log(Info, "System overloaded, reducing batch size to %d", batch_size);
    }
    
    // Check MySQL load and adjust
    if (LoadMonitor::is_mysql_overloaded()) {
        batch_size = std::max(MIN_BATCH_SIZE, batch_size / 2);
        bc_log(Info, "MySQL overloaded, reducing batch size to %d", batch_size);
    }
    
    // Adjust based on storage size
    double storage_size_tb = bc_get_total_storage_size() / (1024.0 * 1024.0 * 1024.0 * 1024.0);
    int storage_based_size = static_cast<int>(storage_size_tb * BATCH_SIZE_MULTIPLIER);
    
    // Use the smaller of the two to be conservative
    batch_size = std::min(batch_size, storage_based_size);
    
    // Ensure within bounds
    batch_size = std::max(MIN_BATCH_SIZE, std::min(MAX_BATCH_SIZE, batch_size));
    
    return batch_size;
}

void CleanupManager::batch_delete_files(const std::vector<std::string>& files, int& deleted_count, size_t& bytes_freed) {
    deleted_count = 0;
    bytes_freed = 0;
    
    for (const auto& filepath : files) {
        struct stat st;
        if (stat(filepath.c_str(), &st) != 0) {
            bc_log(Debug, "File doesn't exist, skipping: %s", filepath.c_str());
            continue;
        }
        
        // Delete the file
        if (unlink(filepath.c_str()) == 0) {
            deleted_count++;
            bytes_freed += st.st_size;
            
            // Delete sidecar files
            std::string base_path = filepath;
            size_t last_dot = base_path.find_last_of('.');
            if (last_dot != std::string::npos) {
                base_path = base_path.substr(0, last_dot);
            }
            
            std::vector<std::string> sidecar_extensions = {".jpg", ".jpeg", ".png", ".txt"};
            for (const auto& ext : sidecar_extensions) {
                std::string sidecar_path = base_path + ext;
                unlink(sidecar_path.c_str()); // Ignore errors for sidecars
            }
        } else {
            bc_log(Error, "Failed to delete file %s: %s", filepath.c_str(), strerror(errno));
        }
    }
}

void CleanupManager::commit_batch_changes(const std::vector<std::string>& deleted_files) {
    if (deleted_files.empty()) {
        return;
    }
    
    // Start transaction
    if (bc_db_query("START TRANSACTION") != 0) {
        bc_log(Error, "Failed to start batch transaction");
        return;
    }
    
    try {
        // Batch update Media table
        std::string media_query = "UPDATE Media SET archive = 1 WHERE filepath IN (";
        for (size_t i = 0; i < deleted_files.size(); ++i) {
            if (i > 0) media_query += ",";
            media_query += "'" + deleted_files[i] + "'";
        }
        media_query += ")";
        
        if (bc_db_query("%s", media_query.c_str()) != 0) {
            bc_log(Error, "Failed to batch update Media table");
            bc_db_query("ROLLBACK");
            return;
        }
        
        // Batch update EventsCam table
        std::string events_query = "UPDATE EventsCam SET archive = 1 WHERE media_id IN (";
        events_query += "SELECT id FROM Media WHERE filepath IN (";
        for (size_t i = 0; i < deleted_files.size(); ++i) {
            if (i > 0) events_query += ",";
            events_query += "'" + deleted_files[i] + "'";
        }
        events_query += "))";
        
        if (bc_db_query("%s", events_query.c_str()) != 0) {
            bc_log(Error, "Failed to batch update EventsCam table");
            bc_db_query("ROLLBACK");
            return;
        }
        
        // Commit transaction
        if (bc_db_query("COMMIT") != 0) {
            bc_log(Error, "Failed to commit batch transaction");
            bc_db_query("ROLLBACK");
            return;
        }
        
        bc_log(Info, "Successfully committed batch changes for %zu files", deleted_files.size());
        
    } catch (...) {
        bc_log(Error, "Exception during batch commit, rolling back");
        bc_db_query("ROLLBACK");
    }
}

bool CleanupManager::process_batch(int batch_size, double target_threshold, int& total_deleted) {
    auto batch_start_time = std::chrono::steady_clock::now();
    
    // Check system load before processing
    LoadMonitor::wait_if_overloaded();
    
    // Get files for this batch
    BC_DB_RES dbres = bc_db_get_table("SELECT m.* FROM Media m "
                                    "WHERE m.archive=0 AND m.filepath!='' "
                                    "ORDER BY m.start ASC LIMIT %d", batch_size);
    
    if (!dbres) {
        bc_log(Error, "Database error during batch cleanup");
        return false;
    }
    
    std::vector<std::string> files_to_delete;
    std::vector<std::string> file_ids;
    
    // Collect files for batch processing
    while (bc_db_fetch_row(dbres) == 0) {
        const char *filepath = bc_db_get_val(dbres, "filepath", NULL);
        int id = bc_db_get_val_int(dbres, "id");
        
        if (!filepath || !*filepath) {
            continue;
        }
        
        files_to_delete.push_back(filepath);
        file_ids.push_back(std::to_string(id));
    }
    bc_db_free_table(dbres);
    
    if (files_to_delete.empty()) {
        bc_log(Info, "No files found for batch processing");
        return false;
    }
    
    bc_log(Info, "Processing batch of %zu files", files_to_delete.size());
    
    // Delete files from filesystem
    int deleted_count = 0;
    size_t bytes_freed = 0;
    batch_delete_files(files_to_delete, deleted_count, bytes_freed);
    
    if (deleted_count > 0) {
        // Commit database changes in a single transaction
        commit_batch_changes(files_to_delete);
        
        total_deleted += deleted_count;
        stats.add_bytes_freed(bytes_freed);
        
        // Update batch statistics
        auto batch_end_time = std::chrono::steady_clock::now();
        auto batch_duration = std::chrono::duration_cast<std::chrono::milliseconds>(batch_end_time - batch_start_time);
        stats.update_avg_batch_time(batch_duration.count() / 1000.0);
        
        bc_log(Info, "Batch completed: %d/%zu files deleted, %.2f MB freed, %.2f seconds", 
               deleted_count, files_to_delete.size(), 
               bytes_freed / (1024.0 * 1024.0), 
               batch_duration.count() / 1000.0);
    }
    
    stats.increment_batches();
    return deleted_count > 0;
}

int CleanupManager::run_cleanup() {
    std::lock_guard<std::mutex> lock(cleanup_mutex);
    
    if (cleanup_in_progress) {
        bc_log(Info, "Cleanup already in progress, skipping");
        return 0;
    }
    
    cleanup_in_progress = true;
    int total_deleted = 0;
    time_t start_time = time(nullptr);
    int batch_count = 0;
    
    try {
        // Get initial storage usage
        double initial_usage = bc_get_storage_usage();
        bc_log(Info, "Initial storage usage: %.1f%%", initial_usage);
        
        // Get target threshold from database
        double target_threshold = 0.0;
        const char* storage_sql = "SELECT max_thresh FROM Storage ORDER BY priority LIMIT 1";
        BC_DB_RES storage_res = bc_db_get_table("%s", storage_sql);
        if (storage_res && !bc_db_fetch_row(storage_res)) {
            size_t len;
            const char* max_thresh_str = bc_db_get_val(storage_res, "max_thresh", &len);
            if (max_thresh_str) {
                target_threshold = atof(max_thresh_str) - 1.0; // 1% below max_thresh
            }
        }
        bc_db_free_table(storage_res);
        
        bc_log(Info, "Target storage threshold: %.1f%%", target_threshold);
        
        while (should_continue_cleanup(start_time)) {
            // Check if we've reached the target
            double current_usage = bc_get_storage_usage();
            if (current_usage <= target_threshold) {
                bc_log(Info, "Storage usage now below target threshold (%.1f%% <= %.1f%%), stopping cleanup", 
                       current_usage, target_threshold);
                break;
            }
            
            // Get adaptive batch size
            int batch_size = get_adaptive_batch_size();
            
            // Process batch
            bool batch_success = process_batch(batch_size, target_threshold, total_deleted);
            batch_count++;
            
            if (!batch_success) {
                bc_log(Info, "No more files to delete, checking for database sync issues");
                
                // Check if storage is still full despite no files found
                if (current_usage > target_threshold + 5.0) { // 5% buffer
                    bc_log(Info, "Storage still full (%.1f%%) despite no files found, running database sync", current_usage);
                    int sync_result = sync_database_with_filesystem();
                    if (sync_result > 0) {
                        bc_log(Info, "Database sync cleaned up %d orphaned entries, re-running cleanup", sync_result);
                        // Reset for another cleanup attempt
                        total_deleted = 0;
                        batch_count = 0;
                        start_time = time(nullptr);
                        continue; // Go back to cleanup loop
                    }
                }
                break;
            }
            
            // Add delay between batches to reduce system load
            if (BATCH_DELAY_MS > 0) {
                usleep(BATCH_DELAY_MS * 1000);
            }
            
            // Sync filesystem periodically (not after every batch)
            if (batch_count % SYNC_INTERVAL_BATCHES == 0) {
                sync();
                bc_log(Debug, "Filesystem sync after %d batches", batch_count);
            }
            
            // Check system load periodically
            if (batch_count % LOAD_CHECK_INTERVAL == 0) {
                if (LoadMonitor::is_system_overloaded()) {
                    stats.increment_load_pauses();
                    bc_log(Info, "System load check: pausing due to high load");
                }
                if (LoadMonitor::is_mysql_overloaded()) {
                    stats.increment_mysql_pauses();
                    bc_log(Info, "MySQL load check: pausing due to high connections");
                }
            }
        }
        
        // Final sync
        sync();
        
        // Log summary
        if (total_deleted > 0) {
            double final_usage = bc_get_storage_usage();
            bc_log(Info, "Cleanup completed: %d files deleted, %d batches processed, %.1f%% -> %.1f%% usage", 
                   total_deleted, batch_count, initial_usage, final_usage);
        } else {
            bc_log(Info, "No files needed to be deleted");
        }
        
    } catch (const std::exception& e) {
        bc_log(Error, "Exception during cleanup: %s", e.what());
        cleanup_in_progress = false;
        return -1;
    }
    
    cleanup_in_progress = false;
    return 0;
}

cleanup_stats_report CleanupManager::get_stats_report() const {
    return stats.get_copy();
}

int CleanupManager::calculate_batch_size() {
    double storage_size_tb = bc_get_total_storage_size() / (1024.0 * 1024.0 * 1024.0 * 1024.0);
    int batch_size = static_cast<int>(storage_size_tb * BATCH_SIZE_MULTIPLIER);
    if (batch_size < MIN_BATCH_SIZE) return MIN_BATCH_SIZE;
    if (batch_size > MAX_BATCH_SIZE) return MAX_BATCH_SIZE;
    return batch_size;
}

bool CleanupManager::should_continue_cleanup(time_t start_time) {
    time_t now = time(nullptr);
    return (now - start_time) < MAX_CLEANUP_TIME;
}

void CleanupManager::update_stats(const cleanup_stats_report& new_stats) {
    std::lock_guard<std::mutex> lock(stats_mutex);
    stats.files_processed += new_stats.files_processed;
    stats.files_deleted += new_stats.files_deleted;
    stats.errors += new_stats.errors;
    stats.bytes_freed += new_stats.bytes_freed;
    stats.retries += new_stats.retries;
    stats.batches_processed += new_stats.batches_processed;
    stats.load_pauses += new_stats.load_pauses;
    stats.mysql_pauses += new_stats.mysql_pauses;
}

bool CleanupManager::should_run_cleanup() const {
    return scheduler->should_run_cleanup();
}

bool CleanupManager::needs_startup_cleanup() const {
    return scheduler->needs_startup_cleanup();
}

void CleanupManager::mark_startup_cleanup_done() {
    scheduler->mark_startup_cleanup_done();
}

// Utility functions
std::string bc_get_directory_path(const std::string& filePath) {
    size_t found = filePath.find_last_of("/\\");
    std::string dirPath = filePath.substr(0, found);
    if (dirPath.back() != '/') dirPath.append("/");
    return dirPath;
}

std::string bc_get_file_name(const std::string& filePath) {
    size_t found = filePath.find_last_of("/\\");
    return filePath.substr(found + 1);
}

bool bc_is_leap_year(int year) {
    return (year % 4 == 0 &&
        (year % 100 != 0 ||
        year % 400 == 0));
}

int bc_days_in_month(int year, int month) {
    static const int days_per_month[] = {
        31, 28, 31, 30,
        31, 30, 31, 31,
        30, 31, 30, 31
    };

    if (--month < 0 || (size_t)month > sizeof(days_per_month) - 1) {
        bc_log(Error, "Time check error, invalid month: %d", month);
        return 0;
    }

    if (month == 1 && bc_is_leap_year(year)) {
        return 29;
    }

    return days_per_month[month];
}

bool bc_compare_time(const bc_time& tm1, const bc_time& tm2) {
    if (tm1.year != tm2.year) {
        return tm1.year < tm2.year;
    }
    if (tm1.month != tm2.month) {
        return tm1.month < tm2.month;
    }
    if (tm1.day != tm2.day) {
        return tm1.day < tm2.day;
    }
    if (tm1.hour != tm2.hour) {
        return tm1.hour < tm2.hour;
    }
    if (tm1.min != tm2.min) {
        return tm1.min < tm2.min;
    }
    return tm1.sec < tm2.sec;
}

bool bc_is_base_path(std::string dir_path) {
    // Remove trailing slash if present
    if (!dir_path.empty() && dir_path.back() == '/') {
        dir_path.pop_back();
    }
    
    // Check if this is a base path (e.g., /var/lib/bluecherry/recordings)
    // Base paths typically have 4-5 components
    size_t slash_count = std::count(dir_path.begin(), dir_path.end(), '/');
    return slash_count <= 5;
}

bool bc_is_current_path(const std::string& path) {
    time_t now = time(nullptr);
    struct tm tm;
    
    if (localtime_r(&now, &tm) == nullptr) {
        bc_log(Error, "Failed to get current time");
        return false;
    }
    
    // Check if path contains current year/month/day
    char current_date[32];
    snprintf(current_date, sizeof(current_date), "%04d/%02d/%02d", 
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    
    return path.find(current_date) != std::string::npos;
}

std::string bc_get_parent_path(const std::string& path) {
    size_t last_slash = path.find_last_of('/');
    if (last_slash == std::string::npos) {
        return "";
    }
    return path.substr(0, last_slash);
}

std::string bc_get_dir_path(const std::string& path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) == 0) {
        if (S_ISDIR(path_stat.st_mode)) {
            return path;
        }
    }
    
    size_t last_slash = path.find_last_of('/');
    if (last_slash == std::string::npos) {
        return "";
    }
    return path.substr(0, last_slash);
}

bool bc_is_dir_empty(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return true; // Consider non-existent directories as empty
    }
    
    struct dirent *entry;
    bool empty = true;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            empty = false;
            break;
        }
    }
    
    closedir(dir);
    return empty;
}

bool bc_remove_dir_if_empty(const std::string& path) {
    if (bc_is_dir_empty(path)) {
        if (rmdir(path.c_str()) == 0) {
            bc_log(Debug, "Removed empty directory: %s", path.c_str());
            return true;
        } else {
            bc_log(Debug, "Failed to remove directory %s: %s", path.c_str(), strerror(errno));
        }
    }
    return false;
}

int bc_remove_directory(const std::string& path) {
    struct stat statbuf = {0};
    if (stat(path.c_str(), &statbuf) == -1) {
        return -1;
    }
    
    if (S_ISDIR(statbuf.st_mode)) {
        DIR* dir = opendir(path.c_str());
        if (dir) {
            struct dirent *entry;
            while ((entry = readdir(dir)) != nullptr) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                
                std::string full_path = path + "/" + std::string(entry->d_name);
                if (bc_remove_directory(full_path) != 0) {
                    closedir(dir);
                    return -1;
                }
            }
            closedir(dir);
        }
        
        if (rmdir(path.c_str()) == -1) {
            return -1;
        }
    } else {
        if (unlink(path.c_str()) == -1) {
            return -1;
        }
    }
    
    return 0;
}

void scan_directory_for_files(const std::string& basepath, std::vector<std::string>& files, int& batch_size, int& files_found) {
    DIR* dir = opendir(basepath.c_str());
    if (!dir) {
        bc_log(Error, "Failed to open directory %s: %s", basepath.c_str(), strerror(errno));
        return;
    }
    
    // Store files with their timestamps for sorting
    struct FileInfo {
        std::string path;
        time_t timestamp;
        
        bool operator<(const FileInfo& other) const {
            return timestamp < other.timestamp;  // Sort by oldest first
        }
    };
    std::vector<FileInfo> file_list;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        std::string full_path = basepath + "/" + entry->d_name;
        struct stat st;
        
        if (stat(full_path.c_str(), &st) != 0) {
            continue;
        }
        
        if (S_ISREG(st.st_mode)) {
            // Regular file
            FileInfo info;
            info.path = full_path;
            info.timestamp = st.st_mtime;
            file_list.push_back(info);
            files_found++;
        } else if (S_ISDIR(st.st_mode)) {
            // Directory - recursively scan
            int sub_batch_size = batch_size / 2;
            int sub_files_found = 0;
            scan_directory_for_files(full_path, files, sub_batch_size, sub_files_found);
            files_found += sub_files_found;
        }
    }
    
    closedir(dir);
    
    // Sort files by timestamp (oldest first)
    std::sort(file_list.begin(), file_list.end());
    
    // Add files to the result vector, respecting batch size
    for (const auto& info : file_list) {
        if (files.size() >= batch_size) {
            break;
        }
        files.push_back(info.path);
    }
}

std::vector<std::string> bc_get_media_files(int batch_size) {
    std::vector<std::string> files;
    
    struct StorageInfo {
        std::string path;
        double usage;
        double max_thresh;
        double min_thresh;
    };
    std::vector<StorageInfo> storage_info;
    
    // Get storage information from database
    const char* storage_sql = "SELECT path, max_thresh, min_thresh FROM Storage ORDER BY priority";
    BC_DB_RES storage_res = bc_db_get_table("%s", storage_sql);
    if (storage_res) {
        while (!bc_db_fetch_row(storage_res)) {
            size_t len;
            const char* path = bc_db_get_val(storage_res, "path", &len);
            const char* max_thresh_str = bc_db_get_val(storage_res, "max_thresh", &len);
            const char* min_thresh_str = bc_db_get_val(storage_res, "min_thresh", &len);
            
            if (path && max_thresh_str && min_thresh_str) {
                struct statvfs stat;
                if (statvfs(path, &stat) == 0) {
                    double total = static_cast<double>(stat.f_blocks) * stat.f_frsize;
                    double free = static_cast<double>(stat.f_bavail) * stat.f_frsize;
                    double used = total - free;
                    double usage = (used / total) * 100.0;
                    
                    StorageInfo info;
                    info.path = path;
                    info.usage = usage;
                    info.max_thresh = atof(max_thresh_str);
                    info.min_thresh = atof(min_thresh_str);
                    storage_info.push_back(info);
                }
            }
        }
        bc_db_free_table(storage_res);
    }
    
    if (storage_info.empty()) {
        bc_log(Error, "No storage information available");
        return files;
    }
    
    // Sort by usage (highest first)
    std::sort(storage_info.begin(), storage_info.end(), 
              [](const StorageInfo& a, const StorageInfo& b) { return a.usage > b.usage; });
    
    // Find paths that need cleanup
    std::vector<std::string> paths_to_clean;
    double max_usage = storage_info[0].usage;
    double target_usage = storage_info[0].max_thresh;
    
    for (const auto& info : storage_info) {
        // Calculate safety margin based on storage size
        double safety_margin = 1.0; // Default 1%
        struct statvfs stat;
        if (statvfs(info.path.c_str(), &stat) == 0) {
            double total_gb = (static_cast<double>(stat.f_blocks) * stat.f_frsize) / (1024.0 * 1024.0 * 1024.0);
            if (total_gb > 1000) { // Large storage (>1TB)
                safety_margin = 2.0;
            } else if (total_gb > 100) { // Medium storage (>100GB)
                safety_margin = 1.5;
            }
            target_usage = info.max_thresh - safety_margin;
            bc_log(Info, "Storage path %s: using %.1f%% safety margin (target %.1f%%)", 
                   info.path.c_str(), safety_margin, target_usage);
        }
        // Only add to cleanup if we're above max threshold
        if (info.usage > info.max_thresh) {
            paths_to_clean.push_back(info.path);
            bc_log(Info, "Storage path %s needs cleanup: %.1f%% > %.1f%%", 
                   info.path.c_str(), info.usage, info.max_thresh);
        } else {
            bc_log(Info, "Storage path %s below threshold: %.1f%% <= %.1f%%", 
                   info.path.c_str(), info.usage, info.max_thresh);
        }
    }
    
    // If we're not above max threshold, don't clean anything
    if (paths_to_clean.empty()) {
        bc_log(Info, "No storage paths above max threshold, skipping cleanup");
        return files;
    }
    
    // Calculate how many files we need to process to reach target usage
    double total_size = 0.0;
    for (const auto& info : storage_info) {
        if (std::find(paths_to_clean.begin(), paths_to_clean.end(), info.path) != paths_to_clean.end()) {
            struct statvfs stat;
            if (statvfs(info.path.c_str(), &stat) == 0) {
                total_size += static_cast<double>(stat.f_blocks) * stat.f_frsize;
            }
        }
    }
    
    double current_used = (max_usage / 100.0) * total_size;
    double target_used = (target_usage / 100.0) * total_size;
    double space_to_free = current_used - target_used;
    
    // If we're already below target usage, don't clean anything
    if (space_to_free <= 0) {
        bc_log(Info, "Already below target usage (%.1f%% < %.1f%%), skipping cleanup", max_usage, target_usage);
        return files;
    }
    
    // If we're critically full, increase the minimum space to free
    if (max_usage >= storage_info[0].max_thresh) {
        space_to_free = std::max(space_to_free, MIN_SPACE_TO_FREE_GB * 2 * 1024 * 1024 * 1024.0);
        bc_log(Info, "Storage critically full (%.1f%%), increasing minimum space to free to %.1f GB", 
               max_usage, MIN_SPACE_TO_FREE_GB * 2.0);
    } else {
        space_to_free = std::max(space_to_free, MIN_SPACE_TO_FREE_GB * 1024 * 1024 * 1024.0);
    }
    
    // Build SQL query to only get files from storage locations that need cleanup
    std::string sql = "SELECT filepath, size FROM Media WHERE (";
    for (size_t i = 0; i < paths_to_clean.size(); ++i) {
        if (i > 0) sql += " OR ";
        sql += "filepath LIKE '" + paths_to_clean[i] + "%'";
    }
    sql += ") ORDER BY start ASC LIMIT " + std::to_string(batch_size * 2);  // Double the batch size when critically full
    
    bc_log(Info, "Executing cleanup query: %s", sql.c_str());
    
    // First check how many files we have in total
    std::string count_sql = "SELECT COUNT(*) as count FROM Media";
    BC_DB_RES count_res = bc_db_get_table("%s", count_sql.c_str());
    if (count_res && !bc_db_fetch_row(count_res)) {
        int total_files = bc_db_get_val_int(count_res, "count");
        bc_log(Info, "Total undeleted files in database: %d", total_files);
    }
    bc_db_free_table(count_res);
    
    BC_DB_RES res = bc_db_get_table("%s", sql.c_str());
    if (res) {
        double freed_so_far = 0.0;
        int files_found = 0;
        while (!bc_db_fetch_row(res) && freed_so_far < space_to_free) {
            size_t len;
            const char* path = bc_db_get_val(res, "filepath", &len);
            const char* size_str = bc_db_get_val(res, "size", &len);
            
            if (path && size_str) {
                files.push_back(path);
                freed_so_far += atof(size_str);
                files_found++;
            }
        }
        bc_log(Info, "Found %d files matching cleanup criteria", files_found);
        bc_db_free_table(res);
    } else {
        bc_log(Error, "Failed to execute cleanup query");
    }
    
    bc_log(Info, "Selected %zu files for cleanup, targeting to free %.2f GB", 
           files.size(), space_to_free / (1024.0 * 1024.0 * 1024.0));
    
    return files;
}

double bc_get_storage_usage() {
    double max_usage = 0.0;
    std::vector<std::string> storage_paths;
    
    // Force filesystem sync before checking usage
    sync();
    
    // Get all storage paths from Storage table
    const char* sql = "SELECT path FROM Storage ORDER BY priority";
    bc_log(Debug, "Executing query: %s", sql);
    
    BC_DB_RES dbres = bc_db_get_table("%s", sql);
    if (!dbres) {
        bc_log(Error, "Failed to get storage paths from database");
        return 100.0;  // Return 100% if we can't check
    }
    
    int row_count = 0;
    while (!bc_db_fetch_row(dbres)) {  // Note: returns 0 on success
        size_t len;
        const char* path = bc_db_get_val(dbres, "path", &len);
        if (path) {
            bc_log(Debug, "Found storage path: %s", path);
            storage_paths.push_back(path);
            row_count++;
        }
    }
    
    bc_log(Debug, "Found %d storage paths in database", row_count);
    bc_db_free_table(dbres);
    
    if (storage_paths.empty()) {
        bc_log(Error, "No storage paths found in Storage table");
        return 100.0;
    }
    
    // Check usage for each storage path
    for (const auto& path : storage_paths) {
        // Force sync for this specific path
        int fd = open(path.c_str(), O_RDONLY);
        if (fd >= 0) {
            fsync(fd);
            close(fd);
        }
        
        // Wait a moment for filesystem to update stats
        usleep(100000);  // 100ms delay
        
        struct statvfs stat;
        if (statvfs(path.c_str(), &stat) != 0) {
            bc_log(Error, "Failed to get storage usage for %s: %s", path.c_str(), strerror(errno));
            continue;
        }
        
        double total = static_cast<double>(stat.f_blocks) * stat.f_frsize;
        double free = static_cast<double>(stat.f_bavail) * stat.f_frsize;
        double used = total - free;
        double usage = (used / total) * 100.0;
        
        bc_log(Debug, "Storage path %s: usage=%.1f%% (total=%.2f GB, free=%.2f GB, used=%.2f GB)", 
               path.c_str(), usage, 
               total/(1024*1024*1024.0), 
               free/(1024*1024*1024.0), 
               used/(1024*1024*1024.0));
        
        if (usage > max_usage) {
            max_usage = usage;
            bc_log(Debug, "New maximum usage found: %.1f%% at %s", max_usage, path.c_str());
        }
    }
    
    bc_log(Debug, "Final maximum storage usage: %.1f%%", max_usage);
    return max_usage;
}

double bc_get_total_storage_size() {
    double total_size = 0.0;
    std::vector<std::string> storage_paths;
    
    // Get all storage paths from Storage table
    const char* sql = "SELECT path FROM Storage ORDER BY priority";
    bc_log(Debug, "Executing query: %s", sql);
    
    BC_DB_RES dbres = bc_db_get_table("%s", sql);
    if (!dbres) {
        bc_log(Error, "Failed to get storage paths from database");
        return 0.0;
    }
    
    int row_count = 0;
    while (!bc_db_fetch_row(dbres)) {  // Note: returns 0 on success
        size_t len;
        const char* path = bc_db_get_val(dbres, "path", &len);
        if (path) {
            bc_log(Debug, "Found storage path: %s", path);
            storage_paths.push_back(path);
            row_count++;
        }
    }
    
    bc_log(Debug, "Found %d storage paths in database", row_count);
    bc_db_free_table(dbres);
    
    if (storage_paths.empty()) {
        bc_log(Error, "No storage paths found in Storage table");
        return 0.0;
    }
    
    // Calculate total size for all storage paths
    for (const auto& path : storage_paths) {
        struct statvfs stat;
        if (statvfs(path.c_str(), &stat) != 0) {
            bc_log(Error, "Failed to get storage size for %s: %s", path.c_str(), strerror(errno));
            continue;
        }
        
        double path_size = static_cast<double>(stat.f_blocks) * stat.f_frsize;
        total_size += path_size;
        
        bc_log(Debug, "Storage path %s: size=%.2f GB", 
               path.c_str(), path_size/(1024*1024*1024.0));
    }
    
    bc_log(Debug, "Total storage size: %.2f GB", total_size/(1024*1024*1024.0));
    return total_size;
}

static float path_used_percent(const char *path) {
    struct statvfs st;
    if (statvfs(path, &st) != 0) {
        return -1.0f;
    }
    
    double total = static_cast<double>(st.f_blocks) * st.f_frsize;
    double free = static_cast<double>(st.f_bavail) * st.f_frsize;
    double used = total - free;
    
    return static_cast<float>((used / total) * 100.0);
}

static int is_storage_full(const struct bc_storage *stor) {
    float used = path_used_percent(stor->path);
    return (used >= 0 && used >= stor->max_thresh);
}

// Database synchronization functions
int CleanupManager::sync_database_with_filesystem() {
    bc_log(Info, "Starting database/filesystem synchronization");
    
    // Get all non-archived media files from database
    BC_DB_RES dbres = bc_db_get_table("SELECT id, filepath FROM Media WHERE archive=0 AND filepath!=''");
    if (!dbres) {
        bc_log(Error, "Database error during sync: failed to get media files");
        return -1;
    }
    
    std::vector<std::string> orphaned_files;
    std::vector<int> orphaned_ids;
    int total_checked = 0;
    int orphaned_count = 0;
    
    // Check each file in database against filesystem
    while (bc_db_fetch_row(dbres) == 0) {
        const char *filepath = bc_db_get_val(dbres, "filepath", NULL);
        int id = bc_db_get_val_int(dbres, "id");
        
        if (!filepath || !*filepath) {
            continue;
        }
        
        total_checked++;
        
        // Check if file exists on filesystem
        struct stat st;
        if (stat(filepath, &st) != 0) {
            // File doesn't exist on filesystem but exists in database
            orphaned_files.push_back(filepath);
            orphaned_ids.push_back(id);
            orphaned_count++;
            
            if (orphaned_count <= 10) { // Log first 10 for debugging
                bc_log(Info, "Found orphaned database entry: ID=%d, filepath=%s", id, filepath);
            }
        }
    }
    bc_db_free_table(dbres);
    
    if (orphaned_count == 0) {
        bc_log(Info, "Database/filesystem sync complete: %d files checked, no orphaned entries found", total_checked);
        return 0;
    }
    
    bc_log(Info, "Found %d orphaned database entries out of %d total files", orphaned_count, total_checked);
    
    // Clean up orphaned database entries in batches
    const int BATCH_SIZE = 100;
    int cleaned_count = 0;
    
    for (size_t i = 0; i < orphaned_ids.size(); i += BATCH_SIZE) {
        size_t batch_end = std::min(i + BATCH_SIZE, orphaned_ids.size());
        
        // Start transaction
        if (bc_db_query("START TRANSACTION") != 0) {
            bc_log(Error, "Failed to start sync transaction");
            return -1;
        }
        
        try {
            // Build batch query for Media table
            std::string media_query = "UPDATE Media SET archive = 1 WHERE id IN (";
            for (size_t j = i; j < batch_end; ++j) {
                if (j > i) media_query += ",";
                media_query += std::to_string(orphaned_ids[j]);
            }
            media_query += ")";
            
            if (bc_db_query("%s", media_query.c_str()) != 0) {
                bc_log(Error, "Failed to update Media table for orphaned entries");
                bc_db_query("ROLLBACK");
                return -1;
            }
            
            // Build batch query for EventsCam table
            std::string events_query = "UPDATE EventsCam SET archive = 1 WHERE media_id IN (";
            for (size_t j = i; j < batch_end; ++j) {
                if (j > i) events_query += ",";
                events_query += std::to_string(orphaned_ids[j]);
            }
            events_query += ")";
            
            if (bc_db_query("%s", events_query.c_str()) != 0) {
                bc_log(Error, "Failed to update EventsCam table for orphaned entries");
                bc_db_query("ROLLBACK");
                return -1;
            }
            
            // Commit transaction
            if (bc_db_query("COMMIT") != 0) {
                bc_log(Error, "Failed to commit sync transaction");
                bc_db_query("ROLLBACK");
                return -1;
            }
            
            cleaned_count += (batch_end - i);
            bc_log(Info, "Cleaned up batch of %zu orphaned entries (total: %d)", batch_end - i, cleaned_count);
            
        } catch (...) {
            bc_log(Error, "Exception during sync batch cleanup, rolling back");
            bc_db_query("ROLLBACK");
            return -1;
        }
    }
    
    bc_log(Info, "Database/filesystem sync complete: %d orphaned entries cleaned up", cleaned_count);
    return cleaned_count;
}

int CleanupManager::run_database_sync() {
    std::lock_guard<std::mutex> lock(cleanup_mutex);
    
    if (cleanup_in_progress) {
        bc_log(Info, "Cleanup already in progress, skipping database sync");
        return 0;
    }
    
    cleanup_in_progress = true;
    
    try {
        int result = sync_database_with_filesystem();
        cleanup_in_progress = false;
        return result;
    } catch (const std::exception& e) {
        bc_log(Error, "Exception during database sync: %s", e.what());
        cleanup_in_progress = false;
        return -1;
    }
}