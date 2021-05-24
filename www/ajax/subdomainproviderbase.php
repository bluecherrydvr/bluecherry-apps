<?php

class subdomainproviderbase extends Controller {

    const CRYPTLEX_BASE_URL = 'https://api.cryptlex.com/v3/licenses';
    const API_BASE_URL_NAME = 'G_SUBDOMAIN_API_BASE_URL';
    private $subdomain_info;

    public function __construct() {
        parent::__construct();
        $this->chAccess('admin');
    }

    public function getData() {
        return $this->getNotImplementedResponse();
    }

    public function postData() {
        return $this->getNotImplementedResponse();
    }

    protected function getNotImplementedResponse() {
        return json_encode([
            'success' => false,
            'message' => 'this request method is not implemented'
        ]);
    }

    protected function getSubdomainApiBaseUrl() {
        $result = data::query('SELECT `value` FROM `GlobalSettings` WHERE `parameter` = \'' .
            self::API_BASE_URL_NAME . '\' LIMIT 1');

        if (empty($result)) {
            throw new \RuntimeException(self::API_BASE_URL_NAME .
                ' parameter is not defined in global settings');
        }

        return $result;
    }

    protected function getLicenseId() {
        // Get the license key from database
        $licenses = data::query('SELECT `license` FROM `Licenses` LIMIT 1');

        if (empty($licenses)) {
            throw new \RuntimeException('Any license isn\'t activated' .
                ' in bluecherry system');
        }

		$license_key = $licenses[0]['license'];

        // Get the cryptlex access token
        $this->subdomain_info = subdomain::getInstance();

        // Get the license list
        $license_id = null;
		$result = $this->getLicenseList($license_key, $i, 100);
		$len = count($result);

		if ($len > 0) {
			$license_id = $result[0]["id"];
		}

        // Check the result
        if ($license_id == null) {
            throw new \RuntimeException('There is no license id' .
            ' of the activated license key');
        }

        return $license_id;
        
    }

    private function getLicenseList($license_key, $page, $limit) {
        // Get the URL of web api for querying the license id
        $baseUrl = self::CRYPTLEX_BASE_URL;
        $params = array(
            'page' => $page,
            'limit' => $limit,
            'query' => $license_key,
        );
    
        $url = $baseUrl . "?" . http_build_query($params);
    
        // Initialize curl
        $curl = curl_init();
        curl_setopt($curl, CURLOPT_URL, $url);
        curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
    
        $headers = array(
            "Accept: application/json",
            "Authorization: Bearer " . $this->subdomain_info->cryptlex_access_token,
        );
    
        curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
        curl_setopt($curl, CURLOPT_SSL_VERIFYHOST, false);
        curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, false);

        // Execute curl
        $result = curl_exec($curl);
    
        curl_close($curl);
    
        // Check the result
        if ($result === false) {
            throw new \RuntimeException('cryptlex api request is failed');
        }
        
        return json_decode($result, true);    
    }

    protected function postToApi($path, $body, $headers = []) {

        $baseUrl = $this->getSubdomainApiBaseUrl();

        $curl = curl_init($baseUrl . $path);

        curl_setopt($curl, CURLOPT_CONNECTTIMEOUT, 4);
        curl_setopt($curl, CURLOPT_POST, true);
        curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($body));
        curl_setopt($curl, CURLOPT_HTTPHEADER, array_merge(['Content-Type: application/json'], $headers));
        curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($curl, CURLOPT_FOLLOWLOCATION, true);

        $result = curl_exec($curl);

        curl_close($curl);


        if ($result === false) {
            throw new \RuntimeException('api request is failed');
        }

        return json_decode($result, true);
    }

    protected function postToApiWithToken($path, $body, $headers = [], $tokenOptional = false) {
        try {
            $licenseId = $this->getLicenseId();
        } catch (\RuntimeException $exception) {

            if ($tokenOptional) {
                return $this->postToApi($path, $body, $headers);
            }

            throw $exception;
        }

        return $this->postToApi($path, $body, array_merge([
            'Authorization: Bearer ' . $licenseId
        ], $headers));
    }

    protected function getFromApi($path, $query = [], $headers = []) {

        $baseUrl = $this->getSubdomainApiBaseUrl();

        if (!empty($query)) {
            $path .= http_build_query($query);
        }

        $curl = curl_init($baseUrl . $path);

        curl_setopt($curl, CURLOPT_CONNECTTIMEOUT, 4);
        curl_setopt($curl, CURLOPT_HTTPHEADER, array_merge(['Content-Type: application/json'], $headers));
        curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($curl, CURLOPT_FOLLOWLOCATION, true);

        $result = curl_exec($curl);

        curl_close($curl);


        if ($result === false) {
            throw new \RuntimeException('api request is failed');
        }

        return json_decode($result, true);
    }

    protected function getFromApiWithToken($path, $query = [], $headers = [], $tokenOptional = false) {

        try {
            $licenseId = $this->getLicenseId();
        } catch (\RuntimeException $exception) {

            if ($tokenOptional) {
                return $this->getFromApi($path, $query, $headers);
            }

            throw $exception;
        }

        return $this->getFromApi($path, $query, array_merge([
            'Authorization: Bearer ' . $licenseId
        ], $headers));

    }

}



?>
