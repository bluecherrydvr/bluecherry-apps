<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');

if (!empty($_POST)){
	camera::updatePtzSettings($_POST['path'], $_POST['baud'], $_POST['bit'], $_POST['parity'], $_POST['stop_bit'], $_POST['protocol'], $_POST['addr'], $_POST['id']);
};

$this_camera = new camera($_GET['id']);

include_once('../template/ajax/ptzsettings.php');


?>