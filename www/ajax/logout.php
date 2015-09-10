<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

if (isset($_SESSION['id'])) {
    $current_user = new user('id', $_SESSION['id']);
    $current_user->checkAccessPermissions('viewer');

    $current_user->doLogout();
}
$_COOKIE['currentPage'] = '';


?>
<script>window.location = '/';</script>
