<?php DEFINE('INDVR', true);

	include("lib/lib.php");  #common functions
	
	if (!empty($_SESSION['id'])){
		$current_user = new user('id', $_SESSION['id']);
		if ($current_user->info['access_setup']){
			(!isset($_GET['l'])) ? 
				include('template/main.admin.php') : include('liveview/liveview.php');
		} else {
				include('liveview/liveview.php');
		}
	} else {
				include('template/login.php');
	};
	

?>
