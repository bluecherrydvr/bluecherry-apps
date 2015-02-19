<?php

# Auth
DEFINE('INDVR', true);
include("../lib/lib.php");
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('basic');

# Parameters validation
if (!isset($_GET['camera_id']) || !is_numeric($_GET['camera_id'])) {
	header('HTTP/ 400 Bad Request - numeric parameter camera_id is required');
	die();
}

# Form the message
$msg = $_GET['camera_id'] . ' ' . $_GET['description'];

# Send via unix socket connection
$socket = fsockopen('unix:///tmp/bluecherry_trigger', -1, $errno, $errstr);
if (!$socket) {
	header("HTTP/ 500 Server Error - failed to connect to local socket, reason $errstr ($errno)");
	die();
}
fwrite($socket, $msg);
$ret_str = fgets($socket);
fclose($socket);

# Form HTTP reply
header('HTTP/ ' . $ret_str);
?>
