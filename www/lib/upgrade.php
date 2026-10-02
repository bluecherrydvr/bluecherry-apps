<?php defined('INDVR') or exit();
#upgrade mysql tables to 2.0.1

$u = data::query("SHOW COLUMNS FROM notificationSchedules LIKE 'nlimit'");

if (!$u){
	$r = data::query("ALTER TABLE notificationSchedules ADD COLUMN nlimit mediumint(8) unsigned NOT NULL DEFAULT '0'", true);
	if ($r) {  
		$r = data::query("ALTER TABLE notificationSchedules ADD COLUMN disabled tinyint(1) NOT NULL DEFAULT '0'");
	}
}

# Indexes for the storage-cleanup queries (SELECT ... WHERE archive=0 ORDER BY
# start, UPDATE ... WHERE filepath IN (...)). Without them every cleanup pass
# full-scans ~1M Media rows while holding the global DB lock, stalling all
# recording on large systems.
$u = data::query("SHOW INDEX FROM Media WHERE Key_name='archive_start'");
if (!$u){
	data::query("ALTER TABLE Media ADD KEY `archive_start` (`archive`,`start`)", true);
}

$u = data::query("SHOW INDEX FROM Media WHERE Key_name='filepath'");
if (!$u){
	data::query("ALTER TABLE Media ADD KEY `filepath` (`filepath`(191))", true);
}

?>
