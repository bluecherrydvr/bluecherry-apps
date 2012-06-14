<?php

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');

$camera_id = (!empty($_GET['ref'])) ? intval($_GET['ref']) : false;
$camera = new ipCamera($camera_id);

$preset_id = (!empty($_GET['id'])) ? intval($_GET['id']) : 'new';
$preset = new ipPtzPreset($preset_id);

if (!empty($_POST)){
	$id = $_POST['presetId']; unset($_POST['presetId']); unset($_POST['ref']);
	$type = ($id!='new') ? 'update' : 'insert';
	$query = data::formQueryFromArray($type, 'ipPtzCommandPresets', $_POST, 'id', $id);
	$result = data::query($query, true);
	data::responseXml($result);
	exit();
} else {
	#template
	include_once('../template/ajax/ptzpresetedit.php');
};
