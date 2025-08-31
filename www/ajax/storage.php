<?php

class storage extends Controller {

    public function __construct(){
        parent::__construct();
        $this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.storage');

        $locations = data::query("SELECT * FROM Storage");
        foreach(array_keys($locations) as $id) {
            $path = database::escapeString($locations[$id]['path']);
            $max_thresh = database::escapeString($locations[$id]['max_thresh']);
            
            // Disk space error checks
            $free_space = disk_free_space($path);
            $total_space = disk_total_space($path);
            if ($free_space === false || $total_space === false) {
                $locations[$id]['record_time'] = 0;
                $locations[$id]['used_percent'] = 0;
                // Set other stats to 0 for consistency
                $locations[$id]['total_duration'] = 0;
                $locations[$id]['total_size_mb'] = 0;
                $locations[$id]['file_count'] = 0;
                $locations[$id]['mb_per_hour'] = 0;
                $locations[$id]['has_sufficient_data'] = false;
                $locations[$id]['earliest_recording'] = 0;
                $locations[$id]['latest_recording'] = 0;
                continue;
            }

            // Get available space considering threshold
            $available_space = ($free_space - $total_space * (1.0 - $max_thresh/100) ) >> 20;
            if ($available_space < 0)
                $available_space = 0;

            // Get actual recording stats
            $query = "SELECT 
                    SUM(CASE WHEN end < start OR end = 0 THEN 0 ELSE end - start END) as total_duration,
                    COALESCE(SUM(size), 0) DIV 1048576 as total_size_mb,
                    COUNT(*) as file_count,
                    MIN(CASE WHEN start > 0 THEN start ELSE NULL END) as earliest_recording,
                    MAX(CASE WHEN end > 0 THEN end ELSE NULL END) as latest_recording
                    FROM Media 
                    WHERE filepath LIKE '{$path}%'
                    AND size > 2000";
            $stats = data::query($query);
            
            $total_hours = ($stats[0]['total_duration'] ?: 0) / 3600;
            $has_sufficient_data = ($total_hours >= 24);
            
            if ($has_sufficient_data && $stats[0]['total_size_mb'] > 0) {
                // Calculate average MB per hour based on actual recordings
                $mb_per_hour = $stats[0]['total_size_mb'] / $total_hours;
                
                // Estimate recording time based on available space and actual usage patterns
                $estimated_seconds = ($available_space * 3600) / $mb_per_hour;
            } else {
                // If insufficient data, use the original calculation
            $query = "SELECT ({$available_space}) * period / size AS result FROM (".
                      "SELECT SUM(CASE WHEN end < start OR end = 0 THEN 0 ELSE end - start END".
                    ") period, COALESCE(SUM(size), 0) DIV 1048576 size".
                " FROM Media WHERE filepath LIKE '{$path}%') q";
            $result = data::query($query);
                $estimated_seconds = $result[0]['result'] ?: 0;
            }
            
            $locations[$id]['record_time'] = $estimated_seconds;
            // Calculate used_percent with division by zero protection
            if ($total_space > 0) {
                $locations[$id]['used_percent'] = ceil(($total_space - $free_space) / $total_space * 100);
            } else {
                $locations[$id]['used_percent'] = 0;
            }
            
            // Add usage statistics
            $locations[$id]['total_duration'] = $stats[0]['total_duration'] ?: 0;
            $locations[$id]['total_size_mb'] = $stats[0]['total_size_mb'] ?: 0;
            $locations[$id]['file_count'] = $stats[0]['file_count'] ?: 0;
            $locations[$id]['mb_per_hour'] = isset($mb_per_hour) ? round($mb_per_hour, 2) : 0;
            $locations[$id]['has_sufficient_data'] = $has_sufficient_data;
            $locations[$id]['earliest_recording'] = $stats[0]['earliest_recording'] ?: 0;
            $locations[$id]['latest_recording'] = $stats[0]['latest_recording'] ?: 0;
        }

        $this->view->locations = $locations;
    }

    private function validatePath($path) {
        // Check for invalid characters
        if (preg_match('/[\|\&\;\$\#\*\(\)\{\}\[\]\<\>]/', $path)) {
            return array(false, "Storage path contains invalid characters");
        }

        // Check if path exists and is writable
        if (!is_dir($path)) {
            return array(false, "Storage path does not exist");
        }

        if (!is_writable($path)) {
            return array(false, "Storage path is not writable");
        }

        return array(true, "");
    }

    private function validateThresholds($max, $min) {
        if (!is_numeric($max) || !is_numeric($min)) {
            return array(false, "Thresholds must be numeric values");
        }

        $max = floatval($max);
        $min = floatval($min);

        if ($max < 1 || $max > 100) {
            return array(false, "Maximum threshold must be between 1 and 100");
        }

        if ($min < 1 || $min > 100) {
            return array(false, "Minimum threshold must be between 1 and 100");
        }

        if ($min >= $max) {
            return array(false, "Minimum threshold must be less than maximum threshold");
        }

        return array(true, "");
    }

    public function postData()
    {
        // Check if this is a database sync request
        if (isset($_POST['action']) && $_POST['action'] === 'sync_database') {
            $this->syncDatabase();
            return;
        }
        
        $storage_check = $this->ctl('storagecheck');
        $values = array();
        $errors = array();

        // Start transaction
        data::query("START TRANSACTION", true);

        try {
            // Validate all paths and thresholds first
            foreach($_POST['path'] as $key => $path) {
                // Validate path
                list($valid, $error) = $this->validatePath($path);
                if (!$valid) {
                    throw new Exception($error);
                }

                // Validate thresholds
                $max = intval($_POST['max'][$key]);
                $min = $max - 10;
                list($valid, $error) = $this->validateThresholds($max, $min);
                if (!$valid) {
                    throw new Exception($error);
                }

                // Check directory permissions
            $dir_status = $storage_check->change_directory_permissions($path);
            if ($dir_status[0] !== 'OK') {
                    throw new Exception($dir_status[1]);
            }

                $values[] = "({$key}, '" . database::escapeString($path) . "', {$max}, {$min})";
        }

            // Ensure at least one storage location
            if (empty($values)) {
                throw new Exception("At least one storage location is required");
            }

            // Delete existing storage locations
        data::query("DELETE FROM Storage", true);

            // Insert new storage locations
            $values = implode(',', $values);
            if (!data::query("INSERT INTO Storage (priority, path, max_thresh, min_thresh) VALUES {$values}", true)) {
                throw new Exception("Failed to save storage locations");
        }

            // Commit transaction
            data::query("COMMIT", true);
            data::responseJSON(true);

        } catch (Exception $e) {
            // Rollback transaction
            data::query("ROLLBACK", true);
            data::responseJSON(false, $e->getMessage());
        }
    }
    
    private function syncDatabase()
    {
        try {
            // Get all non-archived media files from database
            $query = "SELECT id, filepath FROM Media WHERE archive=0 AND filepath!=''";
            $media_files = data::query($query);
            
            if (!$media_files) {
                data::responseJSON(true, "No media files found in database");
                return;
            }
            
            $orphaned_count = 0;
            $total_checked = count($media_files);
            
            // Check each file against filesystem
            foreach ($media_files as $file) {
                $filepath = $file['filepath'];
                $id = $file['id'];
                
                if (empty($filepath)) {
                    continue;
                }
                
                // Check if file exists on filesystem
                if (!file_exists($filepath)) {
                    $orphaned_count++;
                }
            }
            
            if ($orphaned_count == 0) {
                data::responseJSON(true, "Database sync complete: {$total_checked} files checked, no orphaned entries found");
                return;
            }
            
            // Clean up orphaned database entries in batches
            $batch_size = 100;
            $cleaned_count = 0;
            
            for ($i = 0; $i < count($media_files); $i += $batch_size) {
                $batch_end = min($i + $batch_size, count($media_files));
                $batch_ids = array();
                
                // Collect orphaned IDs for this batch
                for ($j = $i; $j < $batch_end; $j++) {
                    $filepath = $media_files[$j]['filepath'];
                    if (!empty($filepath) && !file_exists($filepath)) {
                        $batch_ids[] = $media_files[$j]['id'];
                    }
                }
                
                if (empty($batch_ids)) {
                    continue;
                }
                
                // Start transaction for this batch
                data::query("START TRANSACTION", true);
                
                try {
                    // Update Media table
                    $ids_str = implode(',', $batch_ids);
                    data::query("UPDATE Media SET archive = 1 WHERE id IN ({$ids_str})", true);
                    
                    // Update EventsCam table
                    data::query("UPDATE EventsCam SET archive = 1 WHERE media_id IN ({$ids_str})", true);
                    
                    // Commit transaction
                    data::query("COMMIT", true);
                    
                    $cleaned_count += count($batch_ids);
                    
                } catch (Exception $e) {
                    // Rollback transaction
                    data::query("ROLLBACK", true);
                    throw new Exception("Failed to clean up batch: " . $e->getMessage());
                }
            }
            
            data::responseJSON(true, "Database sync complete: {$cleaned_count} orphaned entries cleaned up out of {$total_checked} total files");
            
        } catch (Exception $e) {
            data::responseJSON(false, "Database sync failed: " . $e->getMessage());
        }
    }
}
