<?php 

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */
 
//defined('INDVR') or exit();

include("lang.php");
include("var.php");

session_name(VAR_SESSION_NAME);
session_start();

//debug
function var_dump_pre($mixed = null) {
	echo '<hr><pre>';
	var_dump($mixed);
	echo '</pre>';
	return null;
}


#classes

#singleton database class, uses php5-bluecherry functions
class database{
	public static $instance;
	private function __construct(){
		$this->connect();
	}
	public static function getInstance() {
		self::$instance or self::$instance = new database();
		return self::$instance;
	}
	private function connect() {
		bc_db_open() or die(LANG_DIE_COULDNOTCONNECT);
	}
	public static function escapeString(&$string) {
		self::$instance or self::$instance = new database();
		$string = bc_db_escape_string($string);
		return $string;
	}
	/* Execute a result-less query */
	public function query($query) {
		return bc_db_query($query);
	}
	/* Execute a query that will return results */
	public function fetchAll($query) {
		return bc_db_get_table($query);
	}
}

class data{
	public static function query($query, $resultless = false){
		$db = database::getInstance();
		return ($resultless) ? $db->query($query) : $db->fetchAll($query);
	}
	public static function getObject($table, $parameter = false , $value = false, $condition = '='){
		$data = self::query("SELECT * FROM ".database::escapeString($table).((!$parameter) ? '' : " WHERE ".database::escapeString($parameter).$condition." '".database::escapeString($value)."'"));
		return ($data) ? $data : false;
	}
	public static function getRandomString($length = 4) {
		$s = '0123456789abcdefghijklmnopqrstuvwxyz';
		for ($p = 0; $p < $length; $p++)
			$string .= $s[mt_rand(0, strlen($s))];
		return $string;
	}
	public static function formQueryFromArray($type, $table, $array, $parameter = false, $value = false){
		foreach($array as $p => $v){
			if ($v === true || $v === 'on' ) { $array[$p] = 1; } elseif ($v == false) {$array[$p] = 0; };
		}
		switch($type){
			case 'insert': return "INSERT INTO {$table} (".implode(", ", array_keys($array)).") VALUES ('".implode("', '", $array)."')"; break;
			case 'update': 
				foreach ($array as $p => $v){
					$tmp[$p]=database::escapeString($p)."='".database::escapeString($v)."'";
				};
				return "UPDATE $table SET ".implode(", ", $tmp)." WHERE {$parameter}='{$value}'";
			break;
		}
	}
	public static function responseXml($status, $message = false, $data = ''){
		if (!$message || empty($message)) { $message = ($status) ? CHANGES_OK : CHANGES_FAIL; };
		$status = ($status) ? 'OK' : 'F'; #in compliance with interface
		header('Content-type: text/xml');
		echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"\x3f>
				<response>
					<status>{$status}</status>
					<msg>{$message}</msg>
					<data>{$data}</data>
				</response>";
				
	}
}

class user{
	public $info;
	public function __construct($parameter = false, $value = false){
		if ($parameter && $value) $this->getInfo($parameter, $value);
		
	}
	private function getInfo($parameter, $value){
		$info = data::getObject('Users', $parameter, $value);
		$this->info = $info[0];
		$this->info['access_ptz_list'] = explode(',', $this->info['access_ptz_list']);
		$this->info['access_device_list'] = explode(',', $this->info['access_device_list']);
	}
	private function checkUserData($data, $new=false){
		if (empty($data['username']))	{ return NO_USERNAME;	}
		if (empty($data['email']))		{ return NO_EMAIL;		}
		if (empty($data['password']))	{ return NO_PASS;		}
		if (data::getObject('Users', 'username', $data['username']) && $new) { return USERNAME_EXISTS; }
		return true;
	}
	private function checkPassword($password){
		return (md5($password.$this->info['salt'])===$this->info['password']) ? true : false;
	}
	private function updateActiveUsers(){ #updates user record and returns kick value
		$tmp = data::query("SELECT * FROM ActiveUsers WHERE ip = '{$_SERVER['REMOTE_ADDR']}' AND id={$_SESSION['id']}");
		if (!$tmp) { #if user record does not exist -- insert new
			data::query("INSERT INTO ActiveUsers VALUES ({$_SESSION['id']}, '{$_SERVER['REMOTE_ADDR']}', '{$_SESSION['from_client']}', ".time().", 0)", true);
		} else { #or update if it exists, i.e. reload within 5 minutes
			data::query("UPDATE ActiveUsers SET time = ".time()." WHERE ip = '{$_SERVER['REMOTE_ADDR']}' AND id={$_SESSION['id']}", true);
		}
		data::query("DELETE FROM ActiveUsers WHERE time <".(time()-300), true);
		return (!empty($tmp[0]['kick'])) ? true : false;
	}
	private function userStatus($type){
		if (empty($_SESSION['id'])) {
			if ($type == 'basic'){
				return ($this->basicAuthCheck()) ? true : false;
			} else {
				return false;
			}
		}
		if ($this->updateActiveUsers()) { $_SESSION['message'] = USER_KICKED; session_destroy(); return false; } #update records, check if user was kicked
		if ($_SESSION['from_client'] && !$_SESSION['from_client_manual']) return false; #require login when opening admin pages from client
		if ($this->info['access_setup']) { return true; } #admin user all priveleges granted
			else {
				return ($type != 'admin' || ($type == 'backup' && $this->info['access_backup'] )) ? true : false; #page does not require admin priv and if its backup user must have permissions
			}
	}
	public function checkAccessPermissions($type){
		if (!$this->userStatus($type)){
			echo JS_RELOAD;
			exit;
		}
	}
	
	private function basicAuthCheck(){
			if (!isset($_SERVER['PHP_AUTH_USER'])) {
				header('WWW-Authenticate: Basic realm="'.DVR_COMPANY_NAME.'"');
   				header('HTTP/1.0 401 Unauthorized');
				exit;
			} else {
				$this->getInfo('username', $_SERVER['PHP_AUTH_USER']);
				return ($this->checkPassword($_SERVER['PHP_AUTH_PW'])) ? true : false;
			}
	}
	public function camPermission($id){
		return (in_array($id, $this->info['access_device_list'])) ? false : true;
	}
	public function doLogin($password, $from_client = false){
		if (!$this->info['access_setup']){ #if user is not admin check for permissions to use web/client
			if (!$this->info['access_web'] && !$from_client)	{ return NA_WEB; };
			if (!$this->info['access_remote'] && $from_client)	{ return NA_CLIENT; };
		}
		if ($this->checkPassword($password)) { 
				$_SESSION['id'] = $this->info['id']; 
				$_SESSION['from_client'] = $from_client; 
				if ($_SESSION['from_client']) { $_SESSION['from_client_manual'] = true; } #if user manually logging in from client
				return 'OK'; } 
		else { return LOGIN_WRONG; };
	}
	public function doLogout(){
		if (!empty($_SESSION['from_client_manual'])){ #if returning to client unset manual login var
			unset($_SESSION['from_client_manual']);
		} else { #if from web -- destroy session on logout
			session_destroy(); 
		}
	}
	public static function update($data, $new = false){
		$check = false;
		$response = self::checkUserData($data, $new);
		if ($response === true){
			if ($new){
				$data['salt'] = data::getRandomString(4);
				$data['password'] = md5($data['password'].$data['salt']);
				$query = data::formQueryFromArray('insert', 'Users', $data);
				$response = USER_CREATED;
			} else {
				$id = intval($data['id']);
				unset($data['id']);
				if ($data['password'] == '__default__') { unset($data['password']); }
					else {
						$ud = data::query("SELECT salt FROM Users WHERE id='{$id}'");
						$data['password'] = md5($data['password'].$ud[0]['salt']);
					}
				$query = data::formQueryFromArray('update', 'Users', $data, 'id', $id);
				$response = false;
			}
			$check = (data::query($query, true)) ? true : false;
		}
		return array($check, $response);
	}
	public static function remove($id){
		return (data::query("DELETE FROM Users WHERE id='{$id}'", true));
	}
	public static function kick($ip){
		$ip = preg_replace("/[^(0-9)\.\:]/", "", $ip);
		if ($_SERVER['REMOTE_ADDR']!=$ip){
			return (data::query("UPDATE ActiveUsers SET kick = 1, time = ".time()." WHERE ip='{$ip}'", true)) ? true : false;
		} else {
			return AU_CANT_EOS;
		}
	}
}


class camera {
	public $info;
	public function __construct($device){
		$this->getInfo($device);
	}
	public function getInfo($device){
		$devices = data::getObject('Devices', 'device', $device);
		$available = data::getObject('AvailableSources', 'device', $device);
		unset($available[0]['id']);
		if (!$devices){ #if does not exist in Devices
			$this->info['status'] = 'notconfigured';
			$this->info += $available[0];
			$tmp = explode('|', $this->info['device']); 
			$this->info['new_name'] = PORT." ".($tmp[2] + 1).ON_CARD.$this->info['card_id'];
		} elseif (!$available){ #if does not exist in AS -- i.e. card removed, unmatched group
			$this->info['status'] = 'unmatched';
			$this->info += $devices[0];
		} else {
			$this->info['status'] = ($devices[0]['disabled']) ? 'disabled' : 'OK';
			array_merge($this->info, $devices, $available);
			$this->info += $devices[0] + $available[0];
		}
		$this->getPtzSettings();
	}
	private function getPtzSettings(){
		if (empty($this->info['ptz_control_path'])){
			$ptz_config = false;
		} else {
			$ptz_config['path'] = $this->info['ptz_control_path'];
			$ptz_config['protocol'] = $this->info['ptz_control_protocol'];
			$config = explode(",", $this->info['ptz_serial_values']);
			$ptz_config['addr'] = $config[0];
			$ptz_config['baud'] = $config[1];
			$ptz_config['bit'] = $config[2];
			switch($config[3]){
				case 'n': $ptz_config['pairity'] = 'None'; break;
				case 'e': $ptz_config['pairity'] = 'Even'; break;
				case 'o': $ptz_config['pairity'] = 'Odd';  break;
			}
			$ptz_config['stop_bit'] = $config[4];
			$this->info['ptz_config'] = $ptz_config;
		}
	}
	public static function updatePtzSettings($path, $baud, $bit, $parity, $stop_bit, $protocol, $addr, $id){
		switch($parity){
				case 'None': $pairty = 'n'; break;
				case 'Even': $pairty = 'e'; break;
				case 'Odd':  $pairty = 'o'; break;
		};
		$status = data::query("UPDATE Devices SET ptz_control_path='{$path}', ptz_control_protocol='{$protocol}', ptz_serial_values='{$addr},{$baud},{$bit},{$pairty},{$stop_bit}' WHERE id='{$id}'", true);
		data::responseXml($status);
	}
	public function changeResFps($type, $value){
		if ($type == 'RES'){ $res = explode('x', $_POST['value']); $res['x'] = intval($res[0]); $res['y'] = intval($res[1]); } else {
			$res['x'] = $this->info['resolutionX']; $res['y'] = $this->info['resolutionY']; 
		}
		$fps = ($type=='FPS') ? intval($_POST['value']) : (30/$this->info['video_interval']);
		$resX = ($type=='RES') ? ($res['x']) : $this->info['resolutionX'];
		
		$required_capacity = (($fps) * (($resX>=704) ? 4 : 1)) - ((30/$this->info['video_interval']) * (($this->info['resolutionX']>=704) ? 4 : 1));
		
		$container_card = new card($this->info['card_id']);
		if ($required_capacity > $container_card->info['available_capacity']){
			$result = false;
			$message = ENABLE_DEVICE_NOTENOUGHCAP;
		} else {
			$result = data::query("UPDATE Devices SET video_interval='".intval(30/$fps)."', resolutionX='{$res['x']}', resolutionY='{$res['y']}' WHERE id='{$this->info['id']}'", true);
			$data = $container_card->info['available_capacity']-$required_capacity;
		}
		return array($result, false, $data);
	}
	public function changeState(){
		$container_card = new card($this->info['card_id']);
		switch($this->info['status']){
			case 'OK':
				$result = data::query("UPDATE Devices SET disabled='1' WHERE id='{$this->info['id']}'", true);
				return array($result, false);
			break;
			case 'disabled':
				$required_capacity = (30/$this->info['video_interval']) * (($this->info['resolutionX']>=704) ? 4 : 1);
				if ($required_capacity > $container_card->info['available_capacity']){ #ns capacity to enable
					return array(false, ENABLE_DEVICE_NOTENOUGHCAP);
				} else {
					$result = data::query("UPDATE Devices SET disabled='0' WHERE id='{$this->info['id']}'", true);
					return array($result, false);
				}
			break;
			case 'notconfigured':
				$disabled = ($container_card->info['available_capacity']>=2) ? 0 : 1;
				if ($container_card->info['encoding'] == 'notconfigured' || $container_card->info['encoding'] == 'NTSC'){
					$res['y']='240';
					$signal_type = 'NTSC';
				} else {
					$res['y'] = '288';
					$signal_type = 'PAL';
				};
				$result = data::query("INSERT INTO Devices (device_name, resolutionX, resolutionY, protocol, device, driver, video_interval, signal_type, disabled) VALUES ('{$this->info['new_name']}', 352, {$res['y']}, 'V4L2', '{$this->info['device']}', '{$this->info['driver']}', 15, '{$signal_type}', '{$disabled}')", true);
				$msg = ($result && $disabled) ? NEW_DEV_NEFPS : false;
				$data = ($result && !$disabled) ? $container_card->info['available_capacity'] : $container_card->info['available_capacity']-$required_capacity;
				return array($result, $msg);
			break;
		}
	}
}

class ipCamera{
	public $info;
	public function __construct($id = false){
		if ($id) $this->getInfo(intval($id));
	}
	public function getInfo($id){
		$info = data::query("SELECT * FROM Devices WHERE id=$id");
		$this->info = $info[0];
		$tmp = explode('|', $info[0]['device']);
		$this->info['status'] = (!$info[0]['disabled']) ? 'OK' : 'disabled';
		$this->info['ipAddr'] = $tmp[0];
		$this->info['port'] = $tmp[1];
		$this->info['rtsp'] = $tmp[2];
	}
	public static function create(){
		if (!$_POST['ipAddr']){ data::responseXml(false, AIP_NEEDIP); return false;};
		if (!$_POST['port']){ data::responseXml(false, AIP_NEEDPORT); return false;};
		if (!$_POST['rtsp']){ data::responseXml(false, AIP_RTSPPATH); return false;};
		$model_info = data::query("SELECT driver FROM ipCameras WHERE model='{$_POST['models']}'");
		$status = data::query("INSERT INTO Devices (device_name, protocol, device, driver, rtsp_username, rtsp_password, resolutionX, resolutionY, mjpeg_path) VALUES ('{$_POST['ipAddr']}', 'IP', '{$_POST['ipAddr']}|{$_POST['port']}|{$_POST['rtsp']}', '{$model_info[0]['driver']}', '{$_POST['user']}', '{$_POST['pass']}', 640, 480, '{$_POST['mjpeg']}')", true);
		$message = ($status) ? AIP_CAMADDED : false;
		data::responseXml($status, $message);
	}
	public static function remove($id){
		return data::query("DELETE FROM Devices WHERE id='{$id}'", true);
	}
	public function changeState(){
		return data::query("UPDATE Devices SET disabled=".(($this->info['disabled']) ? 0 : 1)." WHERE id={$this->info['id']}", true);
	}
}

class card {
	public $info;
	public $cameras;
	public function __construct($id){
		$devices = data::query("SELECT device FROM AvailableSources WHERE card_id='{$id}'");
		$this->info['id'] = $id;
		$port = 0;
		$this->info['encoding'] = 'notconfigured';
		$used_capacity = 0;
		foreach ($devices as $key => $device){
			$this->cameras[$key] = new camera($device['device']);
			$port++; #count ports on the card
			$this->cameras[$key]->info['port'] = $port;
			$this->info['encoding'] = (!empty($this->cameras[$key]->info['signal_type'])) ? $this->cameras[$key]->info['signal_type'] : $this->info['encoding'];
			if (!empty($this->cameras[$key]->info['video_interval']) && !$this->cameras[$key]->info['disabled'] ){
				$used_capacity += ($this->cameras[$key]->info['resolutionX']>352) ? 4*(30/$this->cameras[$key]->info['video_interval']) : (30/$this->cameras[$key]->info['video_interval']);
			}
		}
	
		$this->info['ports'] = $port;
		$this->info['driver'] = $this->cameras[0]->info['driver'];
		$this->info['capacity'] = $GLOBALS['capacity'][$this->info['driver']];
		$this->info['available_capacity'] = $this->info['capacity']-$used_capacity;
	}
	public function changeEncoding(){
		$encoding = ($this->info['encoding']=='NTSC') ? 'PAL' : 'NTSC';
		$resolution_full = explode('x', $GLOBALS['resolutions'][$encoding][0]);
		$resolution_quarter = explode('x', $GLOBALS['resolutions'][$encoding][1]);
		
		if (!data::query("UPDATE Devices SET signal_type='{$encoding}' WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$this->info['id']}')", true)) return false;
		if (!data::query("UPDATE Devices SET resolutionY=$resolution_full[0] WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$this->info['id']}') AND resolutionY>300", true)) return false;
		if (!data::query("UPDATE Devices SET resolutionY=$resolution_quarter[0] WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$this->info['id']}') AND resolutionY<300", true)) return false;
		return array(true, DEVICE_ENCODING_UPDATED);
	}
	
}

class ipCameras{
	public $camera;
	public function __construct(){
		$m = (!empty($_GET['m'])) ? $_GET['m'] : '';
		switch($m){
			case 'model': $this->getModels($_GET['manufacturer']); break;
			case 'ops': $this->getOptions($_GET['model']); break;
			default: $this->getManufacturers(); break; 
		};
	}
	private function getManufacturers(){
		$this->data['manufacturers'] = data::query("SELECT manufacturer FROM ipCameras GROUP by manufacturer");
	}
	private function getModels($m){
		$this->data['models'] = data::query("SELECT model FROM ipCameras WHERE manufacturer='$m' ORDER BY model ASC");
	}
	private function getOptions($model){
		$this->data = data::query("SELECT * FROM ipCameras WHERE model='$model'");
	}
}

class softwareVersion{
	public $version;
	public function __construct(){
		$current = trim(@file_get_contents(VAR_PATH_TO_CURRENT_VERSION));
		$installed = trim(@file_get_contents(VAR_PATH_TO_INSTALLED_VERSION));
		system("dpkg --compare-versions ".escapeshellarg($installed).
		       " lt ".escapeshellarg($current), $ret);
		$this->version['up_to_date'] = $ret != 0;
		$this->version['current'] = substr($current,
						strpos($current, ":") + 1);
		$this->version['installed'] = substr($installed,
						strpos($installed, ":") + 1);
	}
}

$version = new softwareVersion;
?>
