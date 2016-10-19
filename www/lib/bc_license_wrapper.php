<?php

function bc_license_machine_id()
{
	$output = array();
	$ret = 0;
	exec("/usr/lib/bluecherry/licensecmd bc_license_machine_id", $output, $ret);

	if ($ret != 0)
		return false;

	return $output[0];
}

function bc_license_check($key)
{
	$output = array();
	$ret = 0;

	exec("/usr/lib/bluecherry/licensecmd bc_license_check ".$key, $output, $ret);

	if ($ret != 0)
		return false;

	return $output[0];
}

function bc_license_check_auth($key, $auth)
{
	$output = array();
	$ret = 0;

	exec("/usr/lib/bluecherry/licensecmd bc_license_check_auth ".$key." ".$auth, $output, $ret);

	if ($ret != 0)
		return false;

	return $output[0];
}

function bc_license_devices_allowed()
{
	$output = array();
	$ret = 0;
	exec("/usr/lib/bluecherry/licensecmd bc_license_devices_allowed", $output, $ret);

	if ($ret != 0)
		return false;

	return $output[0];
}

?>
