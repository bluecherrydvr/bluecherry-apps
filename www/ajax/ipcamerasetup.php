<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

$id = intval($_GET['id']);

$camera = new ipCamera($id);

include_once('../template/ajax/ipcamerasetup.php');

php?>