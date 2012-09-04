<?php DEFINE('INDVR', true);


#lib
include("../lib/lib.php");  #common functions


$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('basic');

$_GET['pan'] = (!empty($_GET['pan'])) ? $_GET['pan'] : false;
$_GET['tilt'] = (!empty($_GET['tilt'])) ? $_GET['tilt'] : false;
$_GET['zoom'] = (!empty($_GET['zoom'])) ? $_GET['zoom'] : false;
$_GET['duration'] = (!empty($_GET['duration'])) ? $_GET['duration'] : false;

function print_err($msg, $extra = "")
{
	print "<response status=\"ERROR\">\n";
	print "  <error>". htmlentities($msg) ."</error>\n";
	print $extra;
	print "</response>\n";
	exit;
}

function print_query($id)
{

	$devices = data::query("SELECT * FROM Devices WHERE id=$id");
	if (!$devices)
		print_err("Error looking up device: $id");

	$dev = $devices[0];
	print "<response status=\"OK\">\n";
	if (empty($dev['ptz_control_protocol'])) {
		print "  <protocol>none</protocol>\n";
	} else {
		print "  <protocol>". $dev['ptz_control_protocol'] ."</protocol>\n";
		print "  <defaults panspeed=\"0\" tiltspeed=\"0\" duration=\"0\" />\n";
		print "  <capabilities pan=\"1\" tilt=\"1\" zoom=\"1\" />\n";
		$presets = data::query("SELECT * FROM PTZPresets WHERE ".
			"device_id=$id");
		print "  <presets>\n";
		foreach ($presets as $pset) {
			print "    <preset id=\"".$pset['preset_id']."\">";
			print htmlentities($pset['preset_name']);
			print "</preset>\n";
		}
		print "  </presets>\n";
	}
	print "</response>\n";
}

function print_move($id)
{
	$cmd = "";

	$panspeed = -1;
	$tiltspeed = -1;
	$delay = -1;

	if (!empty($_GET['pan'])) {
		if ($_GET['pan'] != "l" and $_GET['pan'] != "r")
			print_err("Invalid value for pan");
		$cmd .= $_GET['pan'];
	}

	if (!empty($_GET['zoom'])) {
		if ($_GET['zoom'] != "w" and $_GET['zoom'] != "t")
			print_err("Invalid value for zoom");
		$cmd .= $_GET['zoom'];
	}

	if (!empty($_GET['tilt'])) {
		if ($_GET['tilt'] != "u" and $_GET['tilt'] != "d")
			print_err("Invalid value for tilt");
		$cmd .= $_GET['tilt'];
	}

	if (!empty($_GET['panspeed']))
		$panspeed = intval($_GET['panspeed']);
	if (!empty($_GET['tiltspeed']))
		$tiltspeed = intval($_GET['tiltspeed']);
	if (!empty($_GET['duration']))
		$delay = intval($_GET['duration']);

	if (empty($cmd) || $_GET['command'] == 'stop') {
		$cmd = 'S';
		$delay = 0;
	}

	bc_ptz_cmd($id, $cmd, $delay, $panspeed, $tiltspeed, 0);

	print "<response status=\"OK\" />\n";
}

function print_preset($id, $cmd)
{
	if (empty($_GET['preset'])) {
		if (!empty($id) && $cmd=='save'){
			$this_device_presets = data::query("select l.preset_id + 1 as preset from PTZPresets as l left outer join PTZPresets as r on l.preset_id + 1 = r.preset_id AND (r.device_id is NULL OR r.device_id={$id}) WHERE r.preset_id is NULL limit 1");
			$preset = $this_device_presets[0]['preset'];
		} else {
			print_err("Preset is required for this command");
		}
	} else
		$preset = intval($_GET['preset']);

	$name = "";

	if ($cmd == "save" or $cmd == "map" or $cmd == "rename" or
	    $cmd == "update" or $cmd == "sync") {
		if (empty($_GET['name']))
			print_err("Name is required for save");
		$name = database::escapeString($_GET['name']);

		if ($cmd == "save" or $cmd == "sync")
			bc_ptz_cmd($id, "s", 0, 0, 0, $preset);

		if ($cmd != "sync") {
			if ($cmd == "rename" or $cmd == "update")
				data::query("DELETE FROM PTZPresets WHERE preset_id=".
					     "$preset AND device_id=$id");

			if (data::query("INSERT INTO PTZPresets (device_id,preset_id,".
				         "preset_name) VALUES ($id,$preset,".
					 "'$name')", true) == false)
				print_err("Failed to save preset");
		}
		$name = htmlentities($_GET['name']);
	} else if ($cmd == "go") {
		$pset = data::query("SELECT * FROM PTZPresets WHERE preset_id=".
					"$preset AND device_id=$id");
		if (!$pset)
			print_err("Unknown preset $preset for device $id");

		bc_ptz_cmd($id, "g", 0, 0, 0, $preset);
		$name = $pset[0]['preset_name'];
	} else if ($cmd == "clear") {
		$pset = data::query("SELECT * FROM PTZPresets WHERE preset_id=".
					"$preset AND device_id=$id");
		if (!$pset)
			print_err("Unknown preset $preset for device $id");

		bc_ptz_cmd($id, "c", 0, 0, 0, $preset);

		data::query("DELETE FROM PTZPresets WHERE preset_id=".
			     "$preset AND device_id=$id", true);
	}

	print "<response status=\"OK\">\n";
	print "  <preset id=\"$preset\">$name</preset>\n";
	print "</response>\n";
}

print "<?xml version=\"1.0\" encoding=\"UTF-8\" \x3f>\n";

if (!isset($_GET['id'])) 
	print_err("Need to supply valid device ID");
$id = intval($_GET['id']);

if (!$current_user->camPermission($id)) {
	header("HTTP/1.0 403 Forbidden");
	exit;
}

#init vars
$duration = intval($_GET['duration']);

#ipPtz/will be merged
$camera = device($id);

if ($camera->info['protocol'] == 'IP'){
	$command = ($_GET['command'] == 'stop') ? 
		'stop' : array('pan' => $_GET['pan'], 'tilt' => $_GET['tilt'], 'zoom' => $_GET['zoom']);
	$camera->ptzControl->move($command);
	if (!empty($duration) && $duration>0){
		usleep(intval($_GET['duration'])*1000); #sleep for $duration/1000 of a second and stop
		$camera->ptzControl->move('stop');
	}
	
	exit;
}

if (empty($_GET['command']))
	print_err("No command given");

switch ($_GET['command']) {
case "query": print_query($id); break;

case "stop" :
case "move" : print_move($id); break;

case "save"  :
case "go"    :
case "map"   :
case "rename":
case "update":
case "sync"  :
case "clear" : print_preset($id, $_GET['command']); break;

default: print_err("Invalid command: " . $_GET['command']); break;
}

?>

