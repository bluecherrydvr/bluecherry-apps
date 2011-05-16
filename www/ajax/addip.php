<?php  DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check
if (!empty($_POST)){
	ipCamera::create();
	exit;
} else {
	$m = (!empty($_GET['m'])) ? $_GET['m'] : '';
	$ipCamData = new ipCameras($m);
}

#template
include_once('../template/ajax/addip.php');
?>
