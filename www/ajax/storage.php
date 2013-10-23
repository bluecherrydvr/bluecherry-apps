<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

if (empty($_POST)){
	$locations = data::query("SELECT * FROM Storage");
} else {
	data::query("DELETE FROM Storage", true);
	$status = true;
	foreach($_POST['path'] as $key => $path){
		$max = intval($_POST['max'][$key]);
		$min = $max - 10;
		$status = (data::query("INSERT INTO Storage VALUES ({$key}, '{$path}', {$max}, {$min})", true)) ? $status : false;
	}
	data::responseXml($status);
	exit;
}

include_once('../template/ajax/storage.php');

?>
