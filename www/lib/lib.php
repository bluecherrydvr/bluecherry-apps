<?php 

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

defined('INDVR') or exit();

define ('DIRSEP', DIRECTORY_SEPARATOR); 
$site_path = __DIR__ . DIRSEP;
define ('SITE_PATH', str_replace('lib'.DIRSEP, '', $site_path));

define('SQLITE_DB', '/usr/share/bluecherry/sqlite/cameras.db');

require_once('Helpers.php');
require_once('Session.php');
require_once('VarPub.php');
require_once('Reply.php');
require_once('Inp.php');
require_once('View.php');
require_once('Controller.php');
require_once ('Ponvif.php');
require_once('bc_license_wrapper.php');

if (empty($nload)){
	include("lang.php");
	include("var.php");
	include("upgrade.php");
}

session_name(VAR_SESSION_NAME);
session_start();

ini_set('default_socket_timeout', 2);

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

function getRenderNodes() {
	$ret = array();

	for($i = 0; $i < 4; $i++) {
		$node = "/dev/dri/renderD".intval($i + 128);
		if (file_exists($node) && filetype($node) == "char")
			$ret[$i] = $node;
	}

	return $ret;
}

function getVaapiOptions() {
    $ret = array();
    $ret[0] = "None";
    $nodes = getRenderNodes();
    if (!empty($nodes))
        array_push($ret, $nodes);
    $ret[] = "Autodetect";

    return $ret;
}

function checkVaapiSupport($renderNode, $profile) {
	$output = array();
	$ret = 0;

	exec("vainfo --display drm --device ".$renderNode, $output, $ret);

	if (intval($ret) !== 0) {
		return -1;//Failure,No VAAPI support detected
	}

	$output = implode("\n", $output);

	return substr_count($output, $profile);
}

function autoDetectVaapiSetup() {
	$nodes = getRenderNodes();

	if (empty($nodes))
		return "None";

	$scores = array();
	$max_score = 0;
	$max_score_id = 0;

	for($i = 0; $i < count($nodes); $i++) {
		$scores[$i] = checkVaapiSupport($nodes[$i], "H264");
		$scores[$i] += checkVaapiSupport($nodes[$i], "HEVC");
		$scores[$i] += checkVaapiSupport($nodes[$i], "JPEG");
	}

	for($i = 0; $i < count($nodes); $i++) {
		if ($scores[$i] > $max_score) {
			$max_score = $scores[$i];
			$max_score_id = $i;
		}

	}

	return $nodes[$max_score_id];
}

function getLiveViewVideoOptions() {
    $ret = array();
    $ret[0] = "HLS";
    $ret[1] = "MJPEG";

    return $ret;
}

/**
 * Returns SQLite Database connection
 * 
 * @staticvar PDO $adapter
 * @return \PDO
 */

function getReadOnlyDb() {
    static $adapter;
    if(null === $adapter) {
        $adapter = new PDO('sqlite:' . SQLITE_DB);
    }
    return $adapter;
}


#classes

#singleton database class, uses mysqli functions
class database{
	public static $instance;
	private $dblink;
	private $dbname;
	private $dbuser;
	private $dbpassword;
	private $dbhost;
	private $connection_timeout = 10; // 10 second timeout
	private $max_retries = 3;

	private function __construct() {
		$this->connect();
	}
	public static function getInstance() {
		self::$instance or self::$instance = new database();
		return self::$instance;
	}

    public static function read_config() {
		$config_file = fopen("/etc/bluecherry.conf", 'r') or die(LANG_DIE_COULDNOTOPENCONF);
		$config_text = fread($config_file, filesize("/etc/bluecherry.conf"));
		fclose($config_file);

		$matches = array();

		preg_match("/dbname[[:blank:]]*=[[:blank:]]*\"(.*)\";/", $config_text, $matches) or die(LANG_DIE_COULDNOTOPENCONF);
		$dbname = $matches[1];

		preg_match("/user[[:blank:]]*=[[:blank:]]*\"(.*)\";/", $config_text, $matches) or die(LANG_DIE_COULDNOTOPENCONF);
		$dbuser = $matches[1];

		preg_match("/password[[:blank:]]*=[[:blank:]]*\"(.*)\";/", $config_text, $matches) or die(LANG_DIE_COULDNOTOPENCONF);
                $dbpassword = $matches[1];

		/* host value is not defined in configs created by old versions of bluecherry, set default value*/
		if (!preg_match("/host[[:blank:]]*=[[:blank:]]*\"(.*)\";/", $config_text, $matches))
		{
			$dbhost = "localhost";
		}
		else
		{
			$dbhost = $matches[1];
		}

        return array($dbname, $dbuser, $dbpassword, $dbhost);
    }

	private function load_config() {
        list($dbname, $dbuser, $dbpassword, $dbhost) = database::read_config();
		$this->dbname = stripslashes($dbname);
		$this->dbuser = stripslashes($dbuser);
		$this->dbpassword = stripslashes($dbpassword);
		$this->dbhost = stripslashes($dbhost);
	}

	// CRITICAL FIX: Improved connection with retry logic and health checking
	private function connect() {
		$this->load_config();
		
		$retries = 0;
		while ($retries < $this->max_retries) {
			// CRITICAL FIX: Set connection timeout
			$this->dblink = mysqli_init();
			if (!$this->dblink) {
				$retries++;
				continue;
			}
			
					// CRITICAL FIX: Set connection options for better reliability
		mysqli_options($this->dblink, MYSQLI_OPT_CONNECT_TIMEOUT, $this->connection_timeout);
		// Use constants only if they exist (PHP version compatibility)
		if (defined('MYSQLI_OPT_READ_TIMEOUT')) {
			mysqli_options($this->dblink, MYSQLI_OPT_READ_TIMEOUT, 30);
		}
		if (defined('MYSQLI_OPT_WRITE_TIMEOUT')) {
			mysqli_options($this->dblink, MYSQLI_OPT_WRITE_TIMEOUT, 30);
		}
			
			$this->dblink = mysqli_real_connect($this->dblink, $this->dbhost, $this->dbuser, $this->dbpassword, $this->dbname);
			
			if ($this->dblink) {
				// CRITICAL FIX: Set charset and other options
				mysqli_real_query($this->dblink, "set names utf8;");
				mysqli_real_query($this->dblink, "SET SESSION wait_timeout=300;");
				mysqli_real_query($this->dblink, "SET SESSION interactive_timeout=300;");
				return;
			}
			
			$retries++;
			if ($retries < $this->max_retries) {
				usleep(100000); // 100ms delay before retry
			}
		}

		// Check if the database connection was successful
		if (!$this->dblink) {
			header('HTTP/1.1 503 Service Unavailable');
			Reply::ajaxDie('unavailable', LANG_DIE_COULDNOTCONNECT);
		}
	}
	
	// CRITICAL FIX: Check connection health
	private function check_connection() {
		if (!$this->dblink || !mysqli_ping($this->dblink)) {
			error_log("Database connection lost, attempting to reconnect");
			$this->connect();
		}
	}
	
	public static function escapeString(&$string) {
		self::$instance or self::$instance = new database();
		self::$instance->check_connection();
		$string = mysqli_real_escape_string(self::$instance->dblink, $string);
		return $string;
	}
	/* Execute a result-less query */
	public function query($query) {
		$this->check_connection();
		
		$ret = false;
		$retries = 0;
		
		while ($retries < $this->max_retries) {
			$ret = mysqli_real_query($this->dblink, $query);
			
			if ($ret) {
				break;
			}
			
			$error = mysqli_error($this->dblink);
			if (strpos($error, 'MySQL server has gone away') !== false || 
				strpos($error, 'Lost connection') !== false) {
				$retries++;
				$this->connect();
				continue;
			}
			
			break;
		}

		if (!$ret)
			trigger_error(mysqli_error($this->dblink)." query:".$query, E_USER_ERROR);

		return $ret;
	}
	/* Execute a query that will return results */
	public function fetchAll($query) {
		$this->check_connection();
		
		$fetchedTable = array();
		$retries = 0;
		
		while ($retries < $this->max_retries) {
			$qresult = mysqli_query($this->dblink, $query, MYSQLI_STORE_RESULT);

			if ($qresult) {
				$fetchedTable = mysqli_fetch_all($qresult, MYSQLI_ASSOC);
				mysqli_free_result($qresult);
				break;
			}
			
			$error = mysqli_error($this->dblink);
			if (strpos($error, 'MySQL server has gone away') !== false || 
				strpos($error, 'Lost connection') !== false) {
				$retries++;
				$this->connect();
				continue;
			}
			
			trigger_error(mysqli_error($this->dblink)." query:".$query, E_USER_ERROR);
			break;
		}

		return $fetchedTable;
	}

	public function last_id() {
		$this->check_connection();
		return mysqli_insert_id($this->dblink);
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
        $string = '';
		$s = '0123456789abcdefghijklmnopqrstuvwxyz';
		for ($p = 0; $p < $length; $p++)
			$string .= $s[mt_rand(0, strlen($s) - 1)];
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
    
    public static function responseJSON($status, $message = false, $data = ''){
		if (!$message || empty($message)) { $message = ($status) ? CHANGES_OK : CHANGES_FAIL; };
		if (is_bool($status)) {
            $status = ($status) ? 6 : 7; #in compliance with interface
        } else {
            if ($status == 'INFO') {
                $status = 6;
            } else if ($status == 'OK') {
                $status = 6;
            } else if ($status == 'F') {
                $status = 7;
            }
        }

        Reply::ajaxDie($status, $message, $data);
	}

	public static function last_id(){ #wrapper instead of mysql_insert_id
		$db = database::getInstance();
		return $db->last_id();
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
		$this->info['access_ptz_list'] = (isset($this->info['access_ptz_list']) ? explode(',', $this->info['access_ptz_list']) : Array());
		$this->info['access_device_list'] = (isset($this->info['access_device_list']) ? explode(',', $this->info['access_device_list']) : Array());
	}
	private static function checkUserData($data, $new=false){
		if (empty($data['username']))	{ return NO_USERNAME;	};
		if (empty($data['email']))		{ return NO_EMAIL;		};
		if (empty($data['password']))	{ return NO_PASS;		};
		if ($data['id'] == $_SESSION['id'] && ($data['access_setup'] == 0 || $data['access_web'] == 0)) { return CANT_REMOVE_ADMIN; };
		if (data::getObject('Users', 'username', $data['username']) && $new) { return USERNAME_EXISTS; }
		return true;
	}
	private function checkPassword($password){
        if (isset($this->info['salt']) && isset($this->info['password'])) {
            return (md5($password.$this->info['salt'])===$this->info['password']) ? true : false;
        } else return false;
	}
	private function updateActiveUsers(){ #updates user record and returns kick value
		if (!empty($this->info['id'])){
			$from_client = (int)$_SESSION['from_client'];
			$tmp = data::query("SELECT * FROM ActiveUsers WHERE ip = '{$_SERVER['REMOTE_ADDR']}' AND id={$this->info['id']}");
			if (!$tmp) { #if user record does not exist -- insert new
				data::query("INSERT INTO ActiveUsers VALUES ({$this->info['id']}, '{$_SERVER['REMOTE_ADDR']}', '{$from_client}', ".time().", 0)", true);
			} else { #or update if it exists, i.e. reload within 5 minutes
				data::query("UPDATE ActiveUsers SET time = ".time().", from_client='{$from_client}' WHERE ip = '{$_SERVER['REMOTE_ADDR']}' AND id={$this->info['id']}", true);
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
		// update records, check if user was kicked
		if ($this->updateActiveUsers()) {
			$_SESSION['message'] = USER_KICKED;
			session_destroy();
			return false;
		}
		// require login when opening admin pages from client
		if ($_SESSION['from_client'] && !$_SESSION['from_client_manual'])
			return false;
		if ($type == 'devices' && !$this->info['access_setup'] && $_GET['XML'])
			return true;
		if ($type == 'backup' && !$this->info['access_backup'] && !$this->info['access_setup'])
			return false;
		// admin user all priveleges granted
		if ($this->info['access_setup'])
			return true;
		else {
			// page does not require admin priv
			return ($type != 'admin');
		}
	}
	public function checkAccessPermissions($type){
		if (!$this->userStatus($type)){
            if (!empty($this->info['id'])) {
                if (($this->info['access_setup'] == 0) && ($this->info['access_web'] == 1)) {
                    header("Location: /liveview");
        			exit;
                }
            }

            header("Location: /login");
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
		if (!$this->info) { return LOGIN_WRONG; };
		if (isset($this->info['access_setup']) && !$this->info['access_setup']){ #if user is not admin check for permissions to use web/client
			if ((isset($this->info['access_setup']) && !$this->info['access_web']) && !$from_client)	{ return NA_WEB; };
			if (!$this->info['access_remote'] && $from_client)	{ return NA_CLIENT; };
		}
		if ($this->checkPassword($password)) { 
				$_SESSION['id'] = $this->info['id'];
				if (!empty($_SESSION['from_client'])) { $_SESSION['from_client_override'] = true; }
				$_SESSION['from_client'] = $from_client; 
				if ($_SESSION['from_client']) { $_SESSION['from_client_manual'] = true; } #if user manually logging in from client
				if (isset($this->info['change_password']) && $this->info['change_password']) return 'OK CHANGE_PASSWORD';
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
		$data['access_web'] = (Inp::post('access_web') == 'on') ? 1 : 0;
		$data['access_remote'] = (Inp::post('access_remote') == 'on') ? 1 : 0;
		$data['access_backup'] = (Inp::post('access_backup') == 'on') ? 1 : 0;
		$data['access_setup'] = (Inp::post('access_setup') == 'on') ? 1 : 0;
		$response = self::checkUserData($data, $new);
		if ($response === true){
			$tmp = '';
			foreach($data['email'] as $key => $email){
				if (empty($email)){
					unset($data['email'][$key]);
				}
			}
			$data['email'] = implode('|', $data['email']);
			
			if ($new){
				$data['salt'] = data::getRandomString(4);
				$data['password'] = md5($data['password'].$data['salt']);
				unset($data['id']);
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
		$this->info['layouts'] = data::query("SELECT id as layout_id, layout_name, layout FROM userLayouts WHERE user_id='{$this->info['id']}'");
	}
}

function device($id){ #wrapper for camera/ipCamera
	$device = data::getObject('Devices', 'id', $id);
	if (substr($device[0]['protocol'], 0, 2) === 'IP'){
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
		if ($available) unset($available[0]['id']);
		if (!$devices){ #if does not exist in Devices
			$this->info['status'] = 'notconfigured';
			$this->info = array_merge($this->info, $available[0]);
		} elseif (!$available){ #if does not exist in AS -- i.e. card removed, unmatched group
			$this->info['status'] = 'unmatched';
			$this->info += $devices[0];
		} else {
			$this->info['status'] = ($devices[0]['disabled']) ? 'disabled' : 'OK';
			$this->info += $devices[0] + $available[0];
		}
		$this->getPtzSettings();

		$dev_spec_parts = explode('|', $this->info['device']);
		$card_port_nr = $dev_spec_parts[2];
		if ($this->info['driver'] == 'tw5864') {
			$devfile = shell_exec("ls {$dev_spec_parts[1]}/video4linux | sed 's/video//' | sort -n | sed 's/^/video/' | tail -n +$card_port_nr | head -n 1 | tr -d '\\n'");
		} else {
			$devfile = "/dev/video" . (1 + ($this->info['card_id'] & 255) + $card_port_nr);
		}

		$this->info['devfile'] = $devfile;
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
				case 'n': $ptz_config['parity'] = 'None'; break;
				case 'e': $ptz_config['parity'] = 'Even'; break;
				case 'o': $ptz_config['parity'] = 'Odd';  break;
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
		data::responseJSON($status);
	}
	public function changeResFps($type, $value){
		$data = false;
		$message = false;
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
		return array($result, $message, $data);
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

				if ($container_card->info['driver'] == 'tw5864') {
				    foreach ($container_card->cameras as $key => $val) {
				        if ($val->info['device'] == $this->info['device']) {
				            $port_numb = $val->info['port'];
				       }
				    }
				} else {
				    #use standard name for new devices
				    $tmp = explode('|', $this->info['device']);
                                    if (!isset($tmp[2])) $tmp[2] = 0;
				    $port_numb = $tmp[2] + 1;
				}
				$new_name = PORT." ".$port_numb.ON_CARD.$container_card->info['order'];

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

	public function setControls($controls)
	{
		// Fetch available video4linux controls
		$ctrls_rawout = shell_exec("v4l2-ctl --device {$this->info['devfile']} --list-ctrls");
		if (!$ctrls_rawout)
			return false;

		$defined_ctrls = array();
		foreach (explode("\n", $ctrls_rawout) as $ctrl_line) {
			$line_split = preg_split("/[\s]+/", $ctrl_line);
			if ($line_split[2] != ':')
				return false;
			$ctrl = array();
			$ctrl['name'] = $line_split[0];
			foreach (array_slice($line_split, 3) as $property) {
				$name_and_val = split('=', $property);
				$ctrl[$name_and_val[0]] = $name_and_val[1];
			}
			$defined_ctrls[$ctrl['name']] = $ctrl;
		}

		$setting_arr = array();

		// Check which controls are requested for setting, and prepare a shell command
		// TODO "video_quality" needs a workaround to use control "h264_minimum_qp_value" with inverted scale

		foreach($controls as $setting_name => $setting) {
			$def = $defined_ctrls[$setting_name];
			if (!isset($def))
				continue;

			/*
			 * x e [x_min, x_max] ([0, 100])
			 * y e [y_min, y_max] (e.g. [-128, 127]
			 *
			 * y(x) = a * x + b
			 *
			 * a * x_min + b = y_min
			 * a * x_max + b = y_max
			 */
			$x = $setting;

			$x_min = 0;
			$x_max = 100;
			$y_min = $def['min'];
			$y_max = $def['max'];
			$a = ($y_max - $y_min) / ($x_max - $x_min);
			$b = $y_min - $a * $x_min;
			$y = $a * $x + $b;

			$setting_arr[] = $setting_name . '=' . $y;
		}

		if (empty($setting_arr))
			return false;

		$ret = shell_exec("v4l2-ctl --device {$this->info['devfile']} --set-ctrls=" . implode(',', $setting_arr));
		return bool($ret);
	}
}

class ipCamera{
	public	$info;
	private $options;
	public	$ptzControl;
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
		$this->info['device_name'] = empty($info[0]['device_name']) ? "Unnamed" : $info[0]['device_name'];
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
		$this->info['substream_enabled'] = ($info[0]['substream_mode'] != '0') ? '1' : '0';
		$tmp = explode('|', $info[0]['substream_path']);
		if (isset($tmp[2]))
			$this->info['substream'] = $tmp[2];
		#get manufacturer and model information
                $stmt = getReadOnlyDb()->prepare(
                    'SELECT m.manufacturer ' . 
                    'FROM cameras AS c ' . 
                    'JOIN manufacturers AS m ON c.manufacturer = m.id ' . 
                    'WHERE api_id = ?'
                );
                $stmt->execute(array($this->info['model']));
		$tmp = $stmt->fetch(PDO::FETCH_ASSOC);
		if (!empty($tmp)) { $this->info['manufacturer'] = $tmp['manufacturer']; };
		
		if ($this->info['ptz_control_protocol']){ #if protocol is set get the preset
			$this->ptzControl = new cameraPtz($this);
		}
	}
	public function checkConnection() {
		 //FIXME: Currently only supports RTSP. MJPEG is currently untested
		ini_set('default_socket_timeout', 1);

		$path = "";
		$args = "";

		switch($this->info['protocol'])  {
			case 'IP-RTSP':
				$path = 'rtsp://'.((empty($this->info['rtsp_username'])) ? '' : urlencode($this->info['rtsp_username']).':'.urlencode($this->info['rtsp_password']).'@').$this->info['ipAddr'].':'.$this->info['port'].$this->info['rtsp'];
				$rtp_args_menu = array("-rtsp_flags +prefer_tcp", "-rtsp_transport tcp", "-rtsp_transport +udp+udp_multicast");
				$args = $rtp_args_menu[$this->info['rtsp_rtp_prefer_tcp']];
				break;
			case 'IP-MJPEG': 
				//FIXME: This is the old logic for testing MJPEG. Testing for MJPEG is currently not supported by the bundled ffprobe method used for RTSP
				$path = 'http://'.((empty($this->info['rtsp_username'])) ? '' : urlencode($this->info['rtsp_username']).':'.urlencode($this->info['rtsp_password']).'@').((empty($this->info['ipAddrMjpeg'])) ? $this->info['ipAddr'] : $this->info['ipAddrMjpeg']).':'.$this->info['portMjpeg'].$this->info['mjpeg_path'];
				$headers = @get_headers($path);
				if (!$headers) { $this->info['connection_status']['success'] = false; return; }
				preg_match("/([0-9]{3})/", $headers[0], $response_code);
				$this->info['connection_status']['success'] = ($response_code[0]=='200') ? true : false;
				return;  
		}

		//-> '-timeout' is measured in microseconds
		$ffprobe_output = shell_exec(
			"/usr/lib/bluecherry/ffprobe -timeout 5000000 -hide_banner -show_format -show_streams -print_format json ".$args. " " . escapeshellarg($path));

		$rtsp_data = json_decode($ffprobe_output, true);

		$this->info['connection_status']['success'] = array_key_exists('streams', $rtsp_data) ? count($rtsp_data['streams']) > 0 : false;
	}

	protected static function autoConfigure($driver, $info){ #auto configure known cameras
		include_once("ipcamlib.php");
		$result = false;
		$control = get_ipcam_control($driver, $info);
		if ($control) { $result = $control->auto_configure(); };
		return $result;
	}
	private static function prepareData($rawData, $self_id = false){
		#prepare device
			if (empty($rawData['ipAddr']))	{ return array(false, AIP_NEEDIP); };
			if (empty($rawData['camName']))	{ return array(false, AIP_NEEDIP); };
			if ($rawData['protocol'] == 'IP-MJPEG'){
				if (empty($rawData['mjpeg']))	{ return array(false, AIP_NEEDMJPEG); }
				if (empty($rawData['portMjpeg'])) { return array(false, AIP_NEEDMJPEGPORT); }
			} else{
				if (empty($rawData['port']))	{ return array(false, AIP_NEEDPORT);};
				if (empty($rawData['rtsp']))	{ return array(false, AIP_NEEDRTSP); };
			}
			empty ($rawData['rtsp']) or $rawData['rtsp'] = (substr($rawData['rtsp'][0], 0, 1) != '/') ? '/'.$rawData['rtsp'] : $rawData['rtsp'];
			$data['device'] = "{$rawData['ipAddr']}|{$rawData['port']}|{$rawData['rtsp']}";

		#prepare device name
			$data['device_name'] = (empty($rawData['camName'])) ? $rawData['ipAddr'] : $rawData['camName'];
			if ($self_id === false) { # check this only in case of creation, don't check on edit
				$duplicate_name = data::getObject('Devices', 'device_name', $data['device_name']);
				if ($duplicate_name) { return array(false, AIP_NAME_ALREADY_EXISTS); }
			}
		#prepare mjpeg path
            $rawData['mjpeg'] = (!empty($rawData['mjpeg']) && (substr($rawData['mjpeg'][0], 0, 1) != '/')) ? '/'.$rawData['mjpeg'] : $rawData['mjpeg'];
			$data['mjpeg_path'] = "{$_POST['ipAddr']}|{$_POST['portMjpeg']}|{$_POST['mjpeg']}";	
		#prepare audio check box, ignored for new devices
			$data['audio_disabled'] = (!empty($rawData['audio_enabled']) && $rawData['audio_enabled']=='on') ? 0 : 1;
		#prepare debug level, ignored for new devices
			$data['substream_mode'] = (!empty($rawData['substream_enabled']) && $rawData['substream_enabled']=='on') ? 1 : 0;
			empty ($rawData['substream']) or $rawData['substream'] = (substr($rawData['substream'][0], 0, 1) != '/') ? '/'.$rawData['substream'] : $rawData['substream'];
			if ($rawData['protocol'] == "IP-MJPEG")
				$data['substream_path'] = "{$rawData['ipAddr']}|{$rawData['portMjpeg']}|{$rawData['substream']}";
			else
				$data['substream_path'] = "{$rawData['ipAddr']}|{$rawData['port']}|{$rawData['substream']}";

			$data['debug_level'] = (!empty($rawData['debug_level']) && $rawData['debug_level']=='on') ? 1 : 0;
		#prepare rtsp username/password
			$data['rtsp_username'] = empty($_POST['user']) ?  '' : $rawData['user'];
			$data['rtsp_password'] = empty($_POST['pass']) ?  '' : $rawData['pass'];
		#prepare model name
			$data['model'] = (!empty($rawData['models'])) ? $rawData['models'] : false;
		#prepare driver
			$data['driver'] = '';
			$data['rtsp_rtp_prefer_tcp'] = $rawData['prefertcp'];
			$data['protocol'] = ($rawData['protocol'] == "IP-MJPEG") ? "IP-MJPEG" : "IP-RTSP"; //default to rtsp
			$data['onvif_port'] = (empty($rawData['onvif_port'])) ? "80" : $rawData['onvif_port']; //default to Onvif port
		#prepare hls window
			$data['hls_window_size'] = (empty($rawData['hls_window_size'])) ? "5" : $rawData['hls_window_size']; //default to hls window size
			$data['hls_segment_duration'] = $rawData['hls_segment_duration']; //default to hls segment duration
			$data['hls_segment_size'] = $rawData['hls_segment_size']; //default to hls segment size

			//var_dump_pre($data); exit();
			
		$duplicate_path = data::getObject('Devices', 'device', $data['device']);
		$duplicate_path = ($duplicate_path[0]['id'] == $self_id) ? false : $duplicate_path; 
		if (!empty($duplicate_path) && ($duplicate_path[0]['mjpeg_path'] == $data['mjpeg_path'])) {
			return array(false, AIP_ALREADY_EXISTS);
		}
		return array(true, $data);
	}
	public function edit($data){
		$data = self::prepareData($data, $this->info['id']);
		#if errors were detected -- return error
        if (!$data[0]) { 
            return $data; 
        } else { 
            //unset($data[1]['device_name']); 
        }
		#if there were no errors, edit the camera
		$query = data::formQueryFromArray('update', 'Devices', $data[1], 'id', $this->info['id']);
		$result = data::query($query, true);

		return array($result, false);
	}
	public static function create($rawData){
		#check the number of the allowed devices
		if (!self::checkLimitDevices()) { return array(false, AIP_LIMIT_ALLOWED_DEVICES); };

		#get the data ready
		
		$data = self::prepareData($rawData);
		#if errors were detected -- return error
		if (!$data[0]) { return $data; } else { $data = $data[1]; };
		#if there were no errors, add the camera

		$hls_segment_size = $data['hls_segment_size']  ? $data['hls_segment_size'] : 0;
		$hls_segment_duration = $data['hls_segment_duration'] ?  $data['hls_segment_duration'] : 0;
		$inputQuery = "INSERT INTO Devices (device_name, protocol, device, driver, rtsp_username, rtsp_password, resolutionX, resolutionY, mjpeg_path, model, rtsp_rtp_prefer_tcp, onvif_port, substream_path, hls_window_size, hls_segment_size, hls_segment_duration) VALUES ('{$data['device_name']}', '{$data['protocol']}', '{$data['device']}', '{$data['driver']}', '{$data['rtsp_username']}', '{$data['rtsp_password']}', 640, 480, '{$data['mjpeg_path']}', '{$data['model']}', {$data['rtsp_rtp_prefer_tcp']}, {$data['onvif_port']},'{$data['substream_path']}', {$data['hls_window_size']}, {$hls_segment_size}, {$hls_segment_duration})";

		$result = data::query($inputQuery, true);
		#try to automatically set the camera up
		$message = ($result) ? AIP_CAMADDED : false;
		if ($result)
		$autoconfigure = self::autoConfigure($data['driver'], array(
				'ipAddr' =>$rawData['ipAddr'],
				'portMjpeg' =>$rawData['portMjpeg'],
				'rtsp_username' =>$rawData['user'],
				'rtsp_password' =>$rawData['pass']
		)); 
		if (!empty($autoconfigure)) { $message .= '<hr />'.$autoconfigure; };
		return array($result, $message);
	}
	public static function remove($id){
		$attempts = 10;
		$device_delete_ret;
		do {
			data::query("DELETE FROM EventsCam WHERE device_id='{$id}'", true);
			data::query("DELETE FROM OnvifEvents WHERE device_id='{$id}'", true);
			$media_files = data::query("SELECT id, filepath FROM Media WHERE device_id='{$id}' ORDER BY id DESC");
            if (!empty($media_files)) {
			    $highest_id = $media_files[0]['id'];
    			foreach ($media_files as $key => $media_file) {
    				if (file_exists($media_file['filepath']))
    					@unlink($media_file['filepath']);
                }
    			# Remove all traversed entries, not touching the possible to appear entries with higher id
    			data::query("DELETE FROM Media WHERE device_id='{$id}' AND id <= {$highest_id}", true);
            }

			data::query("DELETE FROM PTZPresets WHERE device_id='{$id}'", true);
			$device_delete_ret = data::query("DELETE FROM Devices WHERE id='{$id}'", true);
			$attempts--;
		} while ((!$device_delete_ret) && ($attempts > 0));
		return $device_delete_ret;
	}
	public function changeState(){
		if (!$this->info['disabled']) { self::autoConfigure($this->info['driver'], $this->info); }
		return array(data::query("UPDATE Devices SET disabled=".(($this->info['disabled']) ? 0 : 1)." WHERE id={$this->info['id']}", true));
	}
	private static function checkLimitDevices(){
		// Always return true for free and open source version - no device limits
		return true;
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
		$cards = data::query("SELECT card_id FROM AvailableSources GROUP BY card_id");
		foreach($cards as $id => $card){
			if ($card['card_id'] == $this->info['id']) 
				$this->info['order'] = $id+1;
		};
		$this->info['ports'] = $port;
		$this->info['driver'] = (isset($this->cameras[0]->info['driver']) ? $this->cameras[0]->info['driver'] : '');
		$this->info['capacity'] = (isset($GLOBALS['capacity'][$this->info['driver']]) ? $GLOBALS['capacity'][$this->info['driver']] : '');
		$this->info['available_capacity'] = $this->info['capacity']-$used_capacity;
	}
	public function changeEncoding(){
		$encoding = ($this->info['encoding']=='NTSC') ? 'PAL' : 'NTSC';
		$resolution_full = explode('x', $GLOBALS['resolutions'][$encoding][0]);
		$resolution_quarter = explode('x', $GLOBALS['resolutions'][$encoding][1]);
		
		if (!data::query("UPDATE Devices SET signal_type='{$encoding}' WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$this->info['id']}')", true)) return false;
		if (!data::query("UPDATE Devices SET resolutionY=$resolution_full[1] WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$this->info['id']}') AND resolutionY>300", true)) return false;
		if (!data::query("UPDATE Devices SET resolutionY=$resolution_quarter[1] WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$this->info['id']}') AND resolutionY<300", true)) return false;
		//reset motion_map to default for solo devices
		if (substr($this->info['driver'], 0, 5) == 'solo6'){
			//22x15 for NTSC, 22x18 for PAL
			if ($encoding == 'PAL')
				$mapsize = 22*18;
			else
				$mapsize = 22*15;

			$motion_map = str_repeat('3', $mapsize);

			if (!data::query("UPDATE Devices SET motion_map='{$motion_map}' WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$this->info['id']}')", true)) return false;
		}

		return array(true, DEVICE_ENCODING_UPDATED);
	}
	public function enableAllPorts(){
        if (!empty($this->cameras)) {
    		foreach ($this->cameras as $key => $camera){
    			$camera->changeState();
            }
        }
		return true;
	}
}

function ipCameras($type, $parameter = false){
	$parameter = ($parameter) ? database::escapeString($parameter) : false;
        $dbo = getReadOnlyDb();
	switch($type){
            case 'manufacturers':
                return $dbo->query('SELECT manufacturer ' . 
                                   'FROM manufacturers ' . 
                                   'ORDER BY manufacturer ASC')
                           ->fetchAll(PDO::FETCH_ASSOC);
                break;
            case 'models':
                $stmt = $dbo->prepare(
                    'SELECT api_id, model ' . 
                    'FROM cameras AS C ' . 
                    'JOIN manufacturers AS M ON C.manufacturer = M.id ' . 
                    'WHERE M.manufacturer = ?'
                );
                $stmt->execute(array($parameter));
                return $stmt->fetchAll(PDO::FETCH_KEY_PAIR);
		break;
            case 'options':
                $stmt = $dbo->prepare(
                    'SELECT * ' . 
                    'FROM cameras AS C ' . 
                    'WHERE C.model = ? ' . 
                    'OR C.api_id = ?'
                );
                $stmt->execute(array($parameter, $parameter));
                return $stmt->fetch(PDO::FETCH_ASSOC);
		break;
	}
}

class softwareVersion{
	public $version;
	public function __construct($global_settings){
		$this->version['installed'] = trim(@file_get_contents(VAR_PATH_TO_INSTALLED_VERSION));
		$time = $global_settings->data['G_LAST_SOFTWARE_VERSION_CHECK'];
		$tmp = isset($global_settings->data['G_LAST_CURRENT_VERSION']) ? 
			$global_settings->data['G_LAST_CURRENT_VERSION'] : false;

		if ($time + 24*60*60 < time()) {
			$this->version['current'] = trim(@file_get_contents(VAR_PATH_TO_CURRENT_VERSION.'?uid='.bc_license_machine_id()));
			data::query("UPDATE GlobalSettings SET value='".time()."' WHERE parameter='G_LAST_SOFTWARE_VERSION_CHECK'", true);
			if ($tmp) data::query("UPDATE GlobalSettings SET value='{$this->version['current']}' WHERE parameter='G_LAST_CURRENT_VERSION'", true);
			else data::query("INSERT INTO GlobalSettings VALUES ('G_LAST_CURRENT_VERSION', '{$this->version['current']}')", true); //to avoid whole db update
		} else {
			$this->version['current'] = $tmp;
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
		data::responseJSON($result);
	}
	
}

class cameraPtz{
	private $preset;
	protected $camera;
	public $command; #debug
	public function __construct($camera){
		$this->camera = $camera;
		if (substr($this->camera->info['protocol'], 0, 2) === 'IP'){ #load IP ptz command preset
			$this->preset = $this->getIpCommandPreset($camera->info['ptz_control_path']);
		}
	}
	public function move($command, $id=false){
		if (substr($this->camera->info['protocol'], 0, 2) === 'IP'){ #prepare command URL
			if (strtolower($this->preset[0]['protocol']) == 'onvif') {
				$this->moveOnvif($command);
			} else {
				$this->command = $this->getCmd($command);
				$this->command = $this->prepareCmd($this->command);

				$ch = curl_init();
				curl_setopt($ch, CURLOPT_URL, $this->command);
				curl_setopt($ch, CURLOPT_USERPWD, $this->camera->info['rtsp_username'].":".$this->camera->info['rtsp_password']);
				// Set so curl_exec returns the result instead of outputting it.
				curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
				curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
				curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
				// Get the response and close the channel.
				$response = curl_exec($ch);
				curl_close($ch);
			}
		}
	}

	private function moveOnvif($command)
	{
		if (is_array($command) && (Inp::get('command') == 'move')) {
			$speed_pantilt_x = '0.5';
			$speed_pantilt_y = '0.5';
			$pantilt_x = 0;
			$pantilt_y = 0;
			$zoom = 0;
			$zoom_speed = '0.5';

			$xy_value = 5 / 100 ;

			if ($command['pan'] == 'r') {
				$pantilt_x = $xy_value;
			}

			if ($command['pan'] == 'l') {
				$pantilt_x = -1 * $xy_value;
			}

			if ($command['tilt'] == 'u') {
				$pantilt_y = $xy_value;
			}

			if ($command['tilt'] == 'd') {
				$pantilt_y = -1 * $xy_value;
			}

			if ($command['zoom'] == 't') {
				$zoom = '0.1';
			}

			if ($command['zoom'] == 'w') {
				$zoom = '-0.1';
			}

			$ponvif = new Ponvif();
			$ponvif->setIPAddress($this->camera->info['ipAddr'] . ':' . $this->camera->info['onvif_port']);
			$ponvif->setUsername($this->camera->info['rtsp_username']);
			$ponvif->setPassword($this->camera->info['rtsp_password']);


			try {
				$init = $ponvif->initialize();
				$sources = $ponvif->getSources();

				if (empty($sources) || $ponvif->isFault($sources)) {
					// error
				} else {
					$profileToken = $sources[0][0]['profiletoken'];

					if ($pantilt_x || $pantilt_y) {
						$ponvif->ptz_ContinuousMove($profileToken, $pantilt_x, $pantilt_y, $speed_pantilt_x, $speed_pantilt_y);
					}

					if ($zoom) {
						$ponvif->ptz_ContinuousMoveZoom($profileToken, $zoom, $zoom_speed);
					}
				}
			} catch (Exception $e) {
                    }
            } elseif (!is_array($command) && $command == 'stop') {

                    $ponvif = new Ponvif();
                    $ponvif->setIPAddress($this->camera->info['ipAddr'] . ':' . $this->camera->info['onvif_port']);
                    $ponvif->setUsername($this->camera->info['rtsp_username']);
                    $ponvif->setPassword($this->camera->info['rtsp_password']);


                    try {
                            $init = $ponvif->initialize();
                            $sources = $ponvif->getSources();

                            if (empty($sources) || $ponvif->isFault($sources)) {
                                    // error
                            } else {
                                    $profileToken = $sources[0][0]['profiletoken'];

                                    $ponvif->ptz_Stop($profileToken, true, false);
                            }
                    } catch (Exception $e) {
                    }
            }
    }

	private function getIpCommandPreset($id){
		return data::getObject("ipPtzCommandPresets", "id", $id);
	}
	private function prepareCmd($command, $id = false){
		$command = ((isset($command[0])) && ($command[0]=='/')) ? $command : '/'.$command;
		if (!$this->preset[0]['http_auth']){ #if !http_auth then login/password should be in the GET parameters
			$command = str_replace(array('%USERNAME%', '%PASSWORD%', '%ID%'), array($this->camera->info['rtsp_username'], $this->camera->info['rtsp_password'], $id), $command);
			$command = $this->preset[0]['protocol']."://".$this->camera->info['ipAddr'].(($this->preset[0]['port']) ? ':'.$this->preset[0]['port'] : '').$command;
		} else {
			$command = $this->preset[0]['protocol']."://{$this->camera->info['rtsp_username']}:{$this->camera->info['rtsp_password']}@".$this->camera->info['ipAddr'].(($this->preset[0]['port']) ? ':'.$this->preset[0]['port'] : '').$command;
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
		if (empty($this->data['G_MAX_RECORD_TIME'])) {
			data::query("DELETE FROM GlobalSettings where parameter = 'G_MAX_RECORD_TIME'", true);
			data::query("INSERT INTO GlobalSettings VALUES('G_MAX_RECORD_TIME', '900')", true);
			$this->data['G_MAX_RECORD_TIME'] = 900;
		}

		if (empty($this->data['G_DATA_SOURCE'])) {
			data::query("DELETE FROM GlobalSettings where parameter = 'G_DATA_SOURCE'", true);
			data::query("INSERT INTO GlobalSettings VALUES('G_DATA_SOURCE', 'live')", true);
			$this->data['G_DATA_SOURCE'] = 'local';
		}

		if (empty($this->data['G_SMTP_EMAIL_FROM'])) {
			data::query("DELETE FROM GlobalSettings where parameter = 'G_SMTP_EMAIL_FROM'", true);
			data::query("INSERT INTO GlobalSettings VALUES('G_SMTP_EMAIL_FROM', 'donotreply@bluecherryserver')", true);
			$this->data['G_SMTP_EMAIL_FROM'] = 'donotreply@bluecherryserver';
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

class Manufacturers
{

      const API_URL = 'https://cam-api.bluecherry.workers.dev/manufacturers?fields[]=name';

    public static function getList()
    {
        global $global_settings;
        $list = array();
        if($global_settings->data['G_DATA_SOURCE'] == 'live') {
            $data = @file_get_contents(self::API_URL);
            if($data) {
                $data = json_decode($data, true);

                                foreach($data['records'] as $manufacturer) {
                                        $list[$manufacturer['fields']['name']] = $manufacturer['fields']['name'];
                }
            }

        } else {
            $adapter = getReadOnlyDb();
            $list = $adapter->query("
                SELECT manufacturers.manufacturer FROM manufacturers
                LEFT JOIN cameras ON (cameras.manufacturer=manufacturers.id)
                WHERE cameras.mjpeg_url<>'' OR cameras.h264_url<>''
                GROUP BY manufacturers.manufacturer
                ");
            $list = $list->fetchAll(PDO::FETCH_ASSOC);
        }
        asort($list);
        return $list;
    }
}

class Cameras
{
//    const API_SEARCH_URL = 'https://api.evercam.io/v1/models?page=%d&vendor_id=%s';
      const API_SEARCH_URL = 'https://cam-api.bluecherry.workers.dev/models?filterByFormula=={Manufacturer}="%s"';

    const API_DETAILS_URL = 'https://api.evercam.io/v1/models/%s';

    public static function getList($manufacturer)
    {
        global $global_settings;
        $list = array();
        if($global_settings->data['G_DATA_SOURCE'] == 'live') {

            $offset = NULL;
            do {
                                $url = sprintf(self::API_SEARCH_URL, rawurlencode($manufacturer));
                                if($offset)
                                {
                                        $url .= "&offset=".$offset;

                                        // echo $url;
                                        // exit;
                                }

                $data = @file_get_contents($url);
                if(!$data) {
                    break;
                }
                                $data = json_decode($data, true);

                // echo "<pre>";
                                // print_r($data);

                                foreach($data['records'] as $camera) {
                    $list[rawurlencode($manufacturer).' - '.$camera['id']] = $camera['fields']['Model'];
                                }

                                if(isset($data['offset']))
                                {
                                        $offset = $data['offset'];
                                }

                        } while(isset($data['offset']));

                        // print_r($list);
                        // exit;


        } else {
            $adapter = getReadOnlyDb();
            $list = $adapter->prepare(
                'SELECT api_id, model ' .
                'FROM cameras AS C ' .
                'JOIN manufacturers AS M ON C.manufacturer = M.id ' .
                'WHERE M.manufacturer = ? ' .
                'ORDER BY model ASC'
            );
            $list->execute(array($manufacturer));
            $list = $list->fetchAll(PDO::FETCH_KEY_PAIR);
        }
        return $list;
    }

    public static function getCamDetails($details)
    {
        global $global_settings;
        if($global_settings->data['G_DATA_SOURCE'] == 'live') {

                        $array = explode(" - ",$details);
                        $manufacturer = $array[0];
                        $id = $array[1];

                        // echo "<pre>";
                        // print_r($details);
                        // exit;

                        // $url = sprintf(self::API_SEARCH_URL, rawurlencode($manufacturer ));

                        // $data = @file_get_contents($url);

            // if(!$data) {
            //     return '';
                        // }

                        // // echo "<pre>";
                        // // print_r($data);
                        // // exit;

                        // $data = json_decode($data, true);

                        $flag = 0;
                        $offset = NULL;
            do {
                                $url = sprintf(self::API_SEARCH_URL, rawurlencode($manufacturer));
                                if($offset)
                                {
                                        $url .= "&offset=".$offset;

                                        // echo $url;
                                        // exit;
                                }

                $offset_data = @file_get_contents($url);
                if(!$offset_data) {
                    break;
                                }

                                $offset_data = json_decode($offset_data, true);

                                foreach($offset_data['records'] as $key => $camera) {
                                        if ($camera['id'] == $id) {
                                                $data = $camera['fields'];
                                                $flag = 1;
                                                break;
                                        }
                                }

                                if(isset($offset_data['offset']) && $flag == 0)
                                {
                                        $offset = $offset_data['offset'];
                                }
                                else
                                {
                                        $offset = NULL;
                                }
                        } while(isset($offset));


                        // echo "<pre>";
                        // print_r($data);
                        // exit;

                        // array_walk($data['models'][0], array('Cameras', 'sanitize'));

                        // $data = $data['models'][0];

        } else {
            $adapter = getReadOnlyDb();
            $stmt = $adapter->prepare(
                'SELECT C.*, M.manufacturer AS manufacturer_id ' .
                'FROM cameras AS c ' .
                'JOIN manufacturers AS M ON c.manufacturer = M.id ' .
                'WHERE api_id = ? '
            );
            $stmt->execute(array($id));
            $data = $stmt->fetch(PDO::FETCH_ASSOC);
        }

        // if (isset($data['vendor_id'])) {
        //     $data['rtsp_port'] = strcasecmp('acti', $data['vendor_id']) === 0 ? 7070 : 554;
        // } else {
        //     $data['rtsp_port'] = strcasecmp('acti', $data['manufacturer_id']) === 0 ? 7070 : 554;
                // }


                if (isset($data['ManufacturerLink'][0])) {
            $data['rtsp_port'] = strcasecmp('acti', $data['ManufacturerLink'][0]) == '' ? 7070 : 554;
        }

        if(!empty($data['MJPEG path'])) {
            $data['mjpeg_path'] = '/' . ltrim($data['MJPEG path'], '/');
        }

        if(!empty($data['RTSP path'])) {
            $data['rtsp_path'] = '/' . ltrim($data['RTSP path'], '/');
        }

        //$data = "
                //<camName><![CDATA[{$data['model']}]]></camName>
                //<mjpegPath><![CDATA[{$data['mjpeg_url']}]]></mjpegPath>
                //<rtspPath><![CDATA[{$data['h264_url']}]]></rtspPath>
                //<mjpegPort><![CDATA[80]]></mjpegPort>
                //<rtspPort><![CDATA[{$data['rtsp_port']}]]></rtspPort>
                //<resolutions><![CDATA[{$data['resolution']}]]></resolutions>
                //<user><![CDATA[{$data['default_username']}]]></user>
                //<pass><![CDATA[{$data['default_password']}]]></pass>
                //";

        $data_r = Array(
            'camName' => (isset($data['Model']) ? $data['Model'] : ''),
            'mjpegPath' => (isset($data['mjpeg_path']) ? $data['mjpeg_path'] : ''),
            'rtspPath' => $data['rtsp_path'],
            'mjpegPort' => 80,
            'rtspPort' => $data['rtsp_port'],
            'resolutions' => '',
            'user' => (isset($data['Default username']) ? $data['Default username'] : ''),
            'pass' => (isset($data['Default password']) ? $data['Default password'] : ''),
                );

        data::responseJSON(true, true, $data_r);
    }

    /**
     * Function to be used as array_walk callback for preparing data
     * 
     * 
     * @param mixed $value The array element value
     * @param mixed $key   The array element key
     * 
     */
    
    public static function sanitize(&$value, $key)
    {
        if(in_array(
            $key, 
            array('jpeg_url', 'h264_url', 'mjpeg_url', 
                  'default_username', 'default_password')
        )) {
            $value = str_replace(
                array('Unknown', 'unknown', '<blank>', 'blank', 'none', 'None', 
                      'n/a', 'User defined', 'user defined'), 
                '', 
                $value
            );
        }
    }
}

$global_settings = new globalSettings;
$varpub = VarPub::get();
$varpub->global_settings = $global_settings;

?>
