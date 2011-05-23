<?php DEFINE('INDVR', true);

#libs
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');

class update{
	public $message;
	public $status;
	public $data;
	function __construct(){
		$this->message = CHANGES_FAIL;
		$mode = $_POST['mode']; unset($_POST['mode']);
		switch ($mode) {
			case 'global':	$this->updateGlobal(); break;
			case 'kick':	$this->kickUser(); break;
			case 'deleteIp': $this->deleteIp(); break;
			case 'access_device_list': $this->editAccessList(); break;
			case 'changeStateIp': $this->changeStateIp(); break;
			case 'FPS': $this->changeFpsRes('FPS'); break;
			case 'RES': $this->changeFpsRes('RES'); break;
			case 'deleteUser' : $this->status = $this->deleteUser(); break;
			case 'update': $this->update(); break;
			case 'update_control' : $this->update_control(); break;
			case 'newUser': $this->newUser(); break;
			case 'user': $this->updateUser(); break;
			case 'editIp': $this->editIp(); break;
			case 'changeState': $this->changeState(); break;
			case 'updateEncoding': $this->changeEncoding(); break;
			case 'enableAll': $this->enableAll(); break;
		}
	}
	#update functions will be moved to individual files after template/js update in beta7
	private function enableAll(){
		if (empty($_POST['card_id'])) { $result = false; } else {
			$card_id = $_POST['card_id'];
			$card = new card($card_id);
			if ($card->info['encoding'] != 'notconfigured') { $result =  false; } else {
				$result = $card->enableAllPorts();
			}
		};
		data::responseXml($result);
	}
	private function newUser(){
		$result = user::update($_POST, true);
		data::responseXml($result[0], $result[1]);
	}
	private function updateUser(){
		$result = user::update($_POST);
		data::responseXml($result[0], $result[1]);
	}
	private function editIp(){
		$id = intval($_POST['id']);
		$result = data::query("UPDATE Devices SET device='{$_POST['ipAddr']}|{$_POST['port']}|{$_POST['rtsp']}', mjpeg_path='{$_POST['ipAddrMjpeg']}|{$_POST['portMjpeg']}|{$_POST['mjpeg']}', rtsp_username='{$_POST['user']}', rtsp_password='{$_POST['pass']}' WHERE id={$id}", true);
		data::responseXml($result);
	}
	private	function update_control(){
		$id = intval($_POST['id']);
		$this_device = data::query("SELECT * FROM Devices INNER JOIN AvailableSources USING (device) WHERE Devices.id='$id'");
		$bch = bc_handle_get($this_device[0]['device'], $this_device[0]['driver']);
		if (!$bch) { data::responseXml(false); return; }
		if (isset($_POST['hue'])) { bc_set_control($bch, BC_CID_HUE, intval($_POST['hue'])); };
		if (isset($_POST['saturation'])) { bc_set_control($bch, BC_CID_SATURATION, intval($_POST['saturation'])); };
		if (isset($_POST['contrast'])) { bc_set_control($bch, BC_CID_CONTRAST, intval($_POST['contrast'])); };
		if (isset($_POST['brightness'])) { bc_set_control($bch, BC_CID_BRIGHTNESS, intval($_POST['brightness'])); };
		bc_handle_free($bch);
		$this->update();
	}
	private function update(){
		$table = $_POST['type']; unset($_POST['type']);
		$id = $_POST['id']; unset($_POST['id']);
		$query = data::formQueryFromArray('update', $table, $_POST, 'id', $id);
		data::responseXml(data::query($query, true));
	}
	private function deleteUser(){
		$id = intval($_POST['id']);
		if ($id!=$_SESSION['id']){
			$result = user::remove($id);
		} else {
			$result = false;
			$msg = DELETE_USER_SELF;
		}
		data::responseXml($result, $msg);
	}
	private function deleteIp(){
		$id = intval($_POST['id']);
		data::responseXml(ipCamera::remove($id));
	}
	private function changeStateIp(){
		$id = intval($_POST['id']);
		$camera = new ipCamera($id);
		data::responseXml($camera->changeState());
	}
	private function updateGlobal(){
		$status = true;
		foreach ($_POST as $parameter => $value){
			$status = (data::query("UPDATE GlobalSettings SET value='{$value}' WHERE parameter='{$parameter}'", true)) ? $status : false;
			
		}
		data::responseXml($status);
	}
	private function kickUser(){
		$result = user::kick($_POST['id']); 
		if ($result===true){
			$status = true;
			$result = '';
		} else {
			$status = false;
		}
		data::responseXml($status, $result);
	}
	private function editAccessList(){
		$status = data::query("UPDATE Users SET access_device_list='".trim($_POST['value'], ",")."' WHERE id='{$_POST['id']}'", true);
		data::responseXml($status);
	}
	function changeFpsRes($type){
		$camera = new camera($_POST['id']);
		$result = $camera->changeResFps($type, $_POST['value']);
		data::responseXml($result[0], $result[1], $result[2]);
	}
	
	private function changeState(){
		$camera = new camera($_POST['id']);
		$result = $camera->changeState();
		data::responseXml($result[0], $result[1]);
	}
	
	private function changeEncoding(){
		$id = intval($_POST['id']);
		$card = new card($id);
		$result = $card->changeEncoding();
		data::responseXml($result[0], $result[1]);
	}

}

$update = new update;
?>
