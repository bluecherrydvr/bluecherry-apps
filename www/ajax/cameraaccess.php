<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check
	
$user_data = new DVRUser('id', intval($_GET['id']));

?>
