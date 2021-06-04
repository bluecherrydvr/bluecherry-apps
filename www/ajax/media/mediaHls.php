<?php

class mediaHls extends Controller {
	
    public function __construct()
    {
        parent::__construct();
		$this->chAccess('basic');
    }

    public function getData()
    {
		$status = true;
		$message = "";

		$current_user = $this->user;

        if (!isset($_GET['id']) or !$current_user->camPermission($_GET['id'])) {
			$status = false;
			$message = str_replace('%ID%', $_GET['id'], MJPEG_NO_PERMISSION);
			$data = "no_permission";
		}

        $this_camera = data::getObject('Devices', 'id', $_GET['id']);
        if ($this_camera[0]['disabled'] == true){
			$status = false;
			$message = MJPEG_DISABLED;
			$data = "disabled";
        } elseif (!$this_camera){
			$status = false;
			$message = str_replace('%ID%', $_GET['id'], MJPEG_DEVICE_NOT_FOUND);
			$data = "not_found";
        }
        
        if (!isset($_GET['id'])) {
			$status = false;
			$message = MJPEG_DEVICE_ID_NOT_SUPPLIED;
			$data = "not_id";
		}

		if ($status === true) {
			$id = intval($_GET['id']);
			session_write_close();
			$message = createLink($id, $current_user);
		}

		data::responseJSON($status, $message);
    }

}

function createLink($id, $current_user) {

	// Generate auth token for DB, insert into DB
	$token = session_id();  // Lazy way of unique token generation
	$user_id = $current_user->info['id'];
	$hostname = $_GET['hostname'];
	$port = $_GET['port'];
	data::query("INSERT INTO HlsAuthTokens (user_id, token, date) VALUES ($user_id, '$token', now()) ON DUPLICATE KEY UPDATE date = now()", true);
	// Cleanup old tokens from DB
	data::query("DELETE FROM HlsAuthTokens WHERE date < now() - INTERVAL 5 MINUTE", true);
	$hls_link = "https://$hostname:$port/hls/$id/index.m3u8?authtoken=$token";

	return $hls_link;
}
