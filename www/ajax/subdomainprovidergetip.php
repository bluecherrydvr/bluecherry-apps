<?php

require_once 'subdomainproviderbase.php';

class subdomainprovidergetip extends subdomainproviderbase
{

    public function getData() {
        return json_encode($this->getServerPublicIp(boolval($_GET['ipv6'])));
    }

    public function postData() {
        return json_encode($this->getServerPublicIp(boolval($_GET['ipv6'])));
    }
}
?>