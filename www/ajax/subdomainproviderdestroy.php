<?php

require_once 'subdomainproviderbase.php';

class subdomainproviderdestroy extends subdomainproviderbase
{

    public function postData() {
        try {
            $response = $this->getFromApiWithToken('/actual-configuration', [], [], false,
                'DELETE');

            if (!empty($response) && !empty($response['success'])) {
                $this->setGlobalSettingsValue(self::AUTO_UPDATE_IPV4_CONFIG_NAME, 0);
                $this->setGlobalSettingsValue(self::AUTO_UPDATE_IPV6_CONFIG_NAME, 0);
            }

        } catch (\RuntimeException $exception) {
            $response = [
                'success' => false,
                'message' => $exception->getMessage()
            ];
        }

        return json_encode($response);
    }
}
?>