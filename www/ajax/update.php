<?php DEFINE('INDVR', true);

#libs
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
class update{
	public $message;
	public $status;
	public $data;
	private $result;
	function __construct(){
		$this->message = CHANGES_FAIL;
		$mode = $_POST['mode']; unset($_POST['mode']);
		$result = '';
		switch ($mode) {
			case 'global':	$this->updateGlobal(); break;
			case 'kick':	$this->kickUser(); break;
			case 'deleteIp': $this->deleteIp(); break;
			case 'access_device_list': $this->editAccessList(); break;
			case 'changeStateIp': $this->changeState(); break;
			case 'FPS': $this->changeFpsRes('FPS'); break;
			case 'RES': $this->changeFpsRes('RES'); break;
			case 'deleteUser' : $this->deleteUser(); break;
			case 'update': $this->update(); break;
			case 'update_control' : $this->update_control(); break;
			case 'newUser': $this->newUser(); break;
			case 'user': $this->updateUser(); break;
			case 'changeState': $this->changeState(); break;
			case 'updateEncoding': $this->changeEncoding(); break;
			case 'enableAll': $this->enableAll(); break;
			case 'deleteIpPtzPreset': ipPtzPreset::remove(intval($_POST['id'])); break;
		}
		if (!empty($this->result)){
			data::responseXml($this->result[0], $this->result[1]);
		}
	}
	private function enableAll(){
		if (empty($_POST['card_id']) && $_POST['card_id']!=0) { $result = false; } else {
			$card_id = $_POST['card_id'];
			$card = new card($card_id);
			if ($card->info['encoding'] != 'notconfigured') { $result =  false; echo 'nncf';} else {
				$result = $card->enableAllPorts();
			} 
		};
		data::responseXml($result);
	}
	private function newUser(){
		$result = user::update($_POST, true);
		data::responseJSON($result[0], $result[1], $result[2]);
	}
	private function updateUser(){
		$result = user::update($_POST);
		data::responseXml($result[0], $result[1], $result[2]);
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
		if (isset($_POST['video_quality'])) { bc_set_control($bch, BC_CID_VIDEO_QUALITY, intval($_POST['video_quality'])); };

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
		data::responseJSON($result, $msg);
	}
	private function deleteIp(){
		$id = intval($_POST['id']);
		data::responseXml(ipCamera::remove($id));
	}
	private function updateGlobal(){
		$status = true;
		if (empty($_POST['G_DEV_SCED'])){
			$_POST['G_DISABLE_VERSION_CHECK'] = (empty($_POST['G_DISABLE_VERSION_CHECK'])) ? 1 : 0;
			$_POST['G_DISABLE_IP_C_CHECK'] = (!empty($_POST['G_DISABLE_IP_C_CHECK'])) ? 1 : 0;
			$_POST['G_DISABLE_WEB_STATS'] = (!empty($_POST['G_DISABLE_WEB_STATS'])) ? 1 : 0;
		};

		if (!empty($_POST['G_MAX_RECORD_AGE']))  # this catches all except "" and "0"
			$_POST['G_MAX_RECORD_AGE'] *= 60 * 60 * 24;  # convert days to seconds

		foreach ($_POST as $parameter => $value){
			if (!preg_match("/G_.+/", $parameter))
				continue;
			$status = (data::query("INSERT INTO GlobalSettings (parameter, value) VALUES ('{$parameter}', '{$value}') ON DUPLICATE KEY UPDATE value='{$value}'", true)) ? $status : false;
		}

        data::responseJSON($status);
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
		$id = intval($_POST['id']);
		if (!$id) {
			$camera = new camera($_POST['id']);
		} else {
			$camera = device($id);
		}
		
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
