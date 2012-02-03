<?php 

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

defined('INDVR') or exit();

if (empty($nload)){
	include("lang.php");
	include("var.php");
}

session_name(VAR_SESSION_NAME);
session_start();

init();

//debug
function var_dump_pre($mixed = null) {
	echo '<hr><pre>';
	var_dump($mixed);
	echo '</pre>';
	return null;
}

function init(){
	#strip extra spaces from post inputs
	foreach($_POST as $key => $value){
		if (!is_array($value)) { $_POST[$key] = trim($value); $_POST[$key] = database::escapeString($_POST[$key]); };
	};
}

function init_user(){
	$userId = (!empty($_SESSION['id'])) ? $_SESSION['id'] : false;
	$current_user = new user('id', $_SESSION['id']);
	$current_user->checkAccessPermissions('admin');
	return $current_user;
};

function is_assoc($array){
	return array_keys($array) !== range(0, count($array) - 1);
}

function getOs(){
	$it = $_SERVER['HTTP_USER_AGENT'];
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
	public static function getObject($table, $parameter = false , $value = false, $condition = '=', $other = ''){
		$data = self::query("SELECT * FROM ".database::escapeString($table).((!$parameter) ? '' : " WHERE ".database::escapeString($parameter).$condition." '".database::escapeString($value)."'").' '.$other);
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
			if ($v === true || $v === 'on' ) { $array[$p] = 1; } elseif ($v === false) {$array[$p] = 0; };
		}
		switch($type){
			case 'insert': return "INSERT INTO {$table} (".implode(", ", array_keys($array)).") VALUES ('".implode("', '", $array)."')"; break;
			case 'update':
				$parameter = database::escapeString($parameter);
				$value = database::escapeString($value);
				foreach ($array as $p => $v){
					$tmp[$p]=database::escapeString($p)."='".database::escapeString($v)."'";
				};
				return "UPDATE $table SET ".implode(", ", $tmp)." WHERE {$parameter}='{$value}'";
			break;
		}
	}
	public static function responseXml($status, $message = false, $data = ''){
		if (!$message || empty($message)) { $message = ($status) ? CHANGES_OK : CHANGES_FAIL; };
		if (empty($data)) { $data = '0'; };
		if (is_bool($status))
			$status = ($status) ? 'OK' : 'F'; #in compliance with interface
		header('Content-type: text/xml');
		echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"\x3f>
				<response>
					<status><![CDATA[{$status}]]></status>
					<msg><![CDATA[{$message}]]></msg>
					<data>{$data}</data>
				</response>";
				
	}
	public static function last_id(){ #wrapper instead of mysql_insert_id
		return bc_db_last_insert_rowid();
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
		$this->info['default_password'] = ($this->info['password']==md5('bluecherry'.$this->info['salt']) && $this->info['username']=='Admin') ? true : false;
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
		if (!empty($this->info['id'])){
			$tmp = data::query("SELECT * FROM ActiveUsers WHERE ip = '{$_SERVER['REMOTE_ADDR']}' AND id={$this->info['id']}");
			if (!$tmp) { #if user record does not exist -- insert new
				data::query("INSERT INTO ActiveUsers VALUES ({$this->info['id']}, '{$_SERVER['REMOTE_ADDR']}', '{$_SESSION['from_client']}', ".time().", 0)", true);
			} else { #or update if it exists, i.e. reload within 5 minutes
				data::query("UPDATE ActiveUsers SET time = ".time()." WHERE ip = '{$_SERVER['REMOTE_ADDR']}' AND id={$this->info['id']}", true);
			}
			data::query("DELETE FROM ActiveUsers WHERE time <".(time()-300), true);
			return (!empty($tmp[0]['kick'])) ? true : false;
		} else {
			return false;
		}
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
		if ($type == 'devices' && !$this->info['access_setup'] && $_GET['XML']) { return true; }
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
				if (!empty($_SESSION['from_client'])) { $_SESSION['from_client_override'] = true; }
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
		$tmp = -1;
		$response = self::checkUserData($data, $new);
		if ($response === true){
			$tmp = '';
			foreach($data['email'] as $key => $email){
				if (!empty($email)) {$tmp .=$email.':'.$data['limit'][$key].'|'; };
			}
			unset($data['limit']);
			$data['email'] = trim($tmp, '|');
			
			if ($new){
				$data['salt'] = data::getRandomString(4);
				$data['password'] = md5($data['password'].$data['salt']);
				$query = data::formQueryFromArray('insert', 'Users', $data);
				$response = USER_CREATED;
				$data = '';
			} else {
				$id = intval($data['id']);
				unset($data['id']);
				if ($data['password'] == '__default__') { unset($data['password']); }
					else {
						$data['salt'] = data::getRandomString(4);
						$data['password'] = md5($data['password'].$data['salt']);
					}
				$data['access_web'] = ($_POST['access_web'] == 'on') ? 1 : 0;
				$data['access_remote'] = ($_POST['access_remote'] == 'on') ? 1 : 0;
				$data['access_backup'] = ($_POST['access_backup'] == 'on') ? 1 : 0;
				$data['access_setup'] = ($_POST['access_setup'] == 'on') ? 1 : 0;
				$query = data::formQueryFromArray('update', 'Users', $data, 'id', $id);
				$response = false;
			}
			$check = (data::query($query, true)) ? true : false;
			$info = data::getObject('Users', 'username', 'Admin');
			$tmp = ($info[0]['password']!=md5('bluecherry'.$info[0]['salt'])) ? 'disposeGeneralMessage' : '';
			$tmp = ($new) ? data::last_id() : $tmp;
		}
		return array($check, $response, $tmp);
		
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
	public function getLayouts(){
		$this->info['layouts'] = data::query("SELECT layout_name FROM userLayouts WHERE user_id='{$this->info['id']}'");
	}
}

function devices($access_list = false){ #wrapper for all cameras
	$devices = data::query('SELECT id FROM Devices');
	foreach($devices as $i => $device){
		$tmp[$i] = device($device['id']);
		$tmp[$i]->isPtz = (!empty($tmp[$i]->info['ptz_control_protocol'])) ? true : false;
	}
	return $tmp;
}

function device($id){ #wrapper for camera/ipCamera
	$device = data::getObject('Devices', 'id', $id);
	if ($device[0]['protocol'] == 'IP'){
		$device = new ipCamera($device[0]['id']);
	} else {
		$device = new camera($device[0]['device']);
	};
	$device->info['ptz_presets'] = data::query("SELECT * FROM PTZPresets WHERE device_id=".$id);
	return $device;
}

class camera {
	public $info;
	public function __construct($device){
		$this->getInfo($device);
	}
	public function getInfo($device){
		$devices = array();
		$available = array();
		$devices = data::getObject('Devices', 'device', $device);
		$available = data::getObject('AvailableSources', 'device', $device);
		unset($available[0]['id']);
		if (!$devices){ #if does not exist in Devices
			$this->info['status'] = 'notconfigured';
			$this->info = array_merge($this->info, $available[0]);
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
				$required_capacity = 2; #2FPS min
				$disabled = ($container_card->info['available_capacity']>=2) ? 0 : 1;
				#use standard name for new devices
				$tmp = explode('|', $this->info['device']);
				$new_name = PORT." ".($tmp[2] + 1).ON_CARD.$container_card->info['order'];
				if ($container_card->info['encoding'] == 'notconfigured' || $container_card->info['encoding'] == 'NTSC'){
					$res['y']='240';
					$signal_type = 'NTSC';
				} else {
					$res['y'] = '288';
					$signal_type = 'PAL';
				};
				#if the card was swapped pci id may have changed
				if (data::getObject('Devices', 'device_name', $new_name)){ #if camera with this default name exists, update the device field
					$result = data::query("UPDATE Devices SET device='{$this->info['device']}' WHERE device_name='{$new_name}'", true);
				} else { #no such device existed
					$result = data::query("INSERT INTO Devices (device_name, resolutionX, resolutionY, protocol, device, driver, video_interval, signal_type, disabled) VALUES ('{$new_name}', 352, {$res['y']}, 'V4L2', '{$this->info['device']}', '{$this->info['driver']}', 15, '{$signal_type}', '{$disabled}')", true);
				}
				$msg = ($result && $disabled) ? NEW_DEV_NEFPS : false;
				$data = ($result && !$disabled) ? $container_card->info['available_capacity'] : $container_card->info['available_capacity']-$required_capacity;
				return array($result, $msg);
			break;
		}
	}
}

class ipCamera{
	public $info;
	private $options;
	public $ptzControl;
	private $control;
	public function __construct($id = false, $options = false){
		$this->options = $options;
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
		if (!empty($this->info['mjpeg_path'])) {
			$tmp = explode('|', $info[0]['mjpeg_path']);
			/* Older database has single path and no server/port */
			if (count($tmp) == 1) {
				$this->info['ipAddrMjpeg'] = $this->info['ipAddr'];
				$this->info['portMjpeg'] = 80;
			} else {
				$this->info['ipAddrMjpeg'] =	$tmp[0];
				$this->info['portMjpeg']   =	$tmp[1];
				$this->info['mjpeg_path']  =	$tmp[2];
			}
		}
		#get manufacturer and model information
		$tmp = data::query("SELECT * FROM ipCameras WHERE model='{$this->info['model']}'");
		$this->info['manufacturer'] = $tmp[0]['manufacturer'];
		
		if ($this->info['ptz_control_protocol']){ #if protocol is set get the preset
			$this->ptzControl = new cameraPtz($this);
		}
	}
	public function checkConnection(){ 
		ini_set('default_socket_timeout', 1);
		#needs server to check for RTSP //// $paths['rtsp']  = 'http://'.((empty($this->info['rtsp_username'])) ? '' : $this->info['rtsp_username'].':'.$this->info['rtsp_password'].'@').$this->info['ipAddr'].':'.$this->info['port'].$this->info['rtsp'];
		$paths['mjpeg'] = 'http://'.((empty($this->info['rtsp_username'])) ? '' : $this->info['rtsp_username'].':'.$this->info['rtsp_password'].'@').((empty($this->info['ipAddrMjpeg'])) ? $this->info['ipAddr'] : $this->info['ipAddrMjpeg']).':'.$this->info['portMjpeg'].$this->info['mjpeg_path'];
		$paths['http'] = 'http://'.((empty($this->info['rtsp_username'])) ? '' : $this->info['rtsp_username'].':'.$this->info['rtsp_password'].'@').((empty($this->info['ipAddrMjpeg'])) ? $this->info['ipAddr'] : $this->info['ipAddrMjpeg']);
		foreach($paths as $type => $path){
			$headers = @get_headers($path);
			$contents = @file_get_contents($path);
			if (!$headers) { $this->info['connection_status'][$type] = 'F'; continue; }
			preg_match("/([0-9]{3})/", $headers[0], $response_code);
			$this->info['connection_status'][$type] = ($response_code[0]=='200') ? 'OK' : $response_code[0];
		}
		return $this->info['connection_status'];
	}
	protected function autoConfigure($driver, $info){ #auto configure known cameras
		include("ipcamlib.php");
		$control = get_ipcam_control($driver, $info);
		if ($control) { $result = $control->auto_configure(); };
		return $result;
	}
	public static function create($data){
		if (!$data['ipAddr'])	{ return array(false, AIP_NEEDIP); };
		if (!$data['port'])	{ return array(false, AIP_NEEDPORT);};
		if (!$data['rtsp'])	{ return array(false, AIP_RTSPPATH); };
		$data['device'] = "{$data['ipAddr']}|{$data['port']}|{$data['rtsp']}";
		$model_info = data::query("SELECT driver FROM ipCameras WHERE model='{$data['models']}'");
		$result = data::query("INSERT INTO Devices (device_name, protocol, device, driver, rtsp_username, rtsp_password, resolutionX, resolutionY, mjpeg_path, model) VALUES ('".((empty($data['camName'])) ? $data['ipAddr'] : $data['camName'])."', 'IP', '{$data['ipAddr']}|{$_POST['port']}|{$_POST['rtsp']}', '{$model_info[0]['driver']}', '{$data['user']}', '{$data['pass']}', 640, 480, '{$data['ipAddrMjpeg']}|{$data['portMjpeg']}|{$data['mjpeg']}', '{$data['models']}')", true);
		#for acti to proper streaming method
		$acti_config_result = false;
		$message = ($result) ? AIP_CAMADDED : false;
		if ($result)
		$message .= '<hr />'.self::autoConfigure($model_info[0]['driver'], array(
				'ipAddr' =>$data['ipAddr'],
				'portMjpeg' =>$data['portMjpeg'],
				'rtsp_username' =>$data['user'],
				'rtsp_password' =>$data['pass']
		)); 
		return array($result, $message);
	}
	public static function remove($id){
		return data::query("DELETE FROM Devices WHERE id='{$id}'", true);
	}
	public function changeState(){
		if (!$this->info['disabled']) { self::autoConfigure($this->info['driver'], $this->info); }
		return array(data::query("UPDATE Devices SET disabled=".(($this->info['disabled']) ? 0 : 1)." WHERE id={$this->info['id']}", true));
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
		#get card order, since mysql does not support ROW_NUMBER
		$cards = data::query("SELECT * FROM AvailableSources GROUP BY card_id");
		foreach($cards as $id => $card){
			if ($card['card_id'] == $this->info['id']) 
				$this->info['order'] = $id+1;
		};
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
		if (!data::query("UPDATE Devices SET resolutionY=$resolution_full[1] WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$this->info['id']}') AND resolutionY>300", true)) return false;
		if (!data::query("UPDATE Devices SET resolutionY=$resolution_quarter[1] WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$this->info['id']}') AND resolutionY<300", true)) return false;
		
		return array(true, DEVICE_ENCODING_UPDATED);
	}
	public function enableAllPorts(){
		foreach ($this->cameras as $key => $camera){
			$camera->changeState();
		}
		return true;
	}
}

function ipCameras($type, $parameter = false){
	$parameter = ($parameter) ? database::escapeString($parameter) : false;
	switch($type){
		case 'manufacturers':
			return data::query("SELECT manufacturer FROM ipCameras GROUP by manufacturer");
		break;
		case 'models':
			return data::query("SELECT model FROM ipCameras WHERE manufacturer='{$parameter}' ORDER BY model ASC");
		break;
		case 'options':
			$data = data::query("SELECT * FROM ipCameras WHERE model='{$parameter}'");
			$driver = data::query("SELECT * FROM ipCameraDriver WHERE id='{$data[0]['driver']}'");
			return (is_array($driver[0])) ? array_merge($data[0], $driver[0]) : $data[0];
		break;
	}
}

class softwareVersion{
	public $version;
	public function __construct(){
		$this->version['installed'] = trim(@file_get_contents(VAR_PATH_TO_INSTALLED_VERSION));
		$time = data::getObject('GlobalSettings', 'parameter', 'G_LAST_SOFTWARE_VERSION_CHECK');
		$tmp = data::getObject('GlobalSettings', 'parameter', 'G_LAST_CURRENT_VERSION');
		if ($time[0]['value'] + 24*60*60 < time()) {
			$this->version['current'] = trim(@file_get_contents(VAR_PATH_TO_CURRENT_VERSION));
			data::query("UPDATE GlobalSettings SET value='".time()."' WHERE parameter='G_LAST_SOFTWARE_VERSION_CHECK'", true);
			if ($tmp) data::query("UPDATE GlobalSettings SET value='{$this->version['current']}' WHERE parameter='G_LAST_CURRENT_VERSION'", true);
				else data::query("INSERT INTO GlobalSettings VALUES ('G_LAST_CURRENT_VERSION', '{$this->version['current']}')", true); //to avoid whole db update
		} else {
			$this->version['current'] = $tmp[0]['value'];
		}
		$this->checkSoftwareVersion();
	}
	function checkSoftwareVersion(){
		
		if ($this->version['current']===$this->version['installed']){
			$ret = true;
		} else {
			system("dpkg --compare-versions ".escapeshellarg($this->version['installed'])." lt ".escapeshellarg($this->version['current']), $ret);
		}
		$this->version['up_to_date'] = $ret != 0;
		$this->version['current'] = substr($this->version['current'],
						strpos($this->version['current'], ":") + 1);
		$this->version['installed'] = substr($this->version['installed'],
						strpos($this->version['installed'], ":") + 1);
	}
	function getIpTablesVersion(){
		$installed = data::getObject('GlobalSettings', 'parameter', 'G_IPCAMLIST_VERSION');
		$installed = $installed[0]['value'];
		$current = @fopen(VAR_PATH_TO_IPCAMLIST_VERSION, 'r');
		
		if (!$current) return true;
		$current = trim(@fgets($current, 1024));
		return array($installed, $current);
	}
	function checkIpTablesVersion(){
		$versions = $this->getIpTablesVersion();
		return (version_compare($versions[1], $versions[0], '>'));
	}
}

class ipPtzPreset{
	public $preset;
	public function __construct($id = 'new'){
		if ($id!='new'){
			$this->preset = data::getObject('ipPtzCommandPresets', 'id', $id);
			$this->preset = $this->preset[0];
		} else {
			$this->preset['name'] = IPP_NEW;
		}
	}
	public static function remove($id){
		$result = data::query("DELETE FROM ipPtzCommandPresets WHERE id='{$id}'", true);
		data::responseXml($result);
	}
	
}

class liveViewLayout{
	public function __construct(){
	}
}
class cameraPtz{
	private $preset;
	protected $camera;
	public $command; #debug
	public function __construct($camera){
		$this->camera = $camera;
		if ($this->camera->info['protocol'] == 'IP'){ #load IP ptz command preset
			$this->preset = $this->getIpCommandPreset($camera->info['ptz_control_path']);
		}
	}
	public function move($command, $id=false){
		if ($this->camera->info['protocol'] == 'IP'){ #prepare command URL
			$this->command = $this->getCmd($command);
			$this->command = $this->prepareCmd($this->command);
			$result = @get_headers($this->command);
		}
	}
	private function getIpCommandPreset($id){
		return data::getObject("ipPtzCommandPresets", "id", $id);
	}
	private function prepareCmd($command, $id = false){
		if (!$this->preset[0]['http_auth']){ #if !http_auth then login/password should be in the GET parameters
			$command = str_replace(array('%USERNAME%', '%PASSWORD%', '%ID%'), array($this->camera->info['rtsp_username'], $this->camera->info['rtsp_password'], $id), $command);
			$command = "http://".$this->camera->info['ipAddr'].(($this->preset[0]['port']) ? ':'.$this->preset[0]['port'] : '').$command;
		}
		return $command;
	}
	private function getCmd($direction){
		if ($direction == 'stop'){
			return $this->preset[0]['stop'];
		}
		if ($direction == 'stop_zoom'){
			return (!empty($this->preset[0]['stop_zoom'])) ? $this->preset[0]['stop_zoom'] : $this->preset[0]['stop'];
		}
		switch ($direction['zoom']){
			case 'w': return $this->preset[0]['wide']; break;
			case 't': return $this->preset[0]['tight']; break;
		}
		switch ($direction['pan']){
			case 'l':
				switch ($direction['tilt']){
					case 'u': return $this->preset[0]['up_left']; break;
					case 'd': return $this->preset[0]['down_left']; break;
					default: return $this->preset[0]['mleft']; break;
				}
			break;
			case 'r':
				switch ($direction['tilt']){
					case 'u': return $this->preset[0]['up_right']; break;
					case 'd': return $this->preset[0]['down_right']; break;
					default: return $this->preset[0]['mright']; break;
				}
			break;
			default: #pan not set, just tilt
				switch ($direction['tilt']){
					case 'u': return $this->preset[0]['up']; break;
					case 'd': return $this->preset[0]['down']; break;
				}
			break;
		}
	}
	public function preset($command, $value = ''){
	}
}
#ip camera API abstraction
abstract class ipCameraControl{
	abstract public function set_streaming_method();
		/*
		0 : TCP Only
		1 : Multicast Over UDP
		2 : TCP and Multicast
		3. RTP Over UDP
		4. RTP Over Multicast
		5: RTP Over UDP and Multicast
		*/
}


class globalSettings{
	var $data;
	function __construct(){
		$data = $this->getGlobalInfo();
		foreach($data as $key => $entry){
			$this->data[$entry['parameter']] = $entry['value'];
		}
	}
	public static function getParameter($parameter){
		$tmp = data::getObject('GlobalSettings', 'parameter', $parameter);
		return $tmp[0]['value'];
	}
	private function getGlobalInfo(){
		return data::getObject('GlobalSettings');
	}
}

$global_settings = new globalSettings;

if (empty($global_settings->data['G_DISABLE_VERSION_CHECK']) || $global_settings->data['G_DISABLE_VERSION_CHECK']==0){
	$version = new softwareVersion;
}

?>
