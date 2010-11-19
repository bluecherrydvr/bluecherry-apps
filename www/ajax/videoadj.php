<?php DEFINE(INDVR, true);
#lib

include("../lib/lib.php");  #common functions


#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check


$device_data = DVRData::GetObjectData('Devices', 'id', intval($_GET['id']));

include_once('../template/ajax/videoadj.php') 
?>