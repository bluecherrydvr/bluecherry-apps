// Utility struct and functions from bc-cleaner.h
struct bc_time {
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
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