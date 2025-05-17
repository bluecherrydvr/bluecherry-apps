<?php
require_once __DIR__ . '/../lib/lib.php';

class onvifProxy extends Controller {
    private $onvifServiceUrl = 'http://127.0.0.1:4000';
    
    public function __construct() {
        parent::__construct();
        $this->chAccess('admin');
    }
    
    public function getData() {
        // Get the path from the request
        $requestUri = $_SERVER['REQUEST_URI'];
        $path = parse_url($requestUri, PHP_URL_PATH);
        $path = preg_replace('/^\/onvif-proxy/', '', $path);
        
        // Build the target URL
        $targetUrl = rtrim($this->onvifServiceUrl, '/') . $path;
        
        // Initialize cURL with proper timeout settings
        $ch = curl_init($targetUrl);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_TIMEOUT, 30);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 5);
        
        // Forward the request method and content type
        $method = $_SERVER['REQUEST_METHOD'];
        if ($method === 'POST') {
            curl_setopt($ch, CURLOPT_POST, true);
            $postData = file_get_contents('php://input');
            if ($postData) {
                curl_setopt($ch, CURLOPT_POSTFIELDS, $postData);
            }
        }
        
        // Forward all relevant headers
        $headers = [];
        $requestHeaders = getallheaders();
        foreach ($requestHeaders as $key => $value) {
            if (!in_array(strtolower($key), ['host', 'connection', 'content-length'])) {
                $headers[] = "$key: $value";
            }
        }
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        
        // Get response headers
        $responseHeaders = [];
        curl_setopt($ch, CURLOPT_HEADERFUNCTION, function($ch, $header) use (&$responseHeaders) {
            $len = strlen($header);
            $header = explode(':', $header, 2);
            if (count($header) < 2) return $len;
            
            $name = strtolower(trim($header[0]));
            $value = trim($header[1]);
            if (!empty($name)) {
                $responseHeaders[$name] = $value;
            }
            return $len;
        });
        
        // Execute the request
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        $contentType = curl_getinfo($ch, CURLINFO_CONTENT_TYPE);
        
        if (curl_errno($ch)) {
            $error = curl_error($ch);
            $errno = curl_errno($ch);
            curl_close($ch);
            
            // Provide more specific error messages
            $errorMessage = 'ONVIF Service Error: ';
            switch ($errno) {
                case CURLE_OPERATION_TIMEOUTED:
                    $errorMessage .= 'Connection timed out';
                    break;
                case CURLE_COULDNT_CONNECT:
                    $errorMessage .= 'Could not connect to ONVIF service';
                    break;
                default:
                    $errorMessage .= $error;
            }
            
            http_response_code(502);
            header('Content-Type: application/json');
            die(json_encode([
                'error' => 'Proxy Error',
                'message' => $errorMessage,
                'code' => $errno
            ]));
        }
        
        curl_close($ch);
        
        // Forward the response code
        http_response_code($httpCode);
        
        // Forward content type if available
        if ($contentType) {
            header('Content-Type: ' . $contentType);
        }
        
        // Forward other important headers
        foreach ($responseHeaders as $name => $value) {
            if (in_array($name, ['content-type', 'content-length', 'cache-control'])) {
                header("$name: $value");
            }
        }
        
        // Forward the response
        echo $response;
        exit;
    }
} 