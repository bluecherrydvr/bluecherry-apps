<?php
DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check

class userCameraPermissions{
	public $camera_list;
	public $user;
	public function __construct(){
		$id = intval($_GET['id']);
		$this->user = new DVRUser('id', $id);
		$type = ($_GET['t'] == 'PTZ') ? 'access_ptz_list' : 'access_device_list';
		$this->camera_list = $this->matchCams($this->user->data[0][$type]);
		
	}
	private function matchCams($access_list){
		$db = DVRDatabase::getInstance();
		$camera_list = $db->DBFetchAll("SELECT id, device_name FROM Devices");
		$access_list = explode(',', $access_list);
		foreach($camera_list as $key => $camera){
			$camera_list[$key]['allowed'] = (in_array($camera_list[$key]['id'], $access_list)) ? false : true;
		}
		return $camera_list;
	}

}
$p = new userCameraPermissions;

include_once('../template/ajax/cameraperms.php');

?> 