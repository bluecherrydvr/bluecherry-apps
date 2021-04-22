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
	
	if(!preg_match("/^[a-zA-Z0-9]{4}\-[a-zA-Z0-9]{4}\-[a-zA-Z0-9]{4}\-[a-zA-Z0-9]{4}$/", $key))
		return false;

	exec("/usr/lib/bluecherry/licensecmd bc_license_check ".$key, $output, $ret);

	if ($ret != 0)
		return false;

	return $output[0];
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
	$output = bc_v3license_check(LA_GET_LICENSE_METADATA);
	if (is_null($output) || (int)$output[1] != Constant('LA_OK'))
		return 0;

	return (int)$output[2];
}

function bc_v3license_check($command, $key = "")
{
	if(!($sock = socket_create(AF_INET, SOCK_STREAM, 0)))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Couldn't create socket: [$errorcode] $errormsg \n");
		return null;
	}

	// Connect socket to remote server
	if (!socket_connect($sock, '127.0.0.1', 7003))
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
