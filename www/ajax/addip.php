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
	data::responseJSON($result[0], $result[1]);
	exit;
}
	
#template
include_once('../template/ajax/addip.php');
?>


