<?php

require_once 'subdomainproviderbase.php';

class subdomainprovider extends subdomainproviderbase
{

    const EMAIL_ACCOUNT_CONFIG_NAME = 'G_SUBDOMAIN_EMAIL_ACCOUNT';

    public function getData() {

        $this->setView('ajax.subdomainprovider');
        $this->initForm();
    }

    protected function initForm() {
        $this->view->actualSubdomain = '';
        $this->view->actualEmail     = $this->varpub->global_settings->data[self::EMAIL_ACCOUNT_CONFIG_NAME];
        $this->view->actualIpv4Value = '';
        $this->view->actualIpv6Value = '';
        $this->view->autoUpdateIpv4 = $this->getGlobalSettingsValue(self::AUTO_UPDATE_IPV4_CONFIG_NAME );
        $this->view->autoUpdateIpv6 = $this->getGlobalSettingsValue(self::AUTO_UPDATE_IPV6_CONFIG_NAME );
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
//                    case 'a': $this->view->actualIpv4Value = $record['value']; break;
                    case 'aaaa' : $this->view->actualIpv6Value = $record['value']; break;
                }
            }
        }
        $ipv4 = $this->getServerPublicIp(false);
        $this->view->actualIpv4Value = (isset($ipv4) && !empty($ipv4['ip'])) ? $ipv4['ip'] : $this->view->actualIpv4Value;
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

        $this->setGlobalSettingsValue(self::AUTO_UPDATE_IPV4_CONFIG_NAME,
            empty($_POST['update_ip_address_4_auto']) ? 0 : 1);

        $this->setGlobalSettingsValue(self::AUTO_UPDATE_IPV6_CONFIG_NAME,
            empty($_POST['update_ip_address_6_auto']) ? 0 : 1);


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

            $this->setGlobalSettingsValue(self::AUTO_UPDATE_IPV4_VALUE_NAME, $ipv4Value);

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

                $this->setGlobalSettingsValue(self::AUTO_UPDATE_IPV6_VALUE_NAME, $ipv6Value);
            }

        } catch (\RuntimeException $exception) {
            header('Location: /subdomainprovider?status=0');
            return;
        }

        // Update subdomain certs
        $token = $this->getLicenseId();

        exec("sudo -b /usr/share/bluecherry/scripts/update_subdomain_certs.sh $subdomain $email $token", $output, $return_var);

        if ($return_var === 0) {
            $this->setGlobalSettingsValue(self::EMAIL_ACCOUNT_CONFIG_NAME, $email);
            header('Location: /subdomainprovider?status=1');
            return;
        }

        $this->setGlobalSettingsValue(self::EMAIL_ACCOUNT_CONFIG_NAME, $email);
        header('Location: /subdomainprovider?status=3');
    }


}
?>
