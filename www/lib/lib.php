<?php 

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */
 
defined('INDVR') or exit();

session_name('VAR_SESSION_NAME');
session_start();

#include vars and language
	include("lang.php"); #language
	include("var.php");  #config vars

	#debug
	function var_dump_pre($mixed = null) {
	  echo '<hr><pre>';
 	  var_dump($mixed);
	  echo '</pre>';
	  return null;
	}
	
	
	
#classes

class DVRDatabase {
	public static $instance;

	private function __construct(){
		$this->DBConnect();
	}

	public static function getInstance() {
		self::$instance or self::$instance = new DVRDatabase();
		return self::$instance;
	}

	private function DBConnect() {
		bc_db_open() or die(LANG_DIE_COULDNOTCONNECT);
	}

	public function DBEscapeString(&$string) {
		$string = bc_db_escape_string($string);
		return $string;
	}

	/* Execute a result-less query */
	public function DBQuery($query) {
		return bc_db_query($query);
	}

	/* Execute a query that will return results */
	public function DBFetchAll($query) {
		return bc_db_get_table($query);
	}
}

class DVRVersion{
	public $up_to_date;
	public $current_version;
	public $installed_version;
	
	public function __construct(){
		$this->current_version = trim(@file_get_contents(VAR_PATH_TO_CURRENT_VERSION));
		$this->installed_version = trim(@file_get_contents(VAR_PATH_TO_INSTALLED_VERSION));
		$this->up_to_date = ($this->current_version == $this->installed_version) ? true : false;
	}
}

#class to load info/perms of the current user
class DVRUser extends DVRData{
	public $status;
	public $data;
	function __construct($parameter = false, $value = false){
		if ($parameter) { $this->data = $this->GetObjectData('Users', $parameter, $value); };
	}
	public function CheckStatus(){
		if (!empty($_SESSION['l'])){
			$kicked = $this->ActiveUsersUpdate();
			$this->data = $this->GetObjectData('Users', 'id', $_SESSION['id']);
			switch ($_SESSION['l']) {
				case 'admin':  $this->status = 'admin';  break;
				case 'viewer': $this->status = ($this->ScheduleCheck()) ? 'viewer' : 'schedule'; break;
				default: $this->status = 'new';
			};
			if ($kicked){ $this->status = 'kicked'; };
		}
	}
	
	public function StatusAction($l){
			$message = false;
			switch($l){
				case 'admin' :
					if ($this->status!='admin') $message = JS_RELOAD.USER_NACCESS;
				break;
				case 'viewer':
					if ($this->status!='admin' && $this->status!='viewer') $message = JS_RELOAD.USER_NACCESS;
					if ($this->status=='schedule') $message = DSCED_MSG;
				break;
				case 'mjpeg' :
					$this->CheckStatus();
					if ($this->status != 'admin' && $this->status != 'viewer') {
						if (!isset($_SERVER['PHP_AUTH_USER'])) {
							    header('WWW-Authenticate: Basic realm="media"');
    							header('HTTP/1.0 401 Unauthorized');
								exit;
						} else {
							if (!$this->BasicAuthCheck()) $message = LOGIN_WRONG;
						}
						
					}
					return true;
				break;
				case 'devices':
					if ($this->status=='admin' || ($_SESSION['from_client']==true && $this->status=='viewer')){
						return true;
					} elseif (!isset($_SERVER['PHP_AUTH_USER'])) {
						    header('WWW-Authenticate: Basic realm="devices"');
    						header('HTTP/1.0 401 Unauthorized');
							exit;
					};
					if(isset($_GET['short']) && isset($_SERVER['PHP_AUTH_USER'])) {
						if (!$this->BasicAuthCheck()) $message = LOGIN_WRONG;
					} else {
						$message = JS_RELOAD.USER_NACCESS;
					}
				break; 
			}
			switch ($this->status){
				case 'new': $message = USER_RELOGIN; break;
				case 'kicked': session_unset(); $_SESSION['message'] = $message = USER_KICKED; $message .= JS_RELOAD; break;
			};
			
			if ($message) die("<div class='INFO' id='message'>{$message}</div>");
	}
	
	public function camPermission($id){
		$this->data = $this->GetObjectData('Users', 'id', $_SESSION['id']);
		$access_list = explode(',', $this->data[0]['access_device_list']);
		return (in_array($id, $access_list)) ? false : true;
	}
	
	private function BasicAuthCheck(){
		$this->data = $this->GetObjectData('Users', 'username', $_SERVER['PHP_AUTH_USER']);
		
		return (!$this->ValidatePassword($_SERVER['PHP_AUTH_PW'])) ? false : true;
	}
	public function ValidatePassword($entered_password){
		return (md5($entered_password.$this->data[0]['salt'])===$this->data[0]['password']) ? true : false;
	}
	private function ActiveUsersUpdate(){
		$db = DVRDatabase::getInstance(); 
		$db->DBQuery("DELETE FROM ActiveUsers WHERE time <".(time()-300));
		$tmp = $db->DBFetchAll("SELECT * FROM ActiveUsers WHERE ip = '{$_SERVER['REMOTE_ADDR']}'");
		if (count($tmp) == 0){ 
			 $db->DBQuery("INSERT INTO ActiveUsers VALUES ({$_SESSION['id']}, '{$_SERVER['REMOTE_ADDR']}', '{$_SESSION['from_client']}', ".time().", 0)");
		} else {
			if ($tmp[0]['kick']) return true;
			$db->DBQuery("UPDATE ActiveUsers SET time = ".time()." WHERE ip = '{$_SERVER['REMOTE_ADDR']}'");
		}
		return false;
	}
	private function ScheduleCheck(){
		return (substr($this->data['schedule'], date('N')*(date('G')+1), 1)=='1')  ? true : false;
	}
	
	public function ban($v = 0){
		$db = DVRDatabase::getInstance();
		$db->DBQuery("UPDATE Users SET access_setup = $v, access_remote = $v, access_web = $v, access_backup = $v, access_relay = $v WHERE id={$this->data[0]['id']}");
		
	}
}

#unified class for data operations
class DVRData {
	#mode: global - global config (no insertions, update multiple entries) OR update / insert
	public function Edit($mode){
		return $this->FormQueryFromPOST($mode);
	}
	
	public function Kill($type, $id){
		$db = DVRDatabase::getInstance();
		$db->DBQuery("DELETE FROM $type WHERE id=".$id);
	}
	
	#forms SQL query from POST array
	public function FormQueryFromPOST($mode){
		#separate id/table(type) that are not to be inserted
		$db = DVRDatabase::getInstance();
		$id = $db->DBEscapeString($_POST['id']); unset($_POST['id']);
		$type = $db->DBEscapeString($_POST['type']); unset($_POST['type']);
		$ret = true;
		switch ($mode){
			case 'global':
				foreach ($_POST as $parameter => $value) {
					$db->DBEscapeString($parameter);
					$db->DBEscapeString($value);
					if (!$db->DBQuery("UPDATE GlobalSettings SET value = '$value' WHERE parameter='$parameter'")) { $ret = false; break; }
					
				}
				break;
			case 'insert':
				$ret = $db->DBQuery("INSERT INTO $type (".implode(", ", array_keys($_POST)).") VALUES ('".implode("', '", $_POST)."')");
				break;
			case 'update':
				foreach ($_POST as $parameter => $value){
					if ($value=='true') {$value = '1'; } elseif ($value=='false') {$value = '0'; };
					$tmp[$parameter]="{$db->DBEscapeString($parameter)}='{$db->DBEscapeString($value)}'";
				}
				
				$ret = $db->DBQuery("UPDATE $type SET ".implode(", ", $tmp)." WHERE id='$id'");
				break;
		}
		return $ret;
	}
	
	public static function GetObjectData($type, $parameter = false, $value = false, $condition = '='){
		$db = DVRDatabase::getInstance();
		$tmp = $db->DBFetchAll("SELECT * FROM {$db->DBEscapeString($type)}".((!$parameter) ? '' : " WHERE {$db->DBEscapeString($parameter)} ".$condition." '{$db->DBEscapeString($value)}'"));
		return (!$tmp) ? false : $tmp;
	}
	
	
}

class DVRDevices extends DVRData{
	public $number_of_card;
	public $total_devices;
	public $cards;
	public $ip_cameras;
	public function __construct(){
		$id = (isset($_GET['id'])) ? intval($_GET['id']) : false;
		$this->getCards();
		$this->getIpCameras();
	}

	private function getCards(){
		$db = DVRDatabase::getInstance();
		$tmp = $db->DBFetchAll("SELECT * FROM AvailableSources GROUP BY card_id");
		$this->number_of_cards = count($tmp);
		if (!count($tmp)) { return false; };
		foreach ($tmp as $key => $card){
			$this->cards[$card['card_id']] = new BCDVRCard($card['card_id']);
			$this->total_devices += count($this->cards[$card['card_id']]->devices);
		}
	}
	private function getIpCameras(){
		$db = DVRDatabase::getInstance();
		$this->ip_cameras = $db->DBFetchAll("SELECT * FROM Devices WHERE protocol='IP'");
		foreach ($this->ip_cameras as $key => $device){
			$this->ip_cameras[$key]['status'] = (!$device['disabled']) ? 'OK' : 'disabled';
		}
		$this->total_devices += count($this->ip_cameras);
	}
	public function MakeXML() {
		$this_user = new DVRUser('id', $_SESSION['id']);
		$access_list = explode(',', $this_user->data[0]['access_device_list']);
		// The \x3f is a '?'. Used hex so as not to confuse vim
		$xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" \x3f>\n<devices>\n";
		$devices = array();
		if (!empty($this->cards))
		foreach($this->cards as $card_id => $card){
			$devices = array_merge($devices, $card->devices);
		};
		$devices = array_merge($devices, $this->ip_cameras);
		foreach ($devices as $device) {
			if (in_array($device['id'], $access_list))
				continue;

			if (empty($device['id']))
				$xml .= "  <device>\n";
			else
				$xml .= "  <device id=\"" . $device['id'] . "\">\n";

			foreach($device as $property => $value) {
				if ($property == 'rtsp_password' ||
				    $property == 'rtsp_username')
					continue;
				if (isset($_GET['short']) && ($property != 'protocol'
				    && $property != 'device_name' &&
				    $property != 'resolutionX' &&
				    $property != 'resolutionY'))
					continue;
				$xml .= "    <$property>";
				$xml .= htmlentities($value);
				$xml .= "</$property>\n";
			};
			$xml .= "  </device>\n";
		}
		$xml .='</devices>';
		header('Content-type: text/xml');
		print_r($xml);
	}
}

class BCDVRCard{
	public $id;
	public $type;
	public $signal_type;
	public $fps_available;
	public $devices;

	public function __construct($id){
		$this->id = $id;
		$this->fps_available = 480; //BC card capacity
		$db = DVRDatabase::getInstance();
		$this->devices = $db->DBFetchAll("SELECT * FROM AvailableSources WHERE card_id='{$this->id}' ORDER BY id ASC");
		$this->type = count($this->devices);
		$port = 1;
		$this->signal_type = 'notconfigured';
		foreach ($this->devices as $key => $device){
			$tmp = $db->DBFetchAll("SELECT * FROM Devices WHERE device='{$device['device']}'");
			$this->devices[$key]['port'] = $port; $port++;
			if (!count($tmp)) { $this->devices[$key]['status'] = 'notconfigured'; $this->devices[$key]['id']=''; }
			 	else {
					$this->devices[$key] = array_merge($this->devices[$key], $tmp[0]);
					$this->devices[$key]['status'] = ($this->devices[$key]['disabled']) ? 'disabled' : 'OK';
					$this->signal_type = ($this->devices[$key]['signal_type']) ? $this->devices[$key]['signal_type'] : 'notconfigured' ; //NTSC is the default
				}
			(empty($this->devices[$key]['video_interval']) || !empty($this->devices[$key]['disabled'])) or $this->fps_available -= (30/$this->devices[$key]['video_interval']) * (($this->devices[$key]['resolutionX']>=704) ? 4 : 1);
		}
	}
}

class DVRIPCameras{
	public $data;
	public $camera;
	public function __construct($id){
		if ($id == 'new'){
			!empty($_GET['m']) or $_GET['m']='';
			switch($_GET['m']){
				case 'model': $this->getModels($_GET['manufacturer']); break;
				case 'ops': $this->getOptions($_GET['model']); break;
				default: $this->getManufacturers(); break; 
			};
		} else { //pull info
			$db = DVRDatabase::getInstance();
			$this->camera = $db->DBFetchAll("SELECT * FROM Devices WHERE id=$id");
			$tmp = explode('|', $this->camera[0]['device']);
			$this->camera[0]['ipAddr'] = $tmp[0];
			$this->camera[0]['port'] = $tmp[1];
			$this->camera[0]['rtsp'] = $tmp[2];
		};
	}
	private function getManufacturers(){
		$db = DVRDatabase::getInstance();
		$this->data['manufacturers'] = $db->DBFetchAll("SELECT manufacturer FROM ipCameras GROUP by manufacturer");
	}
	private function getModels($m){
		$db = DVRDatabase::getInstance();
		$this->data['models'] = $db->DBFetchAll("SELECT model FROM ipCameras WHERE manufacturer='$m' ORDER BY model ASC");
	}
	private function getOptions($model){
		$db = DVRDatabase::getInstance();
		$this->data = $db->DBFetchAll("SELECT * FROM ipCameras WHERE model='$model'");
	}
}

function genRandomString($length = 4) {
    $characters = '0123456789abcdefghijklmnopqrstuvwxyz';
    $string = '';    
    for ($p = 0; $p < $length; $p++) {
        $string .= $characters[mt_rand(0, strlen($characters))];
    }
    return $string;
}

	$version = new DVRVersion();

?>
