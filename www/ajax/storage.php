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
            
            // Get available space considering threshold
            $available_space = (disk_free_space($path) - disk_total_space($path) * (1.0 - $max_thresh/100) ) >> 20;
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
                    WHERE filepath LIKE '{$path}%'";
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
            $locations[$id]['used_percent'] = ceil((disk_total_space($path) - disk_free_space($path)) / disk_total_space($path) * 100);
            
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

    public function postData()
    {
        $storage_check = $this->ctl('storagecheck');

        $values = Array();
        foreach($_POST['path'] as $key => $path){
            // Call the updated method from storagecheck
            $dir_status = $storage_check->change_directory_permissions($path);
            if ($dir_status[0] !== 'OK') {
                // If directory permissions change failed, return the error
                return data::responseJSON($dir_status[0], $dir_status[1]);
            }

            $max = intval($_POST['max'][$key]);
            $min = $max - 10;
            $values[] = "({$key}, '{$path}', {$max}, {$min})";
        }

        data::query("DELETE FROM Storage", true);

        $status = true;
        if (!empty($values)) {
            $values = implode(',', $values);
            $status = (data::query("INSERT INTO Storage (priority, path, max_thresh, min_thresh) VALUES {$values}", true)) ? $status : false;
        }

        data::responseJSON($status);
    }
}
