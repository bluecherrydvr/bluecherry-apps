<?php DEFINE('INDVR', true);

#lib
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');

function directory_staus($path, $type){ #path -- path to storage dir, type -- new or existing, existing are not checked for existing files
	#if file 
	if (!file_exists($path)){
		return array('F', str_replace('%PATH%', $path, DIR_DOES_NOT_EXIST_OR_NOT_READABLE));
	}
	if (!is_readable($path) || !is_writable($path)){
		return array('F', str_replace('%PATH%', $path, DIR_NOT_READABLE));
	}
	if ($type == 'new'){
		return (count(scandir($path)) == 2) ? array('OK', DIR_OK) : array('INFO', str_replace('%PATH%', $path, DIR_NOT_EMPTY));
	}
	
}

$check_dir = directory_staus($_GET['path'], 'new');

data::responseXml($check_dir[0], $check_dir[1]);

?>
