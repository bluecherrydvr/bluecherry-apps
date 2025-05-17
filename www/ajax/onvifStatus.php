<?php
require_once __DIR__ . '/../lib/lib.php';

class onvifStatus extends Controller {
    
    public function __construct() {
        parent::__construct();
        $this->chAccess('admin');
    }
    
    public function getData() {
        // Set the view to display the status information
        $this->setView('ajax.onvifStatus');
        
        // Check the ONVIF service status
        $results = $this->checkOnvifService();
        
        // Pass the results to the view
        $this->data['status'] = $results;
    }
    
    private function checkOnvifService() {
        $results = array();
        
        // Service URLs to check
        $service_url = "http://127.0.0.1:4000";
        
        // Check if we're on the development machine
        $hostname = gethostname();
        $results['hostname'] = $hostname;
        
        if ($hostname !== '192.168.86.90') {
            $service_url = "http://192.168.86.90:4000";
            $results['environment'] = 'development';
        } else {
            $results['environment'] = 'production';
        }
        
        $results['service_url'] = $service_url;
        
        // Test the discovery endpoint
        $discovery_url = $service_url . "/discovery";
        $results['discovery_url'] = $discovery_url;
        
        // Set up cURL
        $ch = curl_init($discovery_url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 5);
        curl_setopt($ch, CURLOPT_TIMEOUT, 10);
        
        // Execute the request
        $start_time = microtime(true);
        $json_output = curl_exec($ch);
        $end_time = microtime(true);
        
        // Get results
        $http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        $curl_error = curl_error($ch);
        $time_taken = round(($end_time - $start_time) * 1000); // in milliseconds
        
        curl_close($ch);
        
        // Store results
        $results['http_code'] = $http_code;
        $results['response_time'] = $time_taken . 'ms';
        $results['curl_error'] = $curl_error;
        
        if ($http_code == 200 && !empty($json_output)) {
            $data = json_decode($json_output, true);
            if (json_last_error() === JSON_ERROR_NONE) {
                $results['json_valid'] = true;
                if (isset($data['status']) && isset($data['data'])) {
                    $results['data_valid'] = true;
                    $results['camera_count'] = count($data['data']);
                    $results['sample_camera'] = !empty($data['data']) ? $data['data'][0] : null;
                } else {
                    $results['data_valid'] = false;
                }
            } else {
                $results['json_valid'] = false;
                $results['json_error'] = json_last_error_msg();
            }
        } else {
            $results['json_valid'] = false;
            $results['response_length'] = strlen($json_output);
        }
        
        return $results;
    }
} 