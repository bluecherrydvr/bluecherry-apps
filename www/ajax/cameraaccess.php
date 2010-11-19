<?php DEFINE(INDVR, true);
#lib
include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check
	
$user_data = new DVRUser('id', intval($_GET['id']));

?>