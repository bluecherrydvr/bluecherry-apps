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
CleanupManager::CleanupManager() : cleanup_in_progress(false) {
    retry_manager = std::make_unique<CleanupRetryManager>();
    parallel_cleanup = std::make_unique<ParallelCleanup>();
    scheduler = std::make_unique<CleanupScheduler>();
    last_cleanup_time = std::chrono::system_clock::now();
}

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
                // Force filesystem sync after all deletions
                sync();
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

int CleanupManager::run_cleanup() {
    std::lock_guard<std::mutex> lock(cleanup_mutex);
    
    if (cleanup_in_progress) {
        bc_log(Info, "Cleanup already in progress, skipping");
        return 0;
    }
    
    cleanup_in_progress = true;
    int deleted_count = 0;
    std::string last_deleted_path;
    time_t start_time = time(nullptr);
    
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
            // Get oldest media files that need cleanup - ORDER BY start_time to ensure we get oldest first
            BC_DB_RES dbres = bc_db_get_table("SELECT m.* FROM Media m "
                                            "WHERE m.archive=0 AND m.filepath!='' "
                                            "ORDER BY m.start_time ASC LIMIT 1000");
            
            if (!dbres) {
                bc_log(Error, "Database error during media cleanup");
                cleanup_in_progress = false;
                return -1;
            }
            
            bool deleted_any = false;
            int rows_processed = 0;
            
            // Process all rows in the result set
            while (bc_db_fetch_row(dbres) == 0) {  // Changed condition to check for 0 (success)
                const char *filepath = bc_db_get_val(dbres, "filepath", NULL);
                int id = bc_db_get_val_int(dbres, "id");
                const char *start_time = bc_db_get_val(dbres, "start_time", NULL);
                
                if (!filepath || !*filepath) {
                    bc_log(Debug, "Skipping media entry %d with empty filepath", id);
                    continue;
                }
                
                rows_processed++;
                bc_log(Info, "Processing file %s (start_time: %s) [%d/%d]", 
                       filepath, start_time ? start_time : "unknown", rows_processed, 1000);
                
                // Get file size before deletion
                struct stat st;
                if (stat(filepath, &st) != 0) {
                    bc_log(Error, "Failed to stat file %s: %s", filepath, strerror(errno));
                    continue;
                }
                
                // Delete the file and its sidecar
                if (delete_media_file(filepath, id)) {
                    deleted_count++;
                    last_deleted_path = filepath;
                    deleted_any = true;
                    
                    // Force filesystem sync after deletion
                    sync();
                    
                    // Check current storage usage
                    double current_usage = bc_get_storage_usage();
                    bc_log(Info, "Storage usage after deleting %s: %.1f%% (target: %.1f%%)", 
                           filepath, current_usage, target_threshold);
                    
                    // Check if we've freed enough space
                    if (current_usage <= target_threshold) {
                        bc_log(Info, "Storage usage now below target threshold (%.1f%% <= %.1f%%), stopping cleanup", 
                               current_usage, target_threshold);
                        bc_db_free_table(dbres);
                        cleanup_in_progress = false;
                        return 0;
                    }
                }
            }
            
            bc_db_free_table(dbres);
            
            // If we didn't delete any files in this batch, we're done
            if (!deleted_any) {
                bc_log(Info, "No more files to delete in this batch");
                break;
            }
            
            // Force filesystem sync after batch
            sync();
        }
        
        // Log summary of deletions
        if (deleted_count > 0) {
            double final_usage = bc_get_storage_usage();
            if (deleted_count == 1) {
                bc_log(Info, "Successfully deleted and archived file and associated events: %s (Usage: %.1f%% -> %.1f%%)", 
                       last_deleted_path.c_str(), initial_usage, final_usage);
            } else {
                bc_log(Info, "Successfully deleted and archived %d files and associated events (last: %s) (Usage: %.1f%% -> %.1f%%)", 
                       deleted_count, last_deleted_path.c_str(), initial_usage, final_usage);
            }
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

    int days = days_per_month[month];
    if (month == 2 && bc_is_leap_year(year)) days = 29;

    return days;
}

bool bc_compare_time(const bc_time& tm1, const bc_time& tm2) {
    long long time1 = 0, time2 = 0;
    int days1 = 0, days2 = 0;

    // Convert years and months to days
    for (int y = 1; y < tm1.year; y++)
        days1 += bc_is_leap_year(y) ? 366 : 365;

    for (int m = 1; m < tm1.month; m++)
        days1 += bc_days_in_month(tm1.year, m);

    for (int y = 1; y < tm2.year; y++)
        days2 += bc_is_leap_year(y) ? 366 : 365;

    for (int m = 1; m < tm2.month; m++)
        days2 += bc_days_in_month(tm2.year, m);

    // Add days
    days1 += tm1.day;
    days2 += tm2.day;

    // Convert days to seconds
    time1 = (long long)days1 * 24 * 60 * 60;
    time2 = (long long)days2 * 24 * 60 * 60;

    // Convert hours to seconds
    time1 += (long long)tm1.hour * 60 * 60;
    time2 += (long long)tm2.hour * 60 * 60;

    // Convert minutes to seconds
    time1 += (long long)tm1.min * 60;
    time2 += (long long)tm2.min * 60;

    // Add seconds
    time1 += tm1.sec;
    time2 += tm2.sec;

    return time1 < time2;
}

bool bc_is_base_path(std::string dir_path) {
    if (!dir_path.length()) return false;
    char basepath[PATH_MAX];

    if (bc_get_media_loc(basepath, sizeof(basepath)) < 0) {
        bc_log(Error, "No free storage locations for new recordings!");
        return false;
    }

    std::string base_path = std::string(basepath);
    if (base_path.back() != '/') base_path.append("/");
    if (dir_path.back() != '/') dir_path.append("/");

    return dir_path == base_path;
}

bool bc_is_current_path(const std::string& path) {
    // Only check paths that look like they might be date-based
    if (path.length() <= 30) return false;
    
    std::string timed_path = path.substr(path.size() - 30, 26);
    bc_time rec_time;
    int id = 0;

    int count = sscanf(timed_path.c_str(), "%04d/%02d/%02d/%06d/%02d-%02d-%02d",
        (int*)&rec_time.year, (int*)&rec_time.month, (int*)&rec_time.day, (int*)&id,
        (int*)&rec_time.hour, (int*)&rec_time.min, (int*)&rec_time.sec);
    
    // If we can't parse the date, don't skip the path
    if (count != 7) return false;

    struct tm tm;
    time_t ts = time(NULL);
    localtime_r(&ts, &tm);

    // Only skip if it's from today
    return (rec_time.year == (tm.tm_year + 1900) &&
            rec_time.month == (tm.tm_mon + 1) &&
            rec_time.day == tm.tm_mday);
}

std::string bc_get_parent_path(const std::string& path) {
    if (!path.length()) return std::string();
    std::string dir_path = path;

    std::size_t last_slash = dir_path.find_last_of('/');
    if (last_slash == std::string::npos) return std::string();

    dir_path.erase(last_slash);
    return dir_path;
}

bool bc_is_dir_empty(const std::string& path) {
    int file_count = 0;
    struct dirent *entry;

    bc_log(Debug, "Checking if empty: %s", path.c_str());
    DIR *dir = opendir(path.c_str());

    if (dir == NULL) {
        bc_log(Warning, "Can not open directory %s: %s",
            path.c_str(), strerror(errno));

        return false;
    }

    // Check if there is something more than "." and ".."
    while ((entry = readdir(dir)) != NULL)
        if (++file_count > 2) break;

    closedir(dir);

    return file_count <= 2 ?
        true : false;
}

std::string bc_get_dir_path(const std::string& path) {
    /* Do not empty or remove base paths */
    if (bc_is_base_path(path)) return std::string();
    else if (!path.length()) return std::string();

    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) {
        if (errno == ENOENT) {
            /* If file does not exist, try parent directory */
            std::string parent = bc_get_parent_path(path);
            return bc_get_dir_path(parent);
        }

        bc_log(Error, "Failed to stat: %s (%s)",
            path.c_str(), strerror(errno));

        return std::string();
    }

    if (S_ISDIR(path_stat.st_mode))
        return std::string(path);

    if (S_ISREG(path_stat.st_mode)) {
        std::string parent = bc_get_parent_path(path);
        return bc_get_dir_path(parent);
    }

    bc_log(Error, "The path is neither a file nor a directory: %s", path.c_str());
    return std::string();
}

bool bc_remove_dir_if_empty(const std::string& path) {
    // Do not remove current working dir, even if its empty
    if (bc_is_current_path(path)) return false;

    // Get parent directory path of recording file
    std::string dir = bc_get_dir_path(path);
    if (!dir.length()) return false;

    while (dir.length()) {
        if (bc_is_base_path(dir)) break;
        if (!bc_is_dir_empty(dir)) break;

        if (rmdir(dir.c_str()) != 0) {
            bc_log(Error, "Failed to delete directory: %s (%s)",
                dir.c_str(), strerror(errno));

            return false;
        }

        bc_log(Info, "Deleted empty directory: %s", dir.c_str());
        std::string parent = bc_get_parent_path(dir);
        dir = bc_get_dir_path(parent);
    }

    return true;
}

int bc_remove_directory(const std::string& path) {
    /* Check if path exists */
    struct stat statbuf = {0};
    int retval = stat(path.c_str(), &statbuf);
    if (retval < 0) return -1;

    /* Open directory */
    DIR *dir = opendir(path.c_str());
    if (dir) {
        struct dirent *entry;
        retval = 0;

        while (!retval && (entry = readdir(dir))) {
            int xretval = -1;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
            std::string new_path = path + "/" + std::string(entry->d_name);

            if (!stat(new_path.c_str(), &statbuf)) {
                if (!S_ISDIR(statbuf.st_mode)) xretval = unlink(new_path.c_str());
                else xretval = bc_remove_directory(new_path.c_str());
            }

            retval = xretval;
        }

        /* Close dir pointer */
        closedir(dir);
    }

    if (!retval)
        retval = rmdir(path.c_str());

    return retval;
}

// Helper function to recursively scan directories
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
        
        if (S_ISREG(st.st_mode)) {  // Regular file
            // Extract timestamp from filename (format: HH-MM-SS)
            int hour, min, sec;
            if (sscanf(entry->d_name, "%02d-%02d-%02d", &hour, &min, &sec) == 3) {
                // Get the directory path components (YYYY/MM/DD)
                std::string dir_path = bc_get_parent_path(full_path);
                std::string dir_name = bc_get_file_name(dir_path);
                
                int year, month, day;
                if (sscanf(dir_name.c_str(), "%04d/%02d/%02d", &year, &month, &day) == 3) {
                    struct tm tm = {0};
                    tm.tm_year = year - 1900;  // Years since 1900
                    tm.tm_mon = month - 1;     // Months since January (0-11)
                    tm.tm_mday = day;
                    tm.tm_hour = hour;
                    tm.tm_min = min;
                    tm.tm_sec = sec;
                    
                    time_t timestamp = mktime(&tm);
                    if (timestamp != -1) {
                        files_found++;
                        FileInfo info;
                        info.path = full_path;
                        info.timestamp = timestamp;
                        file_list.push_back(info);
                    }
                }
            }
        } else if (S_ISDIR(st.st_mode)) {  // Directory
            // Skip current day's directory
            if (bc_is_current_path(full_path)) {
                continue;
            }
            scan_directory_for_files(full_path, files, batch_size, files_found);
        }
    }
    
    closedir(dir);
    
    // Sort files by timestamp (oldest first)
    std::sort(file_list.begin(), file_list.end());
    
    // Add sorted files to the cleanup batch
    for (const auto& info : file_list) {
        if (files.size() < static_cast<size_t>(batch_size)) {
            files.push_back(info.path);
        } else {
            break;
        }
    }
}

std::vector<std::string> bc_get_media_files(int batch_size) {
    std::vector<std::string> files;
    
    // Define storage info structure
    struct StorageInfo {
        std::string path;
        double usage;
        double max_thresh;
        double min_thresh;
    };
    std::vector<StorageInfo> storage_info;
    
    // First, get all storage locations and their usage
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
                    double max_thresh = atof(max_thresh_str);
                    double min_thresh = atof(min_thresh_str);
                    
                    StorageInfo info;
                    info.path = path;
                    info.usage = usage;
                    info.max_thresh = max_thresh;
                    info.min_thresh = min_thresh;
                    storage_info.push_back(info);
                    
                    bc_log(Info, "Storage path %s: usage=%.1f%%, max_thresh=%.1f%%, min_thresh=%.1f%%", 
                           path, usage, max_thresh, min_thresh);
                }
            }
        }
        bc_db_free_table(storage_res);
    }
    
    // Calculate how much space we need to free
    double max_usage = 0.0;
    double target_usage = 0.0;
    std::vector<std::string> paths_to_clean;
    for (const auto& info : storage_info) {
        if (info.usage > max_usage) {
            max_usage = info.usage;
            // Calculate safety margin based on storage size
            // For large arrays (>1TB), use 0.5% margin
            // For smaller arrays, use up to 1% margin
            double safety_margin = 0.5;  // Default to 0.5%
            struct statvfs stat;
            if (statvfs(info.path.c_str(), &stat) == 0) {
                double total_gb = (static_cast<double>(stat.f_blocks) * stat.f_frsize) / (1024.0 * 1024.0 * 1024.0);
                if (total_gb < 1000) {  // If less than 1TB
                    safety_margin = 1.0;  // Use 1% margin
                }
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
    
    // Sum up size for each storage path
    for (const auto& path : storage_paths) {
        struct statvfs stat;
        if (statvfs(path.c_str(), &stat) != 0) {
            bc_log(Error, "Failed to get storage size for %s: %s", path.c_str(), strerror(errno));
            continue;
        }
        
        double path_size = static_cast<double>(stat.f_blocks) * stat.f_frsize;
        total_size += path_size;
        
        bc_log(Debug, "Storage path %s: size=%.2f GB", path.c_str(), path_size / (1024.0 * 1024.0 * 1024.0));
    }
    
    bc_log(Debug, "Total storage size: %.2f GB", total_size / (1024.0 * 1024.0 * 1024.0));
    return total_size;
}

static float path_used_percent(const char *path) {
    struct statvfs st;

    if (statvfs(path, &st))
        return -1.00;

    // Calculate usage based on f_bavail (available blocks) instead of f_bfree
    // This matches the system's df command behavior
    double total = static_cast<double>(st.f_blocks) * st.f_frsize;
    double free = static_cast<double>(st.f_bavail) * st.f_frsize;
    double used = total - free;
    double usage = (used / total) * 100.0;

    bc_log(Debug, "path_used_percent(%s) = %.1f%%, total=%.2f GB, free=%.2f GB, used=%.2f GB", 
           path, usage, total/(1024*1024*1024.0), free/(1024*1024*1024.0), used/(1024*1024*1024.0));

    return usage;
}

static int is_storage_full(const struct bc_storage *stor) {
    float used = path_used_percent(stor->path);

    if (used < 0)
        return -1;

    // Add a small buffer to prevent oscillation
    const float BUFFER = 0.5; // 0.5% buffer
    return (used >= (stor->max_thresh - BUFFER));
}