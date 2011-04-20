<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

$id = (isset($_GET['id'])) ? intval($_GET['id']) : false;
$ipCamera = new ipCamera($id);

include_once('../template/ajax/editip.php');
?>
