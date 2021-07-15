<?php

require_once 'subdomainproviderbase.php';

class subdomainprovidercron extends subdomainproviderbase
{

    // to override admin access to public
    public function __construct() {
        parent::__construct();
    }

    public function getData() {

        $results = [];

        if (boolval($this->getGlobalSettingsValue(self::AUTO_UPDATE_IPV4_CONFIG_NAME))) {
            $results['ipv4'] = $this->checkIpv4();
        } else {
            $results['ipv4'] = 'auto update is not enabled';
        }

        if (boolval($this->getGlobalSettingsValue(self::AUTO_UPDATE_IPV6_CONFIG_NAME))) {
            $results['ipv6'] = $this->checkIpv6();
        } else {
            $results['ipv4'] = 'auto update is not enabled';
        }

        return json_encode($results);
    }

    protected function checkIpv4()
    {
        $publicIp = $this->getServerPublicIp(false);

        if (filter_var($publicIp, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4) === false) {
            return 'not a valid ipv4 address';
        }

        // check if ip is not changed
        if ($this->getGlobalSettingsValue(self::AUTO_UPDATE_IPV4_VALUE_NAME) === $publicIp) {
            return 'not necessary to update because is not changed';
        }

        $result = $this->postToApiWithToken('/assign-ip-address', [
            'ip_address' => $publicIp,
            'type' => 4
        ]);

        if (!empty($result) && !empty($result['success'])) {
            $this->setGlobalSettingsValue(self::AUTO_UPDATE_IPV4_VALUE_NAME, $publicIp);
            return true;
        }

        return !empty($result['success']) ? $result['success'] : 'failed';
    }

    protected function checkIpv6()
    {
        $publicIp = $this->getServerPublicIp(true);

        if (filter_var($publicIp, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6) === false) {
            return 'not a valid ipv6 address';
        }

        // check if ip is not changed
        if ($this->getGlobalSettingsValue(self::AUTO_UPDATE_IPV6_VALUE_NAME) === $publicIp) {
            return 'not necessary to update because is not changed';
        }

        $result = $this->postToApiWithToken('/assign-ip-address', [
            'ip_address' => $publicIp,
            'type' => 6
        ]);

        if (!empty($result) && !empty($result['success'])) {
            $this->setGlobalSettingsValue(self::AUTO_UPDATE_IPV6_VALUE_NAME, $publicIp);
            return true;
        }

        return !empty($result['success']) ? $result['success'] : 'failed';
    }

}
?>