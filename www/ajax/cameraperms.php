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

class userCameraPermissions{
	public $camera_list;
	public $user;
	public function __construct(){
		$id = intval($_GET['id']);
		$this->user = new user('id', $id);
		$type = ($_GET['t'] == 'PTZ') ? 'access_ptz_list' : 'access_device_list';
		$this->camera_list = $this->matchCams($this->user->info[$type]);
		
	}
	private function matchCams($access_list){
		$camera_list = data::query("SELECT id, device_name FROM Devices");
		foreach($camera_list as $key => $camera){
			$camera_list[$key]['allowed'] = (in_array($camera_list[$key]['id'], $access_list)) ? false : true;
		}
		return $camera_list;
	}

}
$p = new userCameraPermissions;

include_once('../template/ajax/cameraperms.php');

?> 