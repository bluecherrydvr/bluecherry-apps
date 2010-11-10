<?php DEFINE(INDVR, true);

	include("lib/lib.php");  #common functions
	$current_user = new DVRUser();
	$current_user->CheckStatus();
	switch ($current_user->status){
		case 'admin': (!$_GET['l']) ? include('template/main.admin.php') : include('liveview/liveview.php'); break;
		case 'viewer': include('liveview/liveview.php'); break;
		case 'schedule': include('template/schedule.php'); break;
		default: include('template/login.php'); break;
	};
	

?>