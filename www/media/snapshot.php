<?php

DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check


$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('backup');
session_write_close();
#/auth check


function requestError($message)
{
	header('HTTP/1.1 550 Media request error');
	die('<error>'.$message.'</error>');
}

if (empty($_GET['id']))
	requestError('No ID sent');

$id = $_GET['id'];

if (!bc_db_open())
	requestError('Could not open database');

$item = bc_db_get_table("SELECT * FROM EventsCamSnapshot WHERE id=" . intval($id));
if (empty($item))
	requestError('Could not retrieve snapshot for '.$id);

$img = $item[0];

bc_db_close();

header("Content-type: " . $img['file_type']);

echo $img['file_data'];

?>
