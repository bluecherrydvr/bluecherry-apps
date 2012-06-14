<?php DEFINE('INDVR', true);

#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

$camera = device(intval($_GET['id']));


#require template to show data
include_once('../template/ajax/motionmap.php');

?>
