<?php

class storage extends Controller {

    public function __construct(){
        parent::__construct();
        $this->chAccess('admin');
    }

public function getData()
{
    $this->setView('ajax.storage');
    $storage_check = $this->ctl('storagecheck');

    $locations = data::query("SELECT * FROM Storage");
    foreach(array_keys($locations) as $id) {
        $path = database::escapeString($locations[$id]['path']);
        $max_thresh = database::escapeString($locations[$id]['max_thresh']);
        
        // Check if we can access the directory
        $dir_status = $storage_check->change_directory_permissions($path);
        
        // Get disk space info with error checking
        $free_space = disk_free_space($path);
        $total_space = disk_total_space($path);
        
        if ($free_space === false || $total_space === false) {
            // Handle error case
            $locations[$id]['record_time'] = 0;
            $locations[$id]['used_percent'] = 0;
            continue;
        }
        
        $available_space = ($free_space - $total_space * (1.0 - $max_thresh/100) ) >> 20;
        if ($available_space < 0)
            $available_space = 0;
            
        $query = "SELECT ({$available_space}) * period / size AS result FROM (".
              "SELECT SUM(CASE WHEN end < start THEN 0 ELSE end - start END".
            ") period, SUM(size) DIV 1048576 size".
            " FROM Media WHERE filepath LIKE '{$path}%') q";
        $result = data::query($query);
        $locations[$id]['record_time'] = $result[0]['result'];
        
        if ($total_space > 0) {
            $locations[$id]['used_percent'] = ceil(($total_space - $free_space) / $total_space * 100);
        } else {
            $locations[$id]['used_percent'] = 0;
        }
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
