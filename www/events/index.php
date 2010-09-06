<?php

$db = bc_db_open() or die("Could not open database\n");

$events = bc_db_get_table($db, "SELECT * FROM EventsCam ORDER BY time DESC LIMIT 100");

# Output header for this feed
print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
print "<feed xmlns=\"http://www.w3.org/2005/Atom\">\n";
print "  <title type=\"text\">Bluecherry Events for " .
	$_SERVER["SERVER_NAME"] . "</title>\n";

if ($events) {
	$curevent = current($events);
	$lastupdate = date(DATE_ATOM, strtotime($curevent['time']));
} else {
	$lastupdate = date(DATE_ATOM);
}

print "  <updated>" . $lastupdate . "</updated>\n";

# XXX Need to generate this per-server
print "  <id>urn:uuid:60a76c80-d399-11d9-b93C-0003939e0af6</id>\n";

print "  <link rel=\"self\" type=\"application/atom+xml\" " .
	"href=\"http://" . $_SERVER["SERVER_NAME"] . "/events/\"/>\n";
print "  <generator uri=\"http://www.bluecherrydvr.com/atom.html\" version=\"1.0\">\n";
print "    BluecherryDVR Events Atom Generator\n";
print "  </generator>\n";

# Output one item for each event
foreach ($events as $item) {
	print "  <entry>\n";
	print "    <title>" . $item['level_id'] . ": " . $item['type_id'] .
		" event on device " . $item['device_id'] . "</title>\n";
	print "    <published>" . date(DATE_ATOM, strtotime($item['time'])) .
		"</published>\n";

	/* If updated exists and is empty, the event is on-going */
	if (!empty($item['length'])) {
		print "    <updated>";
		if ($item['length'] > 0) {
			print date(DATE_ATOM, strtotime($item['time']) +
				   $item['length']);
		}
		print "</updated>\n";
	}

	print "    <category scheme=\"http://www.bluecherrydvr.com/atom.html\" " .
		"term=\"" . $item['device_id'] . "/" . $item['level_id'] . "/" .
		$item['type_id'] . "\"/>\n";
	print "  </entry>\n";
}

# Close it out
print "</feed>\n";

bc_db_close($db);
?>
