<?php
session_start();

// 1. Check if user is logged in (adjust this to your session logic)
if (!isset($_SESSION['user_id'])) {
    http_response_code(401);
    echo json_encode(['error' => 'Not logged in']);
    exit;
}

// 2. API credentials (server-side only)
$api_url = 'http://localhost:7005/stats'; // Adjust endpoint as needed
$api_user = 'apiuser';
$api_pass = 'apipassword';

// 3. Make cURL request to API
$ch = curl_init($api_url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_USERPWD, "$api_user:$api_pass");
curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

$response = curl_exec($ch);
$httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
curl_close($ch);

// 4. Pass through API response
http_response_code($httpcode);
header('Content-Type: application/json');
echo $response; 