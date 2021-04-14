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
	$output = array();
	$ret = 0;
	exec("/usr/lib/bluecherry/licensecmd bc_license_devices_allowed", $output, $ret);

	if ($ret != 0)
		return false;

	return $output[0];
}

function bc_v3license_check()
{
	$output = array();
	if(!($sock = socket_create(AF_INET, SOCK_STREAM, 0)))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Couldn't create socket: [$errorcode] $errormsg \n");
	}

	echo "Socket created \n";

	//Connect socket to remote server
	if(!socket_connect($sock , '127.0.0.1' , 7003))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Could not connect: [$errorcode] $errormsg \n");
	}

	echo "Connection established \n";

	$message = "GET / HTTP/1.1\r\n\r\n";

	//Send the message to the server
	if( ! socket_send ( $sock , $message , strlen($message) , 0))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Could not send data: [$errorcode] $errormsg \n");
	}

	echo "Message send successfully \n";

	//Now receive reply from server
	if(socket_recv ( $sock , $buf , 2045 , MSG_WAITALL ) === FALSE)
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Could not receive data: [$errorcode] $errormsg \n");
	}

	//print the received message
	echo $buf;
	//exec("/usr/lib/bluecherry/licensecmd bc_v3license_check", $output, $ret);
	//$username = posix_getpwuid(posix_geteuid())['name'];
	//echo ($username);
	//echo shell_exec("whoami");

	echo ($ret);
	echo ($output[0]);

	if ($ret != 0)
		return false;

	return $output[0];

}

?>
