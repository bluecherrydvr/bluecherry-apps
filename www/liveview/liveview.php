<?php defined('INDVR') or exit(); 

#lib

Class LiveViewDevices{
	public $devices;
	
	public function __construct(){
		$db = DVRDatabase::getInstance();
		$this->devices = $db->DBFetchAll($db->DBQuery("SELECT * FROM Devices WHERE disabled='0'"));
	}
}

$lv = new LiveViewDevices;

#require template to show data
include_once('template/main.viewer.php');

?>