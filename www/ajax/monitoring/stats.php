<?php
session_start();

// 1. Check if user is logged in (use the correct session variable)
if (empty($_SESSION['id'])) {
    http_response_code(401);
    echo json_encode(['error' => 'Not logged in']);
    exit;
}

// 2. Get current user's info from database
require_once '../lib/lib.php';

$user = new user('id', $_SESSION['id']);
if (!$user->info) {
    http_response_code(401);
    echo json_encode(['error' => 'Invalid user session']);
    exit;
}

// Check if user has remote access permission
if (!$user->info['access_remote']) {
    http_response_code(403);
    echo json_encode(['error' => 'User lacks remote access permission']);
    exit;
}

// 3. For now, use a simple approach - check if the API server is running
// and return basic system stats from the web interface instead of the API

// Check if bc-server is running
$server_running = shell_exec('pidof bc-server') ? true : false;

if (!$server_running) {
    http_response_code(503);
    echo json_encode(['error' => 'Bluecherry server not running']);
    exit;
}

// 4. Get basic system stats from /proc filesystem
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

// 5. Format response for the monitoring template
$timestamps = [time()];

$out = [
    'timestamps' => $timestamps,
    'cpu' => [$cpu_usage],
    'memory' => [$memory_usage]
];

header('Content-Type: application/json');
echo json_encode($out); 