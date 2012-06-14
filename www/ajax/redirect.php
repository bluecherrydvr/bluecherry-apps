<?php DEFINE('INDVR', true);


#lib
include("../lib/lib.php");  #common functions


$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('basic');

$camera = new ipCamera(intval($_GET['id']));

header('Location: http://'.$camera->info['ipAddr']);

?>