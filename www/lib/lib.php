<?php 
defined('INDVR') or exit();

session_name(VAR_SESSION_NAME);
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

class DVRDatabase{
	public  $config_file_vars;
	private $db;
	public static $instance;
	
	private function __construct(){
		$this->ReadConfig();
		$this->DBConnect();
	}
	
	public static function getInstance(){
		self::$instance or self::$instance = new DVRDatabase();
		return self::$instance;
	}
	
	public function ReadConfig(){
		$config_file = @fopen(VAR_CONF_PATH, "r") or die(LANG_DIE_COULDNOTOPENCONF);
		while($tmp = fgets($config_file, 4096)){
			$tmp = trim($tmp);
			$tmp[0]=='#' or preg_match("/(.*)[\ ][\=][\ ]*(.*);/", $tmp, $matches);
			$this->config_file_vars[$matches[1]] = trim($matches[2], '"');
			empty($matches);
		}
		
	}
	
	private function DBConnect(){
		switch ($this->config_file_vars["type"]){
			case 0:
				$this->db = new SQLite3($this->config_file_vars["file"]) or die(LANG_DIE_COULDNOTCONNECT.$error); break;
			#support for postgresql(1) and mysql(2) to be added
		}
	}
	
	public function DBEscapeString(&$string){
		switch ($this->config_file_vars["type"]){
			case 0: $string =  $this->db->escapeString($string); return $string; break;
			#support for postgresql(1) and mysql(2) to be added
		}
	}
	
	public function DBQuery($query){
		switch ($this->config_file_vars["type"]){
			case 0: return $this->db->query($query); break;
			#support for postgresql(1) and mysql(2) to be added
		}
	}
	
	public function DBMultipleQuery($query){
		switch ($this->config_file_vars["type"]){
			case 0: return $this->db->exec($query); break;
			#support for postgresql(1) and mysql(2) to be added
		}
	}
	
	public function DBNumRows($resource){
		switch ($this->config_file_vars["type"]){
			case 0: return $this->DBFetchAll($resource, true); break;
			#support for postgresql(1) and mysql(2) to be added
		}
	}
	
	public function DBFetchArray($resource){
		switch ($this->config_file_vars["type"]){
			case 0: return $resource->fetchArray(SQLITE3_ASSOC); break;
			#support for postgresql(1) and mysql(2) to be added
		}
	}
	
	public function DBFetchAll($resource, $count = false){
		switch ($this->config_file_vars["type"]){
			case 0: $i=0; while ($tmp[$i] = $resource->fetchArray(SQLITE3_ASSOC)) $i++; unset($tmp[$i]); return ($count) ? $i-1 : $tmp; break;
			#support for postgresql(1) and mysql(2) to be added
		}
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
		switch ($_SESSION['l']) {
			case 'admin':  $this->status = 'admin';  break;
			case 'viewer': $this->status = ($this->ScheduleCheck()) ? 'viewer' : 'schedule'; break;
			default: $this->status = 'new';
		};		
	}
	
	public function StatusAction($l){
		if (($l='admin' && $this->status!='admin') || ($l=='viewer' && (!$this->status || $this->status=='schedule'))) die('403');
	}
	
	public function ValidatePassword($entered_password){
		return ($entered_password===md5($this->data['password'])) ? true : false;
	}
	private function ScheduleCheck(){
		return (substr($this->data['schedule'], date('N')*(date('G')+1), 1)=='1')  ? true : false;
	}
}

#unified class for data operations
class DVRData{
	#mode: global - global config (no insertions, update multiple entries) OR update / insert
	public function Edit($mode){
		$db = DVRDatabase::getInstance();
		return ($db->DBMultipleQuery($this->FormQueryFromPOST($mode))) ? true : false;
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
		switch ($mode){
			case 'global': 
				foreach ($_POST as $parameter => $value){
					$db->DBEscapeString($parameter);
					$db->DBEscapeString($value);
					$query .= "UPDATE GlobalSettings SET value = '$value' WHERE parameter='$parameter'; ";
				}
				break;
			case 'insert':
				$query = "INSERT INTO $type ('".implode("', '", array_keys($_POST))."') VALUES ('".implode("', '", $_POST)."') ";
				break;
			case 'update':
				foreach ($_POST as $parameter => $value){
					if ($value=='true') {$value = '1'; } elseif ($value=='false') {$value = '0'; };
					$tmp[$parameter]="'{$db->DBEscapeString($parameter)}'='{$db->DBEscapeString($value)}'";
				}
				
				$query = "UPDATE $type SET ".implode(", ", $tmp)." WHERE id='$id'";
				break;
		}
		return $query;
	}
	
	public static function GetObjectData($type, $parameter = false, $value = false, $condition = '='){
		$db = DVRDatabase::getInstance();
		$tmp = $db->DBQuery("SELECT * FROM {$db->DBEscapeString($type)}".((!$parameter) ? '' : " WHERE {$db->DBEscapeString($parameter)} ".$condition." '{$db->DBEscapeString($value)}'"));
		$tmp = $db->DBFetchAll($tmp);
		return (!$tmp) ? false : $tmp;
	}
	
	
}

class DVRDevices extends DVRData{
	public $number_of_card;
	public $cards;
	public $ip_cameras;
	public function __construct(){
		$id = (isset($_GET['id'])) ? intval($_GET['id']) : false;
		$this->getCards();
		$this->getIpCameras();
	}

	private function getCards(){
		$db = DVRDatabase::getInstance();
		$tmp = $db->DBFetchAll($db->DBQuery("SELECT * FROM AvailableSources GROUP BY card_id"));
		$this->number_of_cards = count($tmp);
		if (!tmp) { return false; };
		foreach ($tmp as $key => $card){
			$this->cards[$card['card_id']] = new BCDVRCard($card['card_id']);
			
		}
	}
	private function getIpCameras(){
		$db = DVRDatabase::getInstance();
		$this->ip_cameras = $db->DBFetchAll($db->DBQuery("SELECT * FROM Devices WHERE source_video NOT LIKE '/dev/video%'"));
	}
	public function MakeXML(){
		$xml = '<?xml version="1.0" encoding="UTF-8" ?><devices>';
		foreach($this->cards as $card_id => $card){
			foreach($card->devices as $device_id => $device){
				$xml .= '<device';
				$xml .= empty($device['id']) ? '>' : ' id="'.$device['id'].'">';
				foreach($device as $property => $value){
					$xml.="<$property>$value</$property>";
				};
				$xml.='</device>';
			}
		};
		foreach($this->ip_cameras as $camera_id => $device){
			$xml.='<device>';
			foreach($device as $property => $value){
				$xml.="<$property>$value</$property>";
			};
			$xml.='</device>';
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
		$this->getCardInfo($id);
	}

	private function getCardInfo(){
		$this->fps_available = 480; //BC card capacity
		$db = DVRDatabase::getInstance();
		$tmp = $db->DBQuery("SELECT * FROM AvailableSources WHERE card_id='{$this->id}' ORDER BY id DESC");
		$this->devices = $db->DBFetchAll($tmp);
		$this->type = count($this->devices);
		$port = 1;
		foreach ($this->devices as $key => $device){
			$this->devices[$key]['as_id'] = $this->devices[$key]['id'];
			$tmp = $db->DBFetchArray($db->DBQuery("SELECT * FROM Devices WHERE source_video='{$device['devicepath']}'"));
			$this->devices[$key]['port'] = $port; $port++;
			if (!$tmp) { $this->devices[$key]['status'] = 'notconfigured'; $this->devices[$key]['id']=''; }
			 	else {
					$this->devices[$key] = array_merge($this->devices[$key], $tmp);
					$this->devices[$key]['status'] = ($this->devices[$key]['disabled']) ? 'disabled' : 'OK';
					
				}
			(!$this->devices[$key]['video_interval'] || $this->devices[$key]['disabled']) or $this->fps_available -= (30/$this->devices[$key]['video_interval']) * (($this->devices[$key]['resolutionX']>=704) ? 4 : 1);
		}
		$this->signal_type = ($this->devices[0]['signal_type']) ? $this->devices[0]['signal_type'] : 'notconfigured' ; //NTSC is the default
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

?>
