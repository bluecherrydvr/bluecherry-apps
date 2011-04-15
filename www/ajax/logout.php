<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('viewer');

$current_user->doLogout();
$_COOKIE['currentPage'] = '';


?>
<script>window.location = '/';</script>