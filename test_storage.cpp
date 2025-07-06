#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/statvfs.h>
#include <mntent.h>

// Test our storage_path structure
struct storage_path {
    std::string path = std::string("");
    uint64_t total_size = 0;      // Total size in bytes
    uint64_t used_size = 0;       // Used size in bytes
    uint64_t free_size = 0;       // Free size in bytes
    uint32_t usage_percent = 0;   // Usage percentage (0-100)
    std::string filesystem = std::string(""); // Filesystem type
};

// Test our storage update function
bool update_storage_info(std::vector<storage_path>& storage_paths) {
    std::vector<storage_path> new_storage_paths;
    
    // Open /etc/mtab or /proc/mounts to get mounted filesystems
    FILE *mtab = setmntent("/proc/mounts", "r");
    if (!mtab) {
        return false;
    }
    
    struct mntent *entry;
    while ((entry = getmntent(mtab)) != NULL) {
        // Skip non-local filesystems and special filesystems
        if (strcmp(entry->mnt_type, "proc") == 0 || 
            strcmp(entry->mnt_type, "sysfs") == 0 ||
            strcmp(entry->mnt_type, "devpts") == 0 ||
            strcmp(entry->mnt_type, "tmpfs") == 0 ||
            strcmp(entry->mnt_type, "devtmpfs") == 0 ||
            strcmp(entry->mnt_type, "cgroup") == 0 ||
            strncmp(entry->mnt_type, "fuse.", 5) == 0) {
            continue;
        }
        
        struct statvfs vfs;
        if (statvfs(entry->mnt_dir, &vfs) == 0) {
            storage_path path_info;
            path_info.path = std::string(entry->mnt_dir);
            path_info.filesystem = std::string(entry->mnt_type);
            
            // Calculate sizes in bytes
            uint64_t block_size = vfs.f_frsize ? vfs.f_frsize : vfs.f_bsize;
            path_info.total_size = (uint64_t)vfs.f_blocks * block_size;
            path_info.free_size = (uint64_t)vfs.f_bavail * block_size;
            path_info.used_size = path_info.total_size - path_info.free_size;
            
            // Calculate usage percentage
            if (path_info.total_size > 0) {
                path_info.usage_percent = (uint32_t)((path_info.used_size * 100) / path_info.total_size);
            } else {
                path_info.usage_percent = 0;
            }
            
            new_storage_paths.push_back(path_info);
        }
    }
    
    endmntent(mtab);
    
    // Update the storage paths
    storage_paths = new_storage_paths;
    return true;
}

int main() {
    std::vector<storage_path> storage_paths;
    
    std::cout << "Testing storage monitoring implementation..." << std::endl;
    
    if (update_storage_info(storage_paths)) {
        std::cout << "Successfully retrieved storage information for " << storage_paths.size() << " mount points:" << std::endl;
        
        for (const auto& path : storage_paths) {
            std::cout << "  Path: " << path.path << std::endl;
            std::cout << "    Filesystem: " << path.filesystem << std::endl;
            std::cout << "    Total: " << (path.total_size / (1024*1024*1024)) << " GB" << std::endl;
            std::cout << "    Used: " << (path.used_size / (1024*1024*1024)) << " GB" << std::endl;
            std::cout << "    Free: " << (path.free_size / (1024*1024*1024)) << " GB" << std::endl;
            std::cout << "    Usage: " << path.usage_percent << "%" << std::endl;
            std::cout << std::endl;
        }
    } else {
        std::cout << "Failed to retrieve storage information" << std::endl;
        return 1;
    }
    
    std::cout << "Storage monitoring implementation test completed successfully!" << std::endl;
    return 0;
} 