<?php

class monitor extends Controller {

    public function __construct()
    {
        parent::__construct();
        $this->chAccess('viewer'); // Allow any authenticated user to view monitoring
    }

    public function getData()
    {
        $this->setView('monitoring');
        
        // Check if user has remote access permission for API access
        $this->view->has_remote_access = $this->user->info['access_remote'];
        
        // Get basic system info
        $this->view->server_running = shell_exec('pidof bc-server') ? true : false;
        
        // Get system stats
        $cpu_usage = 0;
        $memory_usage = 0;
        
        // Get CPU usage from /proc/loadavg
        $loadavg = file_get_contents('/proc/loadavg');
        if ($loadavg) {
            $loads = explode(' ', $loadavg);
            $cpu_usage = floatval($loads[0]) * 100; // Convert load average to percentage
        }
        
        // Get memory usage from /proc/meminfo
        $meminfo = file_get_contents('/proc/meminfo');
        if ($meminfo) {
            preg_match('/MemTotal:\s+(\d+)/', $meminfo, $total_matches);
            preg_match('/MemAvailable:\s+(\d+)/', $meminfo, $available_matches);
            
            if (isset($total_matches[1]) && isset($available_matches[1])) {
                $total = intval($total_matches[1]);
                $available = intval($available_matches[1]);
                $used = $total - $available;
                $memory_usage = ($used / $total) * 100;
            }
        }
        
        $this->view->cpu_usage = $cpu_usage;
        $this->view->memory_usage = $memory_usage;
    }
} 