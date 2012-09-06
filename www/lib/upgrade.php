<?php defined('INDVR') or exit();
#upgrade mysql tables to 2.0.1

$u = data::query("SHOW COLUMNS FROM notificationSchedules LIKE 'nlimit'");

if (!$u){
	$r = data::query("ALTER TABLE notificationSchedules ADD COLUMN nlimit mediumint(8) unsigned NOT NULL DEFAULT '0'", true);
	if ($r) {  
		$r = data::query("ALTER TABLE notificationSchedules ADD COLUMN disabled tinyint(1) NOT NULL DEFAULT '0'");
	}
}

?>
