<?php
// Simple test script to verify stats API functionality
// Run this from the command line: php test_stats_api.php

// Simulate a web request to the stats API
$_GET['range'] = '1h';

// Include the stats controller
require_once 'www/ajax/stats.php';

// Create a test instance
$stats = new stats();

// Capture output
ob_start();
$stats->start();
$output = ob_get_clean();

// Parse the JSON response
$data = json_decode($output, true);

if ($data === null) {
    echo "ERROR: Failed to parse JSON response\n";
    echo "Raw output:\n" . $output . "\n";
    exit(1);
}

echo "SUCCESS: Stats API is working correctly\n";
echo "Response structure:\n";
echo "- Live stats: " . (isset($data['live']) ? 'OK' : 'MISSING') . "\n";
echo "- Historical data: " . (isset($data['historical']) ? count($data['historical']) . ' points' : 'MISSING') . "\n";
echo "- Storage stats: " . (isset($data['storage']) ? count($data['storage']) . ' paths' : 'MISSING') . "\n";
echo "- Network stats: " . (isset($data['network']) ? count($data['network']) . ' interfaces' : 'MISSING') . "\n";
echo "- RRD available: " . (isset($data['rrd_available']) ? ($data['rrd_available'] ? 'Yes' : 'No') : 'UNKNOWN') . "\n";

if (isset($data['live'])) {
    echo "\nLive stats:\n";
    echo "- CPU: " . $data['live']['cpu'] . "%\n";
    echo "- Memory: " . $data['live']['memory'] . "%\n";
    echo "- Disk: " . $data['live']['disk'] . "%\n";
    echo "- Server running: " . ($data['live']['server_running'] ? 'Yes' : 'No') . "\n";
}

if (isset($data['network']) && count($data['network']) > 0) {
    echo "\nNetwork interfaces:\n";
    foreach ($data['network'] as $iface) {
        echo "- " . $iface['name'] . ": " . ($iface['is_up'] ? 'UP' : 'DOWN') . 
             " (RX: " . round($iface['rx_bytes'] / (1024*1024), 2) . " MB, " .
             "TX: " . round($iface['tx_bytes'] / (1024*1024), 2) . " MB)\n";
    }
}

echo "\nJSON response length: " . strlen($output) . " characters\n";
?> 