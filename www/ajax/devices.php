<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check

	
$devices = new DVRDevices;
if (isset($_GET['XML']))
	$devices->MakeXML();
else
	include_once('../template/ajax/devices.php');

?>

