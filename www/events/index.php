<?php

DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user_id = empty($_SESSION['id']) ? false : $_SESSION['id'];
$current_user = new user('id', $current_user_id);
$current_user->checkAccessPermissions('backup');
#/auth check

$number_of_recs =  data::query("SELECT COUNT(*) as n From EventsCam");
$memory_limit = intval(max($number_of_recs[0]['n']/20000,1)*256).'M';
ini_set('memory_limit', $memory_limit);

$query = "SELECT EventsCam.*, Media.size AS media_size, ((Media.size>0 OR Media.end=0) AND Media.filepath!='') AS media_available ".
         "FROM EventsCam LEFT JOIN Media ON (EventsCam.media_id=Media.id) ".
	 "WHERE ";
if (isset($_GET['startDate']))
	$query .= "EventsCam.time >= ".((int)$_GET['startDate'])." AND ";
if (isset($_GET['endDate']))
	$query .= "EventsCam.time <= ".((int)$_GET['endDate'])." AND ";
if (isset($_GET['beforeId']))
	$query .= "EventsCam.id < ".((int)$_GET['beforeId'])." AND ";
if (isset($_GET['id']))
	$query .= "EventsCam.id = ".((int)$_GET['id'])." AND ";
if (empty($current_user->data[0]['access_device_list'])){
	$current_user->data[0]['access_device_list'] = '-1';
}
$query .= "EventsCam.device_id NOT IN ({$current_user->data[0]['access_device_list']}) ";
$query .= "ORDER BY EventsCam.time DESC ";
$limit = (isset($_GET['limit']) ? (int)$_GET['limit'] : 100);
if ($limit < 1)
	$limit = 15000; // #1015 - have a strict maximum to keep the server from running out of memory
if ($limit > 0)
	$query .= "LIMIT ".$limit;

$events = data::query($query);


# Output header for this feed
print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
print "<feed xmlns=\"http://www.w3.org/2005/Atom\">\n";
print "  <title type=\"text\">Bluecherry Events for " .
	$_SERVER["SERVER_NAME"] . "</title>\n";

if ($events) {
	$curevent = current($events);
	$lastupdate = date(DATE_ATOM, $curevent['time']);
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
	if (!$current_user->camPermission($item['device_id']))
		continue;

	print "  <entry>\n";
	print "    <id raw=\"".$item['id']."\">http://".$_SERVER['SERVER_NAME']."/events/?id=".$item['id']."</id>\n";
	print "    <title>" . $item['level_id'] . ": " . $item['type_id'] .
		" event on device " . $item['device_id'] . "</title>\n";
	print "    <published>" . date(DATE_ATOM, $item['time']) .
		"</published>\n";

	/* If updated exists and is empty, the event is on-going */
	if (!empty($item['length'])) {
		print "    <updated>";
		if ($item['length'] > 0) {
			print date(DATE_ATOM, $item['time'] +
				   $item['length']);
		}
		print "</updated>\n";
	}

	print "    <category scheme=\"http://www.bluecherrydvr.com/atom.html\" " .
		"term=\"" . $item['device_id'] . "/" . $item['level_id'] . "/" .
		$item['type_id'] . "\"/>\n";

	if (!empty($item['media_id']) && $item['media_available']) {
		print "    <content media_id=\"".$item['media_id']."\" media_size=\"".$item['media_size']."\">";
		print (!empty($_SERVER['HTTPS']) ? "https" : "http")."://".$_SERVER['HTTP_HOST']."/media/request.php?id=".$item['media_id'];
		print "</content>\n";
	}

	print "  </entry>\n";
}

# Close it out
print "</feed>\n";


?>
