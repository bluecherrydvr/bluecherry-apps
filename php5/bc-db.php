<?php

$bcdb = bc_db_open();

if (!$bcdb) {
	echo "Could not open database";
	exit(1);
}

$rows = bc_db_get_table($bcdb, "SELECT * from Devices");

var_dump($rows);

if (!bc_db_close($bcdb))
	echo "Failed closing the database";

?>
