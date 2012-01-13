<?php DEFINE('INDVR', true); 
/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

#lib

include("../lib/lib.php");  #common functions

init();

$id = (!empty($_GET['id'])) ? intval($_GET['id']) : false;

$camera = new ipCamera($id);

$camera->checkConnection();

foreach($camera->info['connection_status'] as $type => $status){
	if ($status!='OK'){
		$status_message .= str_replace('%TYPE%', $type, constant('IP_ACCESS_STATUS_'.$status)).'<br /><br />';
	}
};

echo $status_message;

?>
