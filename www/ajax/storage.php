<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

if (empty($_POST)){
	$locations = data::query("SELECT * FROM Storage");
} else {
	data::query("DELETE FROM Storage");
	$status = true;
	foreach($_POST['path'] as $key => $path){
		$min = intval($_POST['min'][$key]);
		$max = intval($_POST['max'][$key]);
		$status = (data::query("INSERT INTO Storage VALUES ({$key}, '{$path}', {$max}, {$min})", true)) ? $status : false;
	}
	data::responseXml($status);
	exit;
}

include_once('../template/ajax/storage.php');

?>