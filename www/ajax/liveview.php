<?php

class liveview extends Controller {
	public $devices;
	public $access_list;
	public $layout_list;
	public $layout_values;

    public function __construct(){
        parent::__construct();
		$this->chAccess('viewer');
    }

    public function postLayouts()
    {
        return $this->ctl('layouts')->postData();
    }

    public function getData()
    {
        $this->setView('main_viewer', false);

		$tmp = data::query("SELECT access_device_list FROM Users WHERE id='{$_SESSION['id']}'");
		$this->layout_list = data::query("SELECT layout_name FROM userLayouts WHERE user_id='{$_SESSION['id']}'");
		$this->access_list = explode(',', $tmp[0]['access_device_list']);
		$this->devices = data::query("SELECT Devices.id, Devices.device_name, Devices.ptz_serial_values, Devices.ptz_control_path  FROM Devices INNER JOIN AvailableSources USING (device) WHERE Devices.disabled='0' UNION SELECT Devices.id, Devices.device_name, Devices.ptz_serial_values, Devices.ptz_control_path  FROM Devices WHERE protocol in ('IP-RTSP', 'IP-MJPEG', 'IP') AND disabled='0'");
		foreach ($this->devices as $id => $device){
			$this->devices[$id]['presets'] = data::query("SELECT * FROM PTZPresets WHERE device_id=".$id);
		}
		if (!empty($_SESSION['load_layout'])) $this->loadLayout();


        $lv = new StdClass();
        $lv->devices = $this->devices;
        $lv->access_list = $this->access_list;
        $lv->layout_list = $this->layout_list;
        $lv->layout_values = $this->layout_values;
		$lv->video_method = $this->varpub->global_settings->data['G_LIVEVIEW_VIDEO_METHOD'];

        $this->view->lv = $lv;
    }
        
    private function loadLayout()
    {
		$this->deleteLayoutCookies(); #unset all layout cookies
		$cookies = explode(';', $_SESSION['load_layout']);
		foreach ($cookies as $key => $cookie){
			$cookie = explode(':', $cookie);
			if (!empty($cookie[1]))
				setcookie($cookie[0], $cookie[1]);
		}
		unset($_SESSION['load_layout']);
    }

    private function deleteLayoutCookies()
    {
		foreach($_COOKIE as $key => $value) {
			if (preg_match("/imgSrcy[0-9]*x[0-9]*|lvRows|lvCols/", $key)) setcookie($key, '', 5);
		}
	}
}
