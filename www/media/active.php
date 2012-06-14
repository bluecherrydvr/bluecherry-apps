<?php DEFINE('INDVR', true);

#lib
include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('basic');

// Checks for in-progress event
function check_active($id)
{
	$list = bc_db_get_table("SELECT id FROM EventsCam WHERE device_id=$id ".
				"AND length=-1");
	if (empty($list))
		return;

	print "  <device id=\"$id\" />\n";
}

if (!isset($_GET['id'])) {
	print "No device ID(s) supplied\n";
	exit;
}

$ids = explode(',', $_GET['id']);

print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
print "<activedevices>\n";
foreach ($ids as $id) {
	$id = intval($id);
	if ($current_user->camPermission($id))
		check_active($id);
}
print "</activedevices>\n";

bc_db_close();

?>
