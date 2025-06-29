<?php
session_start();

// 1. Check if user is logged in (use the correct session variable)
if (empty($_SESSION['id'])) {
    http_response_code(401);
    echo json_encode(['error' => 'Not logged in']);
    exit;
}

// 2. API credentials (edit here as needed)
$api_url = 'http://localhost:7005/stats'; // Adjust endpoint as needed
$api_user = 'apiuser'; // <-- Set your API username here
$api_pass = 'apipassword'; // <-- Set your API password here

// 3. Make cURL request to API
$ch = curl_init($api_url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_USERPWD, "$api_user:$api_pass");
curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

$response = curl_exec($ch);
$httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
curl_close($ch);

// 4. Reformat API response if needed
header('Content-Type: application/json');
http_response_code($httpcode);

// Try to decode and reformat if necessary
$data = json_decode($response, true);
if (is_array($data) && isset($data['timestamps']) && isset($data['cpu']) && isset($data['memory'])) {
    // Already in expected format
    echo json_encode($data);
} else if (is_array($data)) {
    // Try to reformat (example: if API returns flat stats)
    $out = [
        'timestamps' => $data['timestamps'] ?? [],
        'cpu' => $data['cpu'] ?? [],
        'memory' => $data['memory'] ?? []
    ];
    echo json_encode($out);
} else {
    // Pass through raw response (could be error)
    echo $response;
} 