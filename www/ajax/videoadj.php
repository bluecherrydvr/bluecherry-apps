<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check


$device_data = data::getObject('Devices', 'id', intval($_GET['id']));

include_once('../template/ajax/videoadj.php') 
?>
