<?php defined('INDVR') or exit(); 

#lib

Class LiveViewDevices{
	public $devices;
	 
	public function __construct(){
		$db = DVRDatabase::getInstance();
		$this->devices = $db->DBFetchAll($db->DBQuery("SELECT * FROM Devices LEFT OUTER JOIN AvailableSources ON Devices.source_video=AvailableSources.devicepath WHERE Devices.disabled='0' "));
	}
}

$lv = new LiveViewDevices;

#require template to show data
include_once('template/main.viewer.php');

?>