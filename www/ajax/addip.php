<?php

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check
if (!empty($_POST)){
	$result = ipCamera::create($_POST);
	data::responseXML($result[0], $result[1]);
	exit;
} else {
	$m = (!empty($_GET['m'])) ? $_GET['m'] : '';
	$ipCamData = new ipCameras($m);
}

if ($global_settings->data['G_DISABLE_VERSION_CHECK']==0)
	$new_list_available = $version->checkIpTablesVersion();

#template
include_once('../template/ajax/addip.php');
?>


