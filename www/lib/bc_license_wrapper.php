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

function bc_license_activate_key($key)
{
	$output = bc_license_command(LA_ACTIVATE_LICENSE, $key);

	return $output;
}

function bc_license_deactivate_key()
{
	$output = bc_license_command(LA_DEACTIVATE_LICENSE);

	return $output;
}

function bc_license_check_auth($key, $auth)
{
	$output = array();
	$ret = 0;
	
	if(!preg_match("/^[a-zA-Z0-9]{4}\-[a-zA-Z0-9]{4}\-[a-zA-Z0-9]{4}\-[a-zA-Z0-9]{4}$/", $key))
		return false;
	
	if(!preg_match("/^[a-zA-Z0-9]{4}\-[a-zA-Z0-9]{4}$/", $auth))
		return false;

	exec("/usr/lib/bluecherry/licensecmd bc_license_check_auth ".$key." ".$auth, $output, $ret);

	if ($ret != 0)
		return false;

	return $output[0];
}

function bc_license_devices_allowed()
{
	// Always return unlimited devices (-1) for free and open source version
	return -1;
}

function bc_license_check_genuine()
{
	$ret = array();

	// Check if the license is genuine
	$output = bc_license_command(LA_IS_LICENSE_GENUINE);
	if (is_null($output)) {
		$ret[0] = false;
		$ret[1] = L_LA_E_BC_SERVER;
		return $ret;
	}

	// If the command is OK
	if ((int)$output[1] == Constant('LA_OK')) {
		$output = bc_license_genuine_expirydate();
		if (is_null($output)) {
			$ret[0] = false;
			$ret[1] = L_LA_E_BC_SERVER;
		}
		else if ((int)$output[1] == Constant('LA_OK')) {
			$ret[0] = true;
			$left_days = (int)$output[3];

			if ((int)$output[2] == 1) { // unlimited
				$ret[1] = L_LA_E_LICENSE_ACTIVATED;
			}
			else {
				$ret[1] = L_LA_E_LICENSE_ACTIVATED . " Days left: " . strval($left_days);
			}
		}
		else {
			$ret[0] = false;
			$ret[1] = licenses::getLicenseStatusMessage((int)$output[1]);
		}

		return $ret;
	}

	// If the genuine is expired or isn't activated
	if ((int)$output[1] == Constant('LA_EXPIRED')) {
		$ret[0] = false;
		$ret[1] = L_LA_EXPIRED;
	}
	else if ((int)$output[1] == Constant('LA_SUSPENDED')) {
		$ret[0] = false;
		$ret[1] = L_LA_SUSPENDED;
	}
	else if ((int)$output[1] == Constant('LA_GRACE_PERIOD_OVER')) {
		$ret[0] = false;
		$ret[1] = L_LA_GRACE_PERIOD_OVER;
	}
	else {
		$ret = bc_license_check_trial();
		$ret[0] = false;
	}

	return $ret;
}

function bc_license_genuine_expirydate()
{
	$output = bc_license_command(LA_GET_LICENSE_EXPIRYDATE);

	return $output;
}

function bc_license_check_trial()
{
	$ret = array();

	// Check if the license is trial genuine
	$output = bc_license_command(LA_IS_TRIAL_GENUINE);
	if (is_null($output)) {
		$ret[0] = false;
		$ret[1] = L_LA_E_BC_SERVER;
		return $ret;
	}

	// If the command is OK
	if ((int)$output[1] == Constant('LA_OK')) {
		$left_days = bc_license_trial_expirydate();

		$ret[0] = true;
		$ret[1] = L_LA_E_TRIAL_ACTIVATED . strval($left_days);
		return $ret;
	}

	// If the trial is expired or isn't activated
	if ((int)$output[1] == Constant('LA_TRIAL_EXPIRED')) {
		$ret[0] = false;
		$ret[1] = L_LA_TRIAL_EXPIRED;
	}
	else {
		$ret[0] = false;
		$ret[1] = L_LA_E_TRIAL_ACTIVATE_REQUIRE;
	}

	return $ret;
}

function bc_license_activate_trial()
{
	$output = bc_license_command(LA_ACTIVATE_TRIAL);

	return $output;
}

function bc_license_trial_expirydate()
{
	$output = bc_license_command(LA_GET_TRIAL_EXPIRYDATE);
	if (is_null($output) || (int)$output[1] != Constant('LA_OK'))
		return 0;

	return (int)$output[2];
}

function bc_license_command($command, $key = "")
{
	if(!($sock = socket_create(AF_INET, SOCK_STREAM, 0)))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Couldn't create socket: [$errorcode] $errormsg \n");
		return null;
	}

	// Connect socket to bc-server
	if (!socket_connect($sock, '127.0.0.1', 7004))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Could not connect: [$errorcode] $errormsg \n");
		return null;
	}

	// Make a message
	$message = $command . " " . $key;

	// Send the message to the server
	if (!socket_send($sock, $message, strlen($message), 0))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Could not send data: [$errorcode] $errormsg \n");
		return null;
	}

	// Now receive reply from server
	if (socket_recv($sock, $buf, 2045, MSG_WAITALL) === FALSE)
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Could not receive data: [$errorcode] $errormsg \n");
		return null;
	}

	// Close socket
	socket_close($sock);

	// Split the reply
	$output = explode(" ", $buf);

	// Compare command
	if ($output[0] != $command)
		return null;

	return $output;

}

?>
