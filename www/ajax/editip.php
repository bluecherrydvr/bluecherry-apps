<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

if (!empty($_POST)){
	$id = intval($_POST['id']);
	$data['audio_disabled'] = ($_POST['audio_enabled']=='on') ? 0 : 1;
	$data['device'] = "{$_POST['ipAddr']}|{$_POST['port']}|{$_POST['rtsp']}";
	$data['mjpeg_path'] = "{$_POST['ipAddrMjpeg']}|{$_POST['portMjpeg']}|{$_POST['mjpeg']}";
	$data['rtsp_username'] = empty($_POST['user']) ?  '' : $_POST['user'];
	$data['rtsp_password'] = empty($_POST['pass']) ?  '' : $_POST['pass'];
	$data['debug_level'] = ($_POST['debug_level']=='on') ? '1' : '0';
	$data['model'] = $_POST['models'];
	$query = data::formQueryFromArray('update', 'Devices', $data, 'id', $id);
	$result = data::query($query, true);
	data::responseXml($result);
	exit();
}

$id = (isset($_GET['id'])) ? intval($_GET['id']) : false;
$ipCamera = new ipCamera($id);

include_once('../template/ajax/editip.php');
?>
