<?php defined('INDVR') or exit();

function get_ipcam_control($driver, $info){ #abstraction to return control class depending on the driver
	switch($driver){
		case 'RTSP-ACTi':
			$control = new RTSP_ACTI($info); 
		break;
		case 'RTSP-Vivotek-V1':
			$control = new RTSP_Vivotek($info); 
		break;
		case 'RTSP-Vivotek-V2':
			$control = new RTSP_Vivotek($info); 
		break;
		default:
			$control = false; //no control class for given driver
		break;
		
	}
	return $control;
}

#ACTi control class
class RTSP_ACTI {//extends ipCameraControl{
	const URL_BASE = 'http://%RTSP_USERNAME%:%RTSP_PASSWORD%@%IP%:%PORT%/%FILE%?%OPTIONS%';
	const AUTO_SETUP_SUCCESS = ACTI_STREAMING_SET_3;
	const AUTO_SETUP_FAILED  = ACTI_STREAMING_N_SET_3;
	public function __construct($info){
		$this->info = $info;
	}
	public function auto_configure(){ #this function is to try to automatically set the camera to req'd settings
		$refult = false;
		$result = $this->set_streaming_method(3); #3 for RTP over UDP
		return ($result[0]) ? self::AUTO_SETUP_SUCCESS : self::AUTO_SETUP_FAILED;
	}
	private function construct_url($command){
		return str_replace(
							array('%IP%', '%PORT%', '%RTSP_USERNAME%', '%RTSP_PASSWORD%', '%FILE%', '%OPTIONS%',), 
							array($this->info['ipAddr'], $this->info['portMjpeg'], $this->info['rtsp_username'], $this->info['rtsp_password'], $command['file'], $command['options']),
							self::URL_BASE
		);
	}
	private function send_command($command){
		return @file_get_contents($command);
	}
	private function check_response($response){
		#if response is false -- connection failed
		if (!$response) return array(false, COUND_NOT_CONNECT);
		#if not, check whether change was successful or not
		$std_resp = preg_match("/(OK|ERROR):(.*)/", $response, $response);
		if ($std_resp){
			if ($response[1] == 'OK') { return array(true, ''); }
				elseif ($response[1] == 'ERROR') { return array(false, $response[2]); };
		} else {
			return array(false, COUND_NOT_CONNECT);
		}
	}
	
	public function set_streaming_method($type){
		$command['file'] = 'cgi-bin/cmd/system';
		$command['options'] = 'V2_STREAMING_METHOD='.intval($type);
		$command = $this->construct_url($command);
		return $this->check_response($this->send_command($command));
	}
	public function get_streaming_method(){
	}
}

class RTSP_Vivotek{
	const URL_BASE = 'http://%RTSP_USERNAME%:%RTSP_PASSWORD%@%IP%:%PORT%/%FILE%?%OPTIONS%';
	const AUTO_SETUP_SUCCESS = VIVOTEC_SETRAMING_SET;
	const AUTO_SETUP_FAILED  = VIVOTEC_SETRAMING_N_SET;
	public function __construct($info){
		$this->info = $info;
	}
	private function construct_url($command){ #will be abstracted to lib semi-abstract class in 9.4
		return str_replace(
							array('%IP%', '%PORT%', '%RTSP_USERNAME%', '%RTSP_PASSWORD%', '%FILE%', '%OPTIONS%',), 
							array($this->info['ipAddr'], $this->info['portMjpeg'], $this->info['rtsp_username'], $this->info['rtsp_password'], $command['file'], $command['options']),
							self::URL_BASE
		);
	}
	public function auto_configure(){
		$result = $this->set_streaming_method('mjpeg', 1);
		return ($result[0]) ? self::AUTO_SETUP_SUCCESS : self::AUTO_SETUP_FAILED;
	}
	public function set_streaming_method($type, $encoder){ #type mjpeg/mpeg4, encoder 0/1
		$command['file'] = 'cgi-bin/admin/setparam.cgi';
		$command['options'] = 'videoin_c0_s'.$encoder.'_codectype='.$type;
		$command = $this->construct_url($command);
		return $this->check_response($this->send_command($command));
	}
	private function send_command($command){
		return @file_get_contents($command);
	}
	private function check_response($response){
		#if response is false -- connection failed
		if (!$response || empty($response)) return array(false, COUND_NOT_CONNECT);
		return array(true, '');
	}
}

?>
