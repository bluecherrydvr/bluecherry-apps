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
        $this->view->actualEmail     = $this->varpub->global_settings->data['G_SUBDOMAIN_EMAIL_ACCOUNT'];
        $this->view->actualIpv4Value = util::getRemoteIp();
        $this->view->actualIpv6Value = '';
        $this->view->licenseIdExists = false;

        // Check licensekey
        try {
            $licenseKey = $this->getLicenseKey();
        } catch (\RuntimeException $exception) {
            $this->view->licenseIdExists = false;
            return;
        }

        $this->view->licenseIdExists = true;

        // Call '/actual-configuration' api
        try {
            $actualConfig = $this->getFromApiWithToken('/actual-configuration');
        } catch (\RuntimeException $ex) {
            return;
        }

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

        $subdomain = '';
        $email     = '';
        $ipv4Value = '';
        $ipv6Value = '';

        if (!empty($_POST['subdomain_name'])) {
            $subdomain = $_POST['subdomain_name'];
        }

        if (!empty($_POST['subdomain_email'])) {
            $email = $_POST['subdomain_email'];
        }

        if (!empty($_POST['server_ip_address_4'])) {
            $ipv4Value = $_POST['server_ip_address_4'];
        }

        if (!empty($_POST['server_ip_address_6'])) {
            $ipv6Value = $_POST['server_ip_address_6'];
        }

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

        data::query("UPDATE GlobalSettings SET value='$email' WHERE parameter='G_SUBDOMAIN_EMAIL_ACCOUNT'");
        header('Location: /subdomainprovider?status=1');
    }

}
?>