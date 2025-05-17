<?php

/**
 * OnvifService - A PHP class to communicate with the ONVIF service API
 * 
 * This class provides methods to interact with the ONVIF service API,
 * including device discovery and event type retrieval.
 */
class OnvifService {
    private $apiUrl;
    private $timeout;
    private $devMode;

    /**
     * Constructor
     * 
     * @param string $apiUrl The URL of the ONVIF service API
     * @param int $timeout The timeout in seconds for API requests
     * @param bool $devMode Whether to enable development mode
     */
    public function __construct($apiUrl = 'http://127.0.0.1:4000', $timeout = 30, $devMode = false) {
        $this->apiUrl = rtrim($apiUrl, '/');
        $this->timeout = $timeout;
        $this->devMode = $devMode;
    }

    /**
     * Log a message to the discovery log file
     * 
     * @param string $message The message to log
     */
    private function log($message) {
        $logFile = '/tmp/php_discover.log';
        $timestamp = date('Y-m-d H:i:s');
        $prefix = '[OnvifService] ';
        file_put_contents($logFile, "[$timestamp] $prefix$message\n", FILE_APPEND);
    }

    /**
     * Get the configured API URL
     * 
     * @return string The API URL
     */
    public function getApiUrl() {
        return $this->apiUrl;
    }

    /**
     * Discover ONVIF devices on the network
     * 
     * @return array Array of discovered devices
     * @throws Exception if the discovery fails
     */
    public function discoverDevices() {
        $this->log("Making request to ONVIF service at: " . $this->apiUrl . "/devices/discover");
        
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $this->apiUrl . "/devices/discover");
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_POST, false);  // Change to GET request
        curl_setopt($ch, CURLOPT_HTTPHEADER, array(
            'Content-Type: application/json',
            'Accept: application/json'
        ));
        curl_setopt($ch, CURLOPT_TIMEOUT, 30);        // Increase timeout to 30 seconds
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 5);  // Keep connect timeout short
        
        // Disable SSL certificate verification for development
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        $curlError = curl_error($ch);
        $curlErrno = curl_errno($ch);
        
        curl_close($ch);
        
        $this->log("ONVIF service response code: " . $httpCode);
        $this->log("ONVIF service response: " . $response);
        
        if ($curlErrno) {
            $this->log("cURL Error: " . $curlError);
            return array(
                'status' => 'error',
                'message' => "Failed to connect to ONVIF service: " . $curlError,
                'data' => array()
            );
        }
        
        if ($httpCode !== 200) {
            return array(
                'status' => 'error',
                'message' => "ONVIF service returned error code: " . $httpCode,
                'data' => array()
            );
        }
        
        $data = json_decode($response, true);
        if (json_last_error() !== JSON_ERROR_NONE) {
            $this->log("JSON decode error: " . json_last_error_msg());
            return array(
                'status' => 'error',
                'message' => "Failed to parse ONVIF service response: " . json_last_error_msg(),
                'data' => array()
            );
        }
        
        return array(
            'status' => 'success',
            'message' => 'Devices discovered successfully',
            'data' => isset($data['data']) ? $data['data'] : array()
        );
    }

    /**
     * Get event types from an ONVIF device
     * 
     * @param string $ipAddress Device IP address
     * @param int $port Device ONVIF port
     * @param string $username Device username
     * @param string $password Device password
     * @return array Event types
     * @throws Exception if request fails
     */
    public function getEventTypes($ipAddress, $port, $username, $password)
    {
        $url = sprintf("%s/events/types?ip=%s&port=%d&username=%s&password=%s",
            $this->apiUrl,
            urlencode($ipAddress),
            intval($port),
            urlencode($username),
            urlencode($password)
        );

        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_TIMEOUT, 5);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 5);
        curl_setopt($ch, CURLOPT_FAILONERROR, true);

        if ($this->devMode) {
            curl_setopt($ch, CURLOPT_VERBOSE, true);
            $verbose = fopen('php://temp', 'w+');
            curl_setopt($ch, CURLOPT_STDERR, $verbose);
        }

        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

        if ($this->devMode) {
            rewind($verbose);
            $verboseLog = stream_get_contents($verbose);
            error_log("ONVIF getEventTypes verbose output: " . $verboseLog);
        }

        if (curl_errno($ch)) {
            $error = curl_error($ch);
            curl_close($ch);
            throw new Exception("Connection error: " . $error);
        }

        curl_close($ch);

        if ($httpCode !== 200) {
            throw new Exception("HTTP error: " . $httpCode);
        }

        $data = json_decode($response, true);
        if (json_last_error() !== JSON_ERROR_NONE) {
            throw new Exception("Invalid JSON response: " . json_last_error_msg());
        }

        if (!isset($data['eventTypes']) || !is_array($data['eventTypes'])) {
            throw new Exception("Invalid response format");
        }

        return $data;
    }

    /**
     * Get RTSP URLs for a device
     * 
     * @param string $ip Device IP address
     * @param int $port ONVIF port
     * @param string $username Device username
     * @param string $password Device password
     * @return array Array containing main and sub stream URLs
     * @throws Exception if the RTSP URL retrieval fails
     */
    public function getRtspUrls($ip, $port, $username, $password) {
        $isDev = defined('DEVELOPMENT_MODE') && DEVELOPMENT_MODE;
        
        if ($isDev) {
            error_log("ONVIF Service: Getting RTSP URLs for device: $ip:$port");
        }
        
        $data = [
            'xaddr' => "http://{$ip}:{$port}/onvif/device_service",
            'username' => $username,
            'password' => $password
        ];
        
        $ch = curl_init($this->apiUrl . '/devices/rtsp');
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_TIMEOUT, $this->timeout);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 5);
        curl_setopt($ch, CURLOPT_FAILONERROR, true);
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
        curl_setopt($ch, CURLOPT_HTTPHEADER, ['Content-Type: application/json']);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        $curlError = curl_error($ch);
        $curlErrno = curl_errno($ch);
        
        curl_close($ch);

        // Handle connection errors
        if ($curlErrno) {
            $errorMessage = "ONVIF Service: Connection error: " . $curlError;
            if ($isDev) {
                error_log($errorMessage);
            }
            throw new Exception($errorMessage);
        }

        // Handle HTTP errors
        if ($httpCode !== 200) {
            $errorMessage = "ONVIF Service: Failed to get RTSP URLs: HTTP " . $httpCode;
            if ($isDev) {
                error_log($errorMessage);
                error_log("ONVIF Service: Response: " . $response);
            }
            throw new Exception($errorMessage);
        }

        $data = json_decode($response, true);
        if (json_last_error() !== JSON_ERROR_NONE) {
            $errorMessage = "ONVIF Service: JSON Decode Error: " . json_last_error_msg();
            if ($isDev) {
                error_log($errorMessage);
            }
            throw new Exception($errorMessage);
        }
        
        return $data;
    }
} 