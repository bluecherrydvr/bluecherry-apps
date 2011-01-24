<?php DEFINE('INDVR', true);

#libs
include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check

class updateDB extends DVRData{
	public $message;
	public $status;
	public $data;
	function __construct(){
		$this->message = CHANGES_FAIL;
		$mode = $_POST['mode']; unset($_POST['mode']);
		switch ($mode) {
			case 'global': $this->status = $this->Edit('global'); $this->message = ($this->status) ? CHANGES_OK : CHANGES_FAIL; break;
			case 'deleteUser' : $this->status = $this->deleteUser(); break;
			case 'updateEncoding': $this->updateEncoding(); break;
			case 'update': $this->updateField(); break;
			case 'user': $this->editUser(); break;
			case 'newUser' : $this->newUser(); break;
			case 'changeState': $this->changeState(); break;
			case 'changeStateIp': $this->changeStateIp(); break;
			case 'FPS': $this->changeFPSRES('FPS'); break;
			case 'RES': $this->changeFPSRES('RES'); break;
			case 'update_control' : $this->update_control(); break;
			case 'ban': $this->BanUser(); break;
			case 'kick': $this->KickUser(); break;
			case 'addip': $this->status = $this->addIp(); break;
		}
	}
	private function changeStateIp(){
		$db = DVRDatabase::getInstance();
		$device = $db->DBFetchAll("SELECT disabled FROM Devices WHERE id={$_POST['id']}");
		$this->status = $db->DBQuery("UPDATE Devices SET disabled=".(($device['disabled']) ? 0 : 1)." WHERE id={$_POST['id']}");
		$this->message = ($this->status) ? CHANGES_OK : CHANGES_FAIL;
	}
	private function addIp(){
		if (!$_POST['ipAddr']){ $this->message = AIP_NEEDIP;  return false;};
		if (!$_POST['port']){ $this->message = AIP_NEEDPORT; return false;};
		if (!$_POST['mjpeg']){ $this->message = AIP_NEEDMJPEG; return false;};
		$db = DVRDatabase::getInstance();
		$model_info = $db->DBFetchAll("SELECT driver FROM ipCameras WHERE model='{$_POST['models']}'");
		$t = $db->DBQuery("INSERT INTO Devices (device_name, protocol, device, driver, rtsp_username, rtsp_password, resolutionX, resolutionY) VALUES ('{$_POST['ipAddr']}', 'IP', '{$_POST['ipAddr']}|{$_POST['port']}|{$_POST['mjpeg']}', '{$model_info[0]['driver']}', '{$_POST['user']}', '{$_POST['pass']}', 640, 480)");
		$this->message = ($t) ? AIP_CAMADDED : false;
		return ($t) ? true : false;
	}
	private function KickUser(){
		$ip = preg_replace("/[^(0-9)\.]/", "", $_POST['id']);
		$db = DVRDatabase::getInstance();
		if ($_SERVER['REMOTE_ADDR']!=$ip){
			$this->status = $db->DBQuery("UPDATE ActiveUsers SET kick = 1 WHERE ip='$ip'");
			$this->message = ($this->status) ? AU_KICKED : CHANGES_FAIL;
		} else {
			$this->status = false;
			$this->message = AU_CANT_EOS;
		}
	}
	private function BanUser(){
		$id = intval($_POST['id']);
		$user = new DVRUser('id', $id);
		if ($_SESSION['id']!=$id){
			$user->ban();
			$this->status = true;
			$this->message = AU_BANNED;
		} else {
			$this->status = false;
			$this->message = AU_CANT_SB;
		}
	}
	/* XXX Check for errors here */
	function update_control(){
		$bch = bc_handle_get_byid(intval($_POST['id']));
		if ($bch == false) {
			$this->status = false;
			return;
		}
		if (isset($_POST['hue'])) { bc_set_control($bch, BC_CID_HUE, intval($_POST['hue'])); };
		if (isset($_POST['saturation'])) { bc_set_control($bch, BC_CID_SATURATION, intval($_POST['saturation'])); };
		if (isset($_POST['contrast'])) { bc_set_control($bch, BC_CID_CONTRAST, intval($_POST['contrast'])); };
		if (isset($_POST['brightness'])) { bc_set_control($bch, BC_CID_BRIGHTNESS, intval($_POST['brightness'])); };
		bc_handle_free($bch);
		$this->updateField();
	}
	
	function changeFPSRES($type){
		$id = intval($_POST['id']);
		$db = DVRDatabase::getInstance();
		$this_device = $db->DBFetchAll("SELECT * FROM Devices LEFT OUTER JOIN AvailableSources USING (device) WHERE Devices.id='$id'");
		if ($type == 'RES'){ $res = explode('x', $_POST['value']); $res['x'] = intval($res[0]); $res['y'] = intval($res[1]); } else {
			$res['x'] = $this_device[0]['resolutionX']; $res['y'] = $this_device[0]['resolutionY']; 
		}
		$fps = ($type=='FPS') ? intval($_POST['value']) : (30/$this_device[0]['video_interval']);
		$resX = ($type=='RES') ? ($res['x']) : $this_device[0]['resolutionX'];
		
		$this_device[0]['req_fps'] = (($fps) * (($resX>=704) ? 4 : 1)) - ((30/$this_device[0]['video_interval']) * (($this_device[0]['resolutionX']>=704) ? 4 : 1));
		
		$container_card = new BCDVRCard($this_device[0]['card_id']);
		if ($this_device[0]['req_fps'] > $container_card->fps_available){
			$this->status = false;
			$this->message = ENABLE_DEVICE_NOTENOUGHCAP;
		} else {
			$this->status = $db->DBQuery("UPDATE Devices SET video_interval='".intval(30/$fps)."', resolutionX='{$res['x']}', resolutionY='{$res['y']}' WHERE id='$id'");
			$this->message = ($this->status) ? CHANGES_OK : CHANGES_FAIL;
			$container_card = new BCDVRCard($this_device[0]['card_id']);
			$this->data = $container_card->fps_available;
		}
	}

	function changeState(){
		$db = DVRDatabase::getInstance();
		$device = $db->DBEscapeString($_POST['id']);
		$this_device = $db->DBFetchAll("SELECT * FROM AvailableSources LEFT OUTER JOIN Devices USING (device) WHERE AvailableSources.device='$device' ");
		if (!$this_device) {
			$this->status = false;
			$this->message = CHANGES_FAIL;
			return;
		}
		$container_card = new BCDVRCard($this_device[0]['card_id']);
		if (!empty($this_device[0]['protocol'])){ //if the device is configured
			$this_device[0]['req_fps'] = (30/$this_device[0]['video_interval']) * (($this_device[0]['resolutionX']>=704) ? 4 : 1);
			if ($this_device[0]['disabled']){
				if ($this_device[0]['req_fps'] > $container_card->fps_available){
					$this->status = false;
					$this->message = ENABLE_DEVICE_NOTENOUGHCAP;
				} else {
					$this->status = $db->DBQuery("UPDATE Devices SET disabled='0' WHERE device='{$this_device[0]['device']}'");
					$this->message = ($this->status) ? CHANGES_OK : CHANGES_FAIL;
				}

			} else {
				$this->status = $db->DBQuery("UPDATE Devices SET disabled='1' WHERE device='{$this_device[0]['device']}'");
				$this->message = ($this->status) ? CHANGES_OK : CHANGES_FAIL;
			}
		} else {
			$ds = ($container_card->fps_available<2) ? 1 : 0;
			if ($container_card->signal_type == 'notconfigured' || $container_card->signal_type == 'NTSC'){
				$res['y']='240';
				$enc = 'NTSC';
			} else {
				$res['y'] = '288';
				$enc = 'PAL';
			}
			$card_info = explode('|', $this_device[0]['device']);
			$card_info[2]++;
			$this->status = $db->DBQuery("INSERT INTO Devices (device_name, resolutionX, resolutionY, protocol, device, driver, video_interval, signal_type, disabled) VALUES ('Port {$card_info[2]} on Card {$this_device[0]['card_id']}', 352, {$res['y']}, 'V4L2', '{$this_device[0]['device']}', '{$this_device[0]['driver']}', 15, '{$enc}', '$ds')");
			if ($ds==1) { $this->status = 'INFO'; $this->message = NEW_DEV_NEFPS; } else {
				$this->message = ($this->status) ? CHANGES_OK : CHANGES_FAIL;
			}
		}
		
	}
	function newUser(){
		if (empty($_POST['username'])) { $this->status = false; $this->message = NO_USERNAME; return false; }
		if (empty($_POST['email'])) { $this->status = false; $this->message = NO_EMAIL; return false; }
		if (empty($_POST['password'])) { $this->status = false; $this->message = NO_PASS; return false; }
		$_POST['type'] = 'Users';
		$_POST['access_setup'] = ($_POST['access_setup']=='on') ? '1' : '0';
		$_POST['access_web'] = ($_POST['access_web']=='on') ? '1' : '0';
		$_POST['access_remote'] = ($_POST['access_remote']=='on') ? '1' : '0';
		$_POST['access_backup'] = ($_POST['access_backup']=='on') ? '1' : '0';
		$_POST['salt'] = genRandomString(4);
		$_POST['password'] = md5($_POST['password'].$_POST['salt']);
		$this->status = $this->FormQueryFromPOST('insert');
		$this->message = ($this->status) ? USER_CREATED : CHANGES_FAIL;
	}
	
	function editUser(){
		$id = intval($_POST['id']);
		$db = DVRDatabase::getInstance();
		if ($_POST['password']=='__default__') { unset($_POST['password']); }
		 else { 
		 	$tmp = $db->DBFetchAll("SELECT salt FROM Users WHERE id='$id'");
			$_POST['password'] = md5($_POST['password'].$tmp[0]['salt']);
		};
		$_POST['type'] = 'Users';
		$_POST['access_setup'] = ($_POST['access_setup']=='on') ? '1' : '0';
		$_POST['access_web'] = ($_POST['access_web']=='on') ? '1' : '0';
		$_POST['access_remote'] = ($_POST['access_remote']=='on') ? '1' : '0';
		$_POST['access_backup'] = ($_POST['access_backup']=='on') ? '1' : '0';
		if (empty($_POST['username'])) { $this->status = false; $this->message = NO_USERNAME; return false; }
		if (empty($_POST['email'])) { $this->status = false; $this->message = NO_EMAIL; return false; }
		if ($_SESSION['id']==$_POST['id'] && $_POST['access_setup']==0) { $this->message = CANT_REMOVE_ADMIN; return false; }
		$this->status = $this->FormQueryFromPOST('update');
		$this->message = ($this->status) ? CHANGES_OK : CHANGES_FAIL;
	}
	
	function deleteUser(){
		$id = intval($_POST['id']);
		if ($_SESSION['id']==$id) { $this->message = DELETE_USER_SELF; return false; }
		$db = DVRDatabase::getInstance();
		$this->status = $db->DBQuery("DELETE FROM Users WHERE id='$id'");
		$this->message = ($this->status) ? USER_DELETED : CHANGES_FAIL;
		return true;
	}
	
	function updateEncoding(){
		$db = DVRDatabase::getInstance();
		$card_id = $_POST['id'];
		$signal_type = $db->DBEscapeString($_POST['signal_type']);
		if ($signal_type=='NTSC'){ $resolution_full = 480; $resolution_quarter = 240; } else { $resolution_full = 576; $resolution_quarter = 288; };
		
		$this->status = $db->DBQuery("UPDATE Devices SET signal_type='{$signal_type}' WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$card_id}')");
		$this->status = $db->DBQuery("UPDATE Devices SET resolutionY=$resolution_full WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$card_id}') AND resolutionY>300");
		$this->status = $db->DBQuery("UPDATE Devices SET resolutionY=$resolution_quarter WHERE device IN (SELECT device FROM AvailableSources WHERE card_id='{$card_id}') AND resolutionY<300");
		$this->message = ($this->status) ? DEVICE_ENCODING_UPDATED : DB_FAIL_TRY_LATER;
	}
	
	function updateField(){
		$this->status = $this->FormQueryFromPOST('update');
		$this->message = ($this->status) ? CHANGES_OK : CHANGES_FAIL;
	}
	
	function outputXML(){
		switch ($this->status){
			case true: $s = 'OK';    break;
			case false: $s = 'F';    break;
			case 'INFO': $s= 'INFO'; break;
		}
		header('Content-type: text/xml');
		echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
				<response>
					<status>{$s}</status>
					<msg>{$this->message}</msg>
					<data>{$this->data}</data>
				</response>";
				
	}

}

$update = new updateDB;
$update->outputXML();
?>
