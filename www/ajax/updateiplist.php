<?php DEFINE('INDVR', true);

#lib

include("../lib/lib.php");  #common functions	

function updateCamList($versions){
	$file = array('ipCameras' => '', 'ipCameraDriver' => ''); //, 'ipCameraPTZDriver' => ''); ptz not supported yet
	data::query('BEGIN', true);
	foreach ($file as $table => $v){
		$file[$table] = @fopen(VAR_PATH_TO_IPCAMLIST_UPDATE.'?t='.VAR_IPCAMLIST_UPDATE_TOKEN.'&m='.$table, 'r');
		if (!$file[$table]) return array(false, AIP_CAN_NOT_GET_NEW_LIST);
		$query = "INSERT INTO $table VALUES ";
		while ($tmp = @fgets($file[$table], 1024)) {
			$tmp = explode('|', $tmp);
			$query .= ' (';
			foreach ($tmp as $key => $v){
				
				$value .= "'".trim(str_replace('\'', '', $v))."', ";
				
			}
			$value = trim($value, ' ,');
			$query .= $value;
			unset($value);
			$query .= '), ';
		}
		$query = trim($query, ' ,');
		if (!data::query('TRUNCATE TABLE '.$table, true) || !data::query($query, true)) { data::query('ROLLBACK', true); return array(false, AIP_NEW_LIST_MYSQL_E); } else { unset($query); }
		fclose($file[$table]);
	}
	data::query('COMMIT', true);
	data::query("UPDATE GlobalSettings SET value='{$versions[1]}' WHERE parameter='G_IPCAMLIST_VERSION'");
	return array(true, AIP_LIST_UPDATED_SUCCESSFULLY);
}

$x = updateCamList($version->getIpTablesVersion());

data::responseXml($x[0], $x[1]);




?>
