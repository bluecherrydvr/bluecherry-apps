<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

if (!empty($_POST)){
	$id = intval($_POST['id']);
	$result = data::query("UPDATE Devices SET device='{$_POST['ipAddr']}|{$_POST['port']}|{$_POST['rtsp']}', mjpeg_path='{$_POST['ipAddrMjpeg']}|{$_POST['portMjpeg']}|{$_POST['mjpeg']}', rtsp_username='{$_POST['user']}', rtsp_password='{$_POST['pass']}', debug_level=".(($_POST['debug_level']=='on') ? '1' : '0').", model='{$_POST['models']}' WHERE id={$id}", true);
	data::responseXml($result);
	exit;
}

$id = (isset($_GET['id'])) ? intval($_GET['id']) : false;
$ipCamera = new ipCamera($id);

include_once('../template/ajax/editip.php');
?>
