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

$presets = data::getObject('ipPtzCommandPresets');

$camera_id = (!empty($_GET['id'])) ? intval($_GET['id']) : false;

$camera = new ipCamera($camera_id);

#template
include_once('../template/ajax/ptzpresetlist.php');
?>