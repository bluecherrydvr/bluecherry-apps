<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');

if (!empty($_POST)){
	camera::updatePtzSettings($_POST['path'], $_POST['baud'], $_POST['bit'], $_POST['parity'], $_POST['stop_bit'], $_POST['protocol'], $_POST['addr'], $_POST['id']);
} else {
	$this_camera = new camera($_GET['id']);
    if (!isset($this_camera->info['ptz_config'])) {
        $this_camera->info['ptz_config']['protocol'] = '';
        $this_camera->info['ptz_config']['path'] = '';
        $this_camera->info['ptz_config']['addr'] = '';
        $this_camera->info['ptz_config']['baud'] = '';
        $this_camera->info['ptz_config']['bit'] = '';
        $this_camera->info['ptz_config']['stop_bit'] = '';
        $this_camera->info['ptz_config']['parity'] = '';
    }

	include_once('../template/ajax/ptzsettings.php');
};

?>
