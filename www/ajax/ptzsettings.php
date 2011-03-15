<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check


####BORROWED FROM LIB####
class camera extends DVRData{
	public $data;
	public $ptz_config;
	public function __construct($camId){
		$id = intval($camId);
		$this->data = $this->GetObjectData('Devices', 'id', $id);
		$this->data = $this->data[0];
		$this->ptz_config = $this->getPtzSettings();
	}
	private function getPtzSettings(){
		if (empty($this->data['ptz_control_path'])){
			$ptz_config['ptz'] = false;
		} else {
			$ptz_config['path'] = $this->data['ptz_control_path'];
			$ptz_config['values'] = $this->data['ptz_serial_values'];
			$config = explode(",", $this->data['ptz_control_protocol']);
			$ptz_config['baud'] = $config[0];
			$ptz_config['bit'] = $config[1];
			switch($config[2]){
				case 'n': $ptz_config['pairity'] = 'None'; break;
				case 'e': $ptz_config['pairity'] = 'Even'; break;
				case 'o': $ptz_config['pairity'] = 'Odd';  break;
			}
			$ptz_config['stop_bit'] = $config[3];
			return $ptz_config;
		}
	}
	public static function updatePtzSettings($path, $values, $baud, $bit, $parity, $stop_bit, $id){
		switch($parity){
				case 'None': $pairty = 'n'; break;
				case 'Even': $pairty = 'e'; break;
				case 'Odd':  $pairty = 'o'; break;
		};
		$status = self::query("UPDATE Devices SET ptz_control_path='{$path}', ptz_control_protocol='{$baud},{$bit},{$pairty},{$stop_bit}', ptz_serial_values='$values' WHERE id='{$id}'", true);
		self::outputXml($status);
	}
	private function query($query, $resultless = false){
		$db = DVRDatabase::getInstance();
		return ($resultless) ? $db->DBQuery($query) : $db->DBFetchAll($query);
	}
	function outputXml($status, $message = '', $data = ''){
		header('Content-type: text/xml');
		$status = ($status) ? 'OK' : 'F';
		echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
				<response>
					<status>{$status}</status>
					<msg>{$message}</msg>
					<data>{$data}</data>
				</response>";
		exit;
	}
}

################


if (!empty($_POST)){
	camera::updatePtzSettings($_POST['path'], $_POST['values'], $_POST['baud'], $_POST['bit'], $_POST['parity'], $_POST['stop_bit'], $_POST['id']);
};

$this_camera = new camera($_GET['id']);

include_once('../template/ajax/ptzsettings.php');


?>