<?php  DEFINE(INDVR, true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check
if ($_POST){
	ipCamera::create();
	exit;
} else {
	$ipCamData = new ipCameras;
}

#template
include_once('../template/ajax/addip.php');
?>
