<?php DEFINE('INDVR', true);

	include("lib/lib.php");  #common functions
	
	$from_client = (empty($_SESSION['from_client'])) ? false : $_SESSION['from_client'];
	$from_client_override = (empty($_SESSION['from_client_override'])) ? false : $_SESSION['from_client_override'];
	
	if (empty($_SESSION['id']) || ($from_client && !$from_client_override)){
		include('template/login.php');
	} else {
		$current_user = new user('id', $_SESSION['id']);
		#check for general errors
		if ($current_user->info['default_password'] && !$_COOKIE['default_password_warning_dismiss']) {
			$_GLOBALS['general_error'] = array('type' => 'INFO', 'text' => WARN_DEFAULT_PASSWORD);
		}
		#choose which template to display, if no setup access for current_user, display live view automatically
		if ($current_user->info['access_setup']){
			(!isset($_GET['l'])) ? 
				include('template/main.admin.php') : include('liveview/liveview.php');
		} else {
				include('liveview/liveview.php');
		}		
	};
	

?>
