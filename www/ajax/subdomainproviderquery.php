<?php

require_once 'subdomainproviderbase.php';

class subdomainproviderquery extends subdomainproviderbase
{
    protected function querySubdomainName($name) {

        try {
            $response = $this->postToApi('/check-subdomain-availability', [
                'subdomain' => $name
            ], []);
        } catch (\RuntimeException $exception) {
            $response = [
                'success' => false,
                'message' => $exception->getMessage()
            ];
        }

        return json_encode($response);
    }

    public function getData() {
        return $this->querySubdomainName($_REQUEST['name']);
    }

    public function postData() {
        return $this->querySubdomainName($_REQUEST['name']);
    }
}
?>