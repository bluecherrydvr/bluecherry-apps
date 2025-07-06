#pragma once

#include "bc-time.h"

// Utility functions from bc-cleaner.h
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