#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

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
    if (stat(filepath.c_str(), &st) != 0) {
        stats.increment_errors();
        return;
    }
    
    stats.increment_processed();
    if (unlink(filepath.c_str()) == 0) {
        stats.increment_deleted();
        stats.add_bytes_freed(st.st_size);
    } else {
        stats.increment_errors();
    }
}

// CleanupScheduler implementation
CleanupScheduler::CleanupScheduler() 
    : last_run(std::chrono::system_clock::now())
    , interval(std::chrono::seconds(NORMAL_CLEANUP_INTERVAL))
    , high_priority_interval(std::chrono::seconds(HIGH_PRIORITY_INTERVAL)) {}

bool CleanupScheduler::should_run_cleanup() {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_run);
    
    // Check if storage is critically full
    double storage_usage = bc_get_storage_usage();
    auto current_interval = (storage_usage >= CRITICAL_STORAGE_THRESHOLD) ? 
                           high_priority_interval : interval;
    
    return elapsed >= current_interval;
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
    
    time_t start_time = time(nullptr);
    int total_processed = 0;
    
    // Calculate optimal number of threads based on system size
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads > 0) {
        num_threads = std::min(num_threads, 8); // Cap at 8 threads
    } else {
        num_threads = 4; // Default to 4 threads if hardware_concurrency fails
    }
    
    parallel_cleanup->start_cleanup(num_threads);
    
    while (should_continue_cleanup(start_time)) {
        std::vector<std::string> files = bc_get_media_files(calculate_batch_size());
        if (files.empty()) {
            break;
        }
        
        for (const auto& file : files) {
            parallel_cleanup->add_work(file);
        }
        
        total_processed += files.size();
    }
    
    parallel_cleanup->stop_cleanup();
    retry_manager->process_retries();
    
    // Update stats
    update_stats(parallel_cleanup->get_stats());
    scheduler->update_last_run();
    
    return total_processed;
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

std::string bc_get_directory_path(const std::string& filePath)
{
    size_t found = filePath.find_last_of("/\\");
    std::string dirPath = filePath.substr(0, found);
	if (dirPath.back() != '/') dirPath.append("/");
	return dirPath;
}

std::string bc_get_file_name(const std::string& filePath)
{
    size_t found = filePath.find_last_of("/\\");
    return filePath.substr(found + 1);
}

bool bc_is_leap_year(int year)
{
	return (year % 4 == 0 &&
		(year % 100 != 0 ||
		year % 400 == 0));
}

int bc_days_in_month(int year, int month)
{
	static const int days_per_month[] =
		{
			31, 28, 31, 30,
			31, 30, 31, 31,
			30, 31, 30, 31
		};

	if (--month < 0 || (size_t)month > sizeof(days_per_month) - 1)
	{
		bc_log(Error, "Time check error, invalid month: %d", month);
		return 0;
	}

	int days = days_per_month[month];
	if (month == 2 && bc_is_leap_year(year)) days = 29;

	return days;
}

bool bc_compare_time(const bc_time& tm1, const bc_time& tm2)
{
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

bool bc_is_base_path(std::string dir_path)
{
	if (!dir_path.length()) return false;
	char basepath[PATH_MAX];

	if (bc_get_media_loc(basepath, sizeof(basepath)) < 0)
	{
		bc_log(Error, "No free storage locations for new recordings!");
		return false;
	}

	std::string base_path = std::string(basepath);
	if (base_path.back() != '/') base_path.append("/");
	if (dir_path.back() != '/') dir_path.append("/");

	if (dir_path.length() <= base_path.length() ||
		dir_path == base_path) return true;

	return false;
}

bool bc_is_current_path(const std::string& path)
{
	if (path.length() <= 30) return true;
	std::string timed_path = path.substr(path.size() - 30, 26);
	bc_time rec_time;
	int id = 0;

	int count = sscanf(timed_path.c_str(), "%04d/%02d/%02d/%06d/%02d-%02d-%02d",
		(int*)&rec_time.year, (int*)&rec_time.month, (int*)&rec_time.day, (int*)&id,
		(int*)&rec_time.hour, (int*)&rec_time.min, (int*)&rec_time.sec);
	if (count != 7) return true;

	struct tm tm;
	time_t ts = time(NULL);
	localtime_r(&ts, &tm);

	/* Do not remove current recording directory */
	if (rec_time.year == (tm.tm_year + 1900) &&
		rec_time.month == (tm.tm_mon + 1) &&
		rec_time.day == tm.tm_mday) return true;

	return false;
}

std::string bc_get_parent_path(const std::string& path)
{
	if (!path.length()) return std::string();
	std::string dir_path = path;

	std::size_t last_slash = dir_path.find_last_of('/');
	if (last_slash == std::string::npos) return std::string();

	dir_path.erase(last_slash);
	return dir_path;
}

bool bc_is_dir_empty(const std::string& path)
{
	int file_count = 0;
	struct dirent *entry;

	bc_log(Debug, "Cheking if empty: %s", path.c_str());
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

std::string bc_get_dir_path(const std::string& path)
{
	/* Do not empty or remove base paths */
	if (bc_is_base_path(path)) return std::string();
	else if (!path.length()) return std::string();

	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) != 0)
	{
		if (errno == ENOENT)
		{
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

	if (S_ISREG(path_stat.st_mode))
	{
		std::string parent = bc_get_parent_path(path);
		return bc_get_dir_path(parent);
	}

	bc_log(Error, "The path is neither a file nor a directory: %s", path.c_str());
	return std::string();
}

bool bc_remove_dir_if_empty(const std::string& path)
{
	// Do not remove current working dir, even if its empty
	if (bc_is_current_path(path)) return false;

	// Get parent directory path of recording file
	std::string dir = bc_get_dir_path(path);
	if (!dir.length()) return false;

	while (dir.length())
	{
		if (bc_is_base_path(dir)) break;
		if (!bc_is_dir_empty(dir)) break;

		if (rmdir(dir.c_str()) != 0)
		{
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

/*
 * bc_remove_directory - Recursively remove directory. Argument
 * path is the directory path. On success, zero is returned.
 * On error, -1 is returned and errno is set appropriately.
 */
int bc_remove_directory(const std::string& path)
{
	/* Check if path exists */
	struct stat statbuf = {0};
	int retval = stat(path.c_str(), &statbuf);
	if (retval < 0) return -1;

	/* Open directory */
	DIR *dir = opendir(path.c_str());
	if (dir)
	{
		struct dirent *entry;
		retval = 0;

		while (!retval && (entry = readdir(dir)))
		{
			int xretval = -1;

			/* Skip the names "." and ".." as we don't want to recurse on them. */
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
			std::string new_path = path + "/" + std::string(entry->d_name);

			if (!stat(new_path.c_str(), &statbuf))
			{
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

// Implementation of storage-related functions
double bc_get_storage_usage() {
    struct statvfs stat;
    char basepath[PATH_MAX];
    
    if (bc_get_media_loc(basepath, sizeof(basepath)) < 0) {
        bc_log(Error, "No free storage locations for new recordings!");
        return 100.0; // Return 100% usage if we can't get storage info
    }
    
    if (statvfs(basepath, &stat) != 0) {
        bc_log(Error, "Failed to get storage usage for %s: %s", basepath, strerror(errno));
        return 100.0;
    }
    
    double total = static_cast<double>(stat.f_blocks) * stat.f_frsize;
    double free = static_cast<double>(stat.f_bfree) * stat.f_frsize;
    double used = total - free;
    
    return (used / total) * 100.0;
}

double bc_get_total_storage_size() {
    struct statvfs stat;
    char basepath[PATH_MAX];
    
    if (bc_get_media_loc(basepath, sizeof(basepath)) < 0) {
        bc_log(Error, "No free storage locations for new recordings!");
        return 0.0;
    }
    
    if (statvfs(basepath, &stat) != 0) {
        bc_log(Error, "Failed to get storage size for %s: %s", basepath, strerror(errno));
        return 0.0;
    }
    
    return static_cast<double>(stat.f_blocks) * stat.f_frsize;
}

std::vector<std::string> bc_get_media_files(int batch_size) {
    std::vector<std::string> files;
    char basepath[PATH_MAX];
    
    if (bc_get_media_loc(basepath, sizeof(basepath)) < 0) {
        bc_log(Error, "No free storage locations for new recordings!");
        return files;
    }
    
    DIR* dir = opendir(basepath);
    if (!dir) {
        bc_log(Error, "Failed to open directory %s: %s", basepath, strerror(errno));
        return files;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr && files.size() < static_cast<size_t>(batch_size)) {
        if (entry->d_type == DT_REG) {  // Only regular files
            std::string filepath = std::string(basepath) + "/" + entry->d_name;
            files.push_back(filepath);
        }
    }
    
    closedir(dir);
    return files;
}