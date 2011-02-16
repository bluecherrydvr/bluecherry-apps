<?php defined('INDVR') or exit(); 

#lib

Class liveViewDevices{
	public $devices;
	public $access_list;
	public $layout_list;
	public $layout_values;
	public function __construct(){
		$db = DVRDatabase::getInstance();
		$tmp = $db->DBFetchAll("SELECT access_device_list FROM Users WHERE id='{$_SESSION['id']}'");
		$this->layout_list = $db->DBFetchAll("SELECT layout_name FROM userLayouts WHERE user_id='{$_SESSION['id']}'");
		$this->access_list = explode(',', $tmp[0]['access_device_list']);
		$this->devices = $db->DBFetchAll("SELECT Devices.id, Devices.device_name  FROM Devices INNER JOIN AvailableSources USING (device) WHERE Devices.disabled='0' UNION SELECT Devices.id, Devices.device_name  FROM Devices WHERE protocol='IP' AND disabled='0'");
		if (!empty($_SESSION['load_layout'])) $this->loadLayout();
	}
	private function loadLayout(){
		$this->deleteLayoutCookies(); #unset all layout cookies
		$cookies = explode(';', $_SESSION['load_layout']);
		foreach ($cookies as $key => $cookie){
			$cookie = explode(':', $cookie);
			setcookie($cookie[0], $cookie[1]);
		}
		unset($_SESSION['load_layout']);
	}
	private function deleteLayoutCookies(){
		foreach($_COOKIE as $key => $value){
			if (preg_match("/imgSrcy[0-9]*x[0-9]*|lvRows|lvCols/", $key)) setcookie($key, '', 5);
		}
	}
}

$lv = new liveViewDevices;

#require template to show data
include_once('template/main.viewer.php');

?>
