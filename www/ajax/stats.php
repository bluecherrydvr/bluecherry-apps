<?php

class stats extends Controller {

    public function __construct() {
        parent::__construct();
        $this->chAccess('admin');
    }

    public function start($uri = array()) {
        // Prevent any output before JSON
        ob_clean();
        header('Content-Type: application/json');
        
        try {
            $range = isset($_GET['range']) ? $_GET['range'] : '1h';
            $this->getStats($range);
        } catch (Exception $e) {
            // Return error as JSON instead of HTML
            echo json_encode([
                'error' => true,
                'message' => 'Failed to load stats: ' . $e->getMessage(),
                'live' => ['cpu' => 0, 'memory' => 0, 'disk' => 0, 'server_running' => false, 'timestamp' => time()],
                'historical' => [],
                'storage' => []
            ]);
        }
    }

    private function getNetworkStats() {
        $network_stats = [];
        
        // Get network interface names and current stats
        $net_dir = '/sys/class/net/';
        if (is_dir($net_dir)) {
            $interfaces = scandir($net_dir);
            $iface_index = 0;
            
            foreach ($interfaces as $iface) {
                if ($iface !== '.' && $iface !== '..') {
                    // Skip loopback and virtual interfaces
                    if (substr($iface, 0, 2) !== 'lo' && 
                        substr($iface, 0, 6) !== 'docker' &&
                        substr($iface, 0, 4) !== 'veth') {
                        
                        // Get interface statistics
                        $rx_bytes = 0;
                        $tx_bytes = 0;
                        $is_up = false;
                        
                        $rx_file = $net_dir . $iface . '/statistics/rx_bytes';
                        $tx_file = $net_dir . $iface . '/statistics/tx_bytes';
                        $operstate_file = $net_dir . $iface . '/operstate';
                        
                        if (file_exists($rx_file)) {
                            $rx_bytes = intval(file_get_contents($rx_file));
                        }
                        if (file_exists($tx_file)) {
                            $tx_bytes = intval(file_get_contents($tx_file));
                        }
                        if (file_exists($operstate_file)) {
                            $operstate = trim(file_get_contents($operstate_file));
                            $is_up = ($operstate === 'up');
                        }
                        
                        // Get IP address if available
                        $ip_address = '';
                        $addr_file = $net_dir . $iface . '/address';
                        if (file_exists($addr_file)) {
                            $ip_address = trim(file_get_contents($addr_file));
                        }
                        
                        $network_stats[] = [
                            'index' => $iface_index,
                            'name' => $iface,
                            'is_up' => $is_up,
                            'ip_address' => $ip_address,
                            'rx_bytes' => $rx_bytes,
                            'tx_bytes' => $tx_bytes,
                            'rx_mbps' => round($rx_bytes / (1024 * 1024), 2),
                            'tx_mbps' => round($tx_bytes / (1024 * 1024), 2)
                        ];
                        
                        $iface_index++;
                    }
                }
            }
        }
        
        return $network_stats;
    }

    private function getStats($range) {
        $rrd_available = false;
        $rrd_file = null;
        $rrd_paths = [
            '/var/lib/bluecherry/monitor.rrd',
            '/tmp/bluecherry-monitor.rrd',
            '/var/tmp/bluecherry-monitor.rrd'
        ];
        foreach ($rrd_paths as $path) {
            if (file_exists($path)) {
                $rrd_available = true;
                $rrd_file = $path;
                break;
            }
        }

        // Get live stats
        $live_stats = $this->getLiveStats();
        
        // Get historical data if RRD is available
        $historical_data = [];
        if ($rrd_available && $rrd_file) {
            $historical_data = $this->getHistoricalData($rrd_file, $range);
        }
        
        // Get storage path stats
        $storage_stats = $this->getStorageStats();
        
        // Get network interface stats
        $network_stats = $this->getNetworkStats();
        
        $response = [
            'live' => $live_stats,
            'historical' => $historical_data,
            'storage' => $storage_stats,
            'network' => $network_stats,
            'rrd_available' => $rrd_available,
            'time_range' => $range,
            'resolution' => '10s'
        ];
        
        echo json_encode($response);
    }

    private function getHistoricalData($rrd_file, $range) {
        $rrd_range = $this->convertRangeToRRD($range);
        $resolution = $this->getResolution($range);
        $rrdtool_path = '/usr/bin/rrdtool';
        if (file_exists($rrdtool_path)) {
            $cmd = "$rrdtool_path fetch $rrd_file AVERAGE --start $rrd_range --resolution $resolution 2>&1";
            $output = shell_exec($cmd);
            if ($output) {
                $lines = explode("\n", trim($output));
                $historical_data = [];
                foreach ($lines as $line) {
                    $line = trim($line);
                    if (empty($line) || strpos($line, '#') === 0) continue;
                    $parts = preg_split('/\s+/', $line);
                    if (count($parts) >= 4) {
                        $timestamp = intval($parts[0]);
                        $cpu_str = trim($parts[1]);
                        $mem_str = trim($parts[2]);
                        $disk_str = trim($parts[3]);
                        $cpu = (strpos($cpu_str, 'nan') !== false || $cpu_str === '-nan') ? null : floatval($cpu_str);
                        $mem = (strpos($mem_str, 'nan') !== false || $mem_str === '-nan') ? null : floatval($mem_str);
                        $disk = (strpos($disk_str, 'nan') !== false || $disk_str === '-nan') ? null : floatval($disk_str);
                        
                        // Parse network interface data
                        $network_data = [];
                        for ($i = 4; $i < count($parts); $i += 2) {
                            if (isset($parts[$i]) && isset($parts[$i + 1])) {
                                $rx_str = trim($parts[$i]);
                                $tx_str = trim($parts[$i + 1]);
                                $rx = (strpos($rx_str, 'nan') !== false || $rx_str === '-nan') ? null : floatval($rx_str);
                                $tx = (strpos($tx_str, 'nan') !== false || $tx_str === '-nan') ? null : floatval($tx_str);
                                
                                // Calculate interface index (0-based)
                                $iface_index = ($i - 4) / 2;
                                $network_data[$iface_index] = [
                                    'rx' => $rx !== null ? round($rx, 2) : null,
                                    'tx' => $tx !== null ? round($tx, 2) : null
                                ];
                            }
                        }
                        
                        if ($cpu !== null || $mem !== null || $disk !== null || !empty($network_data)) {
                            $historical_data[] = [
                                'timestamp' => $timestamp,
                                'cpu' => $cpu !== null ? round($cpu, 2) : null,
                                'memory' => $mem !== null ? round($mem, 2) : null,
                                'disk' => $disk !== null ? round($disk, 2) : null,
                                'network' => $network_data
                            ];
                        }
                    }
                }
            }
        }
        return $historical_data;
    }

    private function getStorageStats() {
        // Get storage paths from Bluecherry Storage table
        $storage_paths = [];
        
        // Include root filesystem
        $storage_paths['/'] = 'Root Filesystem';
        
        // Get paths from Storage table
        $query = "SELECT path FROM Storage ORDER BY priority ASC";
        $result = data::query($query);
        
        if ($result) {
            foreach ($result as $row) {
                $path = $row['path'];
                if (!empty($path)) {
                    // Use the path as the key and a friendly name as the label
                    $storage_paths[$path] = basename($path);
                }
            }
        }
        
        $stats = [];
        foreach ($storage_paths as $path => $label) {
            if (is_dir($path)) {
                $total = disk_total_space($path);
                $free = disk_free_space($path);
                $used = $total - $free;
                $usage_percent = ($total > 0) ? round(($used / $total) * 100, 2) : 0;
                
                // Get filesystem type
                $filesystem = 'unknown';
                if (function_exists('posix_statvfs')) {
                    $statvfs = posix_statvfs($path);
                    if ($statvfs) {
                        $filesystem = $statvfs['f_type'] ?? 'unknown';
                    }
                }
                
                // Try to get filesystem type from /proc/mounts
                if ($filesystem === 'unknown') {
                    $mounts = file_get_contents('/proc/mounts');
                    if ($mounts) {
                        $lines = explode("\n", $mounts);
                        foreach ($lines as $line) {
                            $parts = explode(' ', trim($line));
                            if (count($parts) >= 3) {
                                $mount_point = $parts[1];
                                $fs_type = $parts[2];
                                
                                // Find the longest matching mount point
                                if (strpos($path, $mount_point) === 0) {
                                    if (strlen($mount_point) > strlen($filesystem) || $filesystem === 'unknown') {
                                        $filesystem = $fs_type;
                                    }
                                }
                            }
                        }
                    }
                }
                
                $stats[] = [
                    'path' => $path,
                    'label' => $label,
                    'filesystem' => $filesystem,
                    'total_size' => $total,
                    'used_size' => $used,
                    'free_size' => $free,
                    'total_gb' => round($total / (1024 * 1024 * 1024), 2),
                    'used_gb' => round($used / (1024 * 1024 * 1024), 2),
                    'free_gb' => round($free / (1024 * 1024 * 1024), 2),
                    'usage_percent' => $usage_percent
                ];
            }
        }
        
        return $stats;
    }

    private function convertRangeToRRD($range) {
        switch ($range) {
            case '1h': return '-1h';
            case '6h': return '-6h';
            case '1d': return '-1d';
            case '1w': return '-1w';
            case '1m': return '-1m';
            default: return '-1h';
        }
    }

    private function getResolution($range) {
        switch ($range) {
            case '1h': return '10s';
            case '6h': return '1m';
            case '1d': return '5m';
            case '1w': return '1h';
            case '1m': return '1h';
            default: return '10s';
        }
    }

    private function getLiveStats() {
        $cpu_usage = 0;
        
        // Get CPU usage from /proc/stat (more accurate than load average)
        $stat = file_get_contents('/proc/stat');
        if ($stat) {
            $lines = explode("\n", $stat);
            foreach ($lines as $line) {
                if (strpos($line, 'cpu ') === 0) {
                    $parts = preg_split('/\s+/', trim($line));
                    if (count($parts) >= 5) {
                        $user = intval($parts[1]);
                        $nice = intval($parts[2]);
                        $system = intval($parts[3]);
                        $idle = intval($parts[4]);
                        $iowait = intval($parts[5]);
                        $irq = intval($parts[6]);
                        $softirq = intval($parts[7]);
                        
                        $total = $user + $nice + $system + $idle + $iowait + $irq + $softirq;
                        $used = $total - $idle - $iowait;
                        
                        if ($total > 0) {
                            $cpu_usage = round(($used / $total) * 100, 2);
                        }
                    }
                    break;
                }
            }
        }
        
        // Fallback to load average if /proc/stat fails
        if ($cpu_usage == 0) {
            $loadavg = file_get_contents('/proc/loadavg');
            if ($loadavg) {
                $loads = explode(' ', trim($loadavg));
                // Convert load average to rough percentage (load > 1.0 = 100%+)
                $cpu_usage = min(100, round(floatval($loads[0]) * 100, 2));
            }
        }
        $mem_usage = 0;
        $meminfo = file_get_contents('/proc/meminfo');
        if ($meminfo) {
            preg_match('/MemTotal:\s+(\d+)/', $meminfo, $total);
            preg_match('/MemAvailable:\s+(\d+)/', $meminfo, $available);
            if ($total && $available) {
                $mem_usage = round(100 - ($available[1] / $total[1] * 100), 2);
            }
        }
        $disk_usage = 0;
        $df_output = shell_exec('df / 2>/dev/null | tail -1');
        if ($df_output) {
            $parts = preg_split('/\s+/', trim($df_output));
            if (count($parts) >= 5) {
                $disk_usage = intval($parts[4]);
            }
        }
        $server_running = shell_exec('pidof bc-server') ? true : false;
        return [
            'cpu' => $cpu_usage,
            'memory' => $mem_usage,
            'disk' => $disk_usage,
            'server_running' => $server_running,
            'timestamp' => time()
        ];
    }
}
?> 