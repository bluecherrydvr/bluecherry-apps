<?php

DEFINE(INDVR, true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('viewer');
#/auth check

print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

$db = bc_db_open();
if (!$db) {
	print "<error>Could not open database</error>\n";
	exit;
}

$events = bc_db_get_table($db, "SELECT * FROM Media ORDER BY start DESC");

# Output one item for each media entry
foreach ($events as $item) {
	print "  <item id='". $item['id'] ."'>\n";

	foreach (array_keys($item) as $key) {
		if ($key == 'id')
			continue;
		print "    <$key>". $item[$key] ."</$key>\n";
	}

	print "  </item>\n";
}

bc_db_close($db);
?>
