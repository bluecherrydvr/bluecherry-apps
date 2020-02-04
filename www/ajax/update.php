<?php 

class update extends Controller {
	public $message;
	public $status;
	public $data;
    private $result;

	function __construct(){
        parent::__construct();
        $this->chAccess('admin');
	}

    public function postData()
    {
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
		data::responseJSON($result);
	}
	private function newUser(){
		$result = user::update($_POST, true);
		data::responseJSON($result[0], $result[1], $result[2]);
	}
	private function updateUser(){
		$result = user::update($_POST);
		data::responseJSON($result[0], $result[1], $result[2]);
	}
	private	function update_control(){
		$device = device(intval($_POST['id']));
		if (empty($device)) {
			data::responseJSON(false);
			return;
		}

		$device->setControls($_POST);
		$this->update();
	}
	private function update(){
		$table = $_POST['type']; unset($_POST['type']);
		$id = $_POST['id']; unset($_POST['id']);
		$query = data::formQueryFromArray('update', $table, $_POST, 'id', $id);
		data::responseJSON(data::query($query, true));
	}
	private function deleteUser(){
        $msg = '';
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
		data::responseJSON(ipCamera::remove($id));
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

		if (stristr($_POST['G_VAAPI_DEVICE'], 'autodetect'))
			$_POST['G_VAAPI_DEVICE'] = autoDetectVaapiSetup();

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
		data::responseJSON($status, $result);
	}
    private function editAccessList(){
        $cams = Inp::gp_arr('cams');
        $cam_ids = Array();
        if (!empty($cams)) {
            foreach ($cams as $key => $val) {
                $cam_ids[] = $key;
            }
        }
        $cam_ids = implode(',', $cam_ids);
        
		$status = data::query("UPDATE Users SET access_device_list='".$cam_ids."' WHERE id='{$_POST['id']}'", true);
		data::responseJSON($status);
	}
	function changeFpsRes($type){
		$camera = new camera($_POST['id']);
		$result = $camera->changeResFps($type, $_POST['value']);
		data::responseJSON($result[0], $result[1], $result[2]);
	}
	
	private function changeState(){
		$id = intval($_POST['id']);
		if (!$id) {
			$camera = new camera($_POST['id']);
		} else {
			$camera = device($id);
		}
		
        $result = $camera->changeState();
        if (!isset($result[1])) $result[1] = '';

		data::responseJSON($result[0], $result[1]);
	}
	
	private function changeEncoding(){
		$id = intval($_POST['id']);
		$card = new card($id);
		$result = $card->changeEncoding();
		data::responseJSON($result[0], $result[1]);
	}

}

