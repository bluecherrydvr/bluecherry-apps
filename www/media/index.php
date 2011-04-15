<?php

DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('backup');
#/auth check

print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

if (!bc_db_open()) {
	print "<error>Could not open database</error>\n";
	exit;
}

$events = bc_db_get_table("SELECT * FROM Media ORDER BY start DESC");

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

bc_db_close();
?>
