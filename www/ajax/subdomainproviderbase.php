<?php

class subdomainproviderbase extends Controller {

    const API_BASE_URL_NAME = 'G_SUBDOMAIN_API_BASE_URL';
    const AUTO_UPDATE_IPV4_CONFIG_NAME = 'G_SUBDOMAIN_AUTO_UPDATE_IPV4';
    const AUTO_UPDATE_IPV6_CONFIG_NAME = 'G_SUBDOMAIN_AUTO_UPDATE_IPV6';
    const AUTO_UPDATE_IPV4_VALUE_NAME = 'G_SUBDOMAIN_AUTO_UPDATE_IPV4_VALUE';
    const AUTO_UPDATE_IPV6_VALUE_NAME = 'G_SUBDOMAIN_AUTO_UPDATE_IPV6_VALUE';

    protected $subdomainApiBaseUrlCache = null;

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

        if (!empty($this->subdomainApiBaseUrlCache)) {
            return $this->subdomainApiBaseUrlCache;
        }

        $result = $this->getGlobalSettingsValue(self::API_BASE_URL_NAME);

        if (empty($result)) {
            throw new \RuntimeException(self::API_BASE_URL_NAME .
                ' parameter is not defined in global settings');
        }

        $this->subdomainApiBaseUrlCache = $result;

        return $result;
    }

    protected function getLicenseKey() {
        // Get the license key from database
        $licenses = data::query('SELECT `license` FROM `Licenses` LIMIT 1');

        if (empty($licenses)) {
            throw new \RuntimeException('Any license isn\'t activated' .
                ' in bluecherry system');
        }

		$license_key = $licenses[0]['license'];
        return $license_key;
    }

    protected function getLicenseId() {
        // Get the license key
        $license_key = $this->getLicenseKey();

        // Get the license id
        $result = $this->postToApi('/get-license-id', [
            'licenseKey' => $license_key
        ]);
    
        if (empty($result['success'])) {
            throw new \RuntimeException($license_key .
                '\'s id not found in cryptlex');
        }

        $license_id = $result['licenseId'];

        return $license_id;
        
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
            throw new \RuntimeException('API request is failed.');
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

    protected function getFromApi($path, $query = [], $headers = [], $customRequest = null) {

        $baseUrl = $this->getSubdomainApiBaseUrl();

        if (!empty($query)) {
            $path .= http_build_query($query);
        }

        return $this->executeApiGetRequest($baseUrl . $path, $headers, $customRequest);
    }

    protected function getFromApiWithToken($path, $query = [], $headers = [], $tokenOptional = false, $customRequest = null) {

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
        ], $headers), $customRequest);

    }

    protected function getServerPublicIp($tryIpv6 = false) {
        return $this->executeApiGetRequest('https://' .  ($tryIpv6 ? 'api64' : 'api') . '.ipify.org/?format=json');
    }

    protected function executeApiGetRequest($url, $headers = [], $customRequest = null) {

        $curl = curl_init($url);

        curl_setopt($curl, CURLOPT_CONNECTTIMEOUT, 4);
        curl_setopt($curl, CURLOPT_HTTPHEADER, array_merge(['Content-Type: application/json'], $headers));
        curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($curl, CURLOPT_FOLLOWLOCATION, true);

        if (!empty($customRequest)) {
            curl_setopt($curl, CURLOPT_CUSTOMREQUEST, $customRequest);
        }

        $result = curl_exec($curl);

        curl_close($curl);


        if ($result === false) {
            throw new \RuntimeException('API request is failed.');
        }

        return json_decode($result, true);
    }

    protected function getGlobalSettingsValue($name) {
        $name = database::escapeString($name);

        $result = data::query('SELECT `value` FROM `GlobalSettings` WHERE `parameter` = \'' .
            $name . '\' LIMIT 1');

        if (!empty($result)) {
            return $result[0]['value'];
        }

        data::query('INSERT INTO `GlobalSettings`(`parameter`, `value`) VALUES(\'' .
            $name . '\', null)', true);

        return null;
    }

    protected function setGlobalSettingsValue($name, $value)
    {
        $name = database::escapeString($name);
        $value = database::escapeString($value);

        return data::query("UPDATE GlobalSettings SET value='$value' WHERE parameter='$name' LIMIT 1", true);
    }
}



?>
