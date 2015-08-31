<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

if (empty($_POST)){
	$locations = data::query("SELECT * FROM Storage");
    foreach(array_keys($locations) as $id) {
        $path = database::escapeString($locations[$id]['path']);
        $fspace = disk_free_space($path) >> 20;
        $query = "SELECT ({$fspace}) * period / size + period AS result FROM (".
              "SELECT SUM(CASE WHEN end < start THEN 0 ELSE end - start END".
            ") period, SUM(size) DIV 1048576 size".
            " FROM Media WHERE filepath LIKE '{$path}%') q";
		$result = data::query($query);
		$locations[$id]['record_time'] = $result[0]['result'];
    }
} else {
	data::query("DELETE FROM Storage", true);
	$status = true;
	foreach($_POST['path'] as $key => $path){
		$max = intval($_POST['max'][$key]);
		$min = $max - 10;
		$status = (data::query("INSERT INTO Storage (priority, path, max_thresh, min_thresh) VALUES ({$key}, '{$path}', {$max}, {$min})", true)) ? $status : false;
	}
	data::responseJSON($status);
	exit;
}

include_once('../template/ajax/storage.php');

?>
