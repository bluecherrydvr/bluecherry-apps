<?php DEFINE('INDVR', true);

require_once 'vendor/autoload.php';

$site_path = __DIR__;

define ('site_path', $site_path . '/');
define ('lib_path', site_path . 'lib/');
define ('ctl_path', site_path . 'ajax/');
define ('template_path', site_path . 'template/');

//include("lib/lib.php");  #common functions
include_once lib_path."Route.php";

$route = new Route();
$route->start();
	
	//$from_client = (empty($_SESSION['from_client'])) ? false : $_SESSION['from_client'];
	//$from_client_override = (empty($_SESSION['from_client_override'])) ? false : $_SESSION['from_client_override'];
	
	//if (empty($_SESSION['id']) || ($from_client && !$from_client_override)){
		//include('template/login.php');
	//} else {
		//$current_user = new user('id', $_SESSION['id']);
		//#check for general errors
		//$default_password_warning_dismiss = (!empty($_COOKIE['default_password_warning_dismiss'])) ? $_COOKIE['default_password_warning_dismiss'] : false;
		//if ($current_user->info['default_password'] && !empty($_COOKIE['default_password_warning_dismiss'])) {
			//$_GLOBALS['general_error'] = array('type' => 'INFO', 'text' => WARN_DEFAULT_PASSWORD);
		//}

		//$status = data::getObject('ServerStatus');
		//if (!empty($status[0]['message'])) {
			//$m = preg_split( '/\r\n|\r|\n/', $status[0]['message']);
			//$_GLOBALS['general_error'] = array('type' => 'F', 'text' => $m[0]);
		//}
		//#choose which template to display, if no setup access for current_user, display live view automatically
		//if ($current_user->info['access_setup']){
			//(!isset($_GET['l'])) ? 
				//include('template/main.admin.php') : include('liveview/liveview.php');
		//} else {
				//include('liveview/liveview.php');
		//}		
	//};
	

?>
