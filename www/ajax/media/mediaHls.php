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
		$message = '';

        if (!isset($_GET['id']) or !$this->user->camPermission($_GET['id'])) {
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

            session_write_close();

		    if (!empty($_GET['tokenOnly'])) {
		        return json_encode([
		            'success' => true,
                    'token' => $this->createHlsToken()
                ]);
            }

			$id = intval($_GET['id']);
			$message = $this->createLink($id);
		}

		data::responseJSON($status, $message);
    }

    function createLink($id) {

        // Generate auth token for DB, insert into DB

        $hostname = $_GET['hostname'];
        $port = $_GET['port'];

        $token = $this->createHlsToken();
        return "https://$hostname:$port/hls/$id/0/playlist.m3u8?authtoken=$token";
    }

    function createHlsToken() {
        $token = session_id();  // Lazy way of unique token generation

        $user_id = $this->user->info['id'];
        data::query("INSERT INTO HlsAuthTokens (user_id, token, date) VALUES ($user_id, '$token', now()) ON DUPLICATE KEY UPDATE date = now()", true);
        // Cleanup old tokens from DB
        data::query("DELETE FROM HlsAuthTokens WHERE date < now() - INTERVAL 5 MINUTE", true);

        return $token;
    }
}


