<?php

require_once 'subdomainproviderbase.php';

class subdomainprovider extends subdomainproviderbase
{

    public function getData() {

        $this->setView('ajax.subdomainprovider');
        $this->initForm();
    }

    protected function initForm() {
        $this->view->actualSubdomain = '';
        $this->view->actualIpv4Value = $this->getRemoteIp();
        $this->view->actualIpv6Value = '';

        try {
            $actualConfig = $this->getFromApiWithToken('/actual-configuration');
        } catch (\RuntimeException $ex) {
            $this->view->licenseIdExists = false;
            return;
        }

        $this->view->licenseIdExists = true;

        if (!empty($actualConfig)) {
            $this->view->actualSubdomain = $actualConfig['subdomain'];

            foreach ($actualConfig['records'] as $record) {
                switch ($record['type']) {
                    case 'a': $this->view->actualIpv4Value = $record['value']; break;
                    case 'aaaa' : $this->view->actualIpv6Value = $record['value']; break;
                }
            }
        }
    }

    public function postData() {

        $subdomain = $_POST['subdomain_name'];
        $ipv4Value = $_POST['server_ip_address_4'];
        $ipv6Value = $_POST['server_ip_address_6'];

        if (empty($subdomain) || empty($ipv4Value)) {
            header('Location: /subdomainprovider?status=0');
            return;
        }

        try {
            $result = $this->postToApiWithToken('/assign-ip-address', [
                'subdomain' => $subdomain,
                'ip_address' => $ipv4Value,
                'type' => 4
            ]);

            if (empty($result['success'])) {
                header('Location: /subdomainprovider?status=0');
                return;
            }

            if (!empty($ipv6Value)) {
                $result = $this->postToApiWithToken('/assign-ip-address', [
                    'subdomain' => $subdomain,
                    'ip_address' => $ipv6Value,
                    'type' => 6
                ]);

                if (empty($result['success'])) {
                    header('Location: /subdomainprovider?status=0');
                    return;
                }
            }

        } catch (\RuntimeException $exception) {
            header('Location: /subdomainprovider?status=0');
            return;
        }


        header('Location: /subdomainprovider?status=1');
    }

    private function getRemoteIp()
    {
        $output = array();
        $ret = 0;
        exec("wget -qO - icanhazip.com", $output, $ret);

        if ($ret != 0) {
            exec("curl ifconfig.me", $output, $ret);
        }

        if ($ret != 0) {
            return '';
        }

        return $output[0];    
    }

}
?>