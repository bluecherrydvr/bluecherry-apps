<?php

$db = bc_db_open() or die("Could not open database\n");

$events = bc_db_get_table($db, "SELECT * FROM Media ORDER BY start DESC");

# Output header for this feed
print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

# Output one item for each event
foreach ($events as $item) {
	print "  <item>\n";

	foreach (array_keys($item) as $key)
		print "    <$key>". $item[$key] ."</$key>\n";

	print "  </item>\n";
}

bc_db_close($db);
?>
