<?php

class mediaMjpeg extends Controller {
	
    public function __construct()
    {
        parent::__construct();
		$this->chAccess('basic');
    }

    public function getData()
    {
        $current_user = $this->user;

        if (!isset($_GET['id']) or !$current_user->camPermission($_GET['id']))
        	image_err(str_replace('%ID%', $_GET['id'], MJPEG_NO_PERMISSION));

        $this_camera = data::getObject('Devices', 'id', $_GET['id']);
        if ($this_camera[0]['disabled'] == true){
        	image_err(MJPEG_DISABLED);
        } elseif (!$this_camera){
        	image_err(str_replace('%ID%', $_GET['id'], MJPEG_DEVICE_NOT_FOUND));
        }
        
        if (!isset($_GET['id']))
        	image_err(MJPEG_DEVICE_ID_NOT_SUPPLIED);
        
        $id = intval($_GET['id']);
        
        header("Cache-Control: no-cache");
        header("Cache-Control: private");
        header("Pragma: no-cache");

        session_write_close();

        $out_multipart = isset($_GET['multipart']);

        // For multi, let's do some weird stuff
        if ($out_multipart) {
        	set_time_limit(0);
        	@apache_setenv('no-gzip', 1);
        	@ini_set('zlib.output_compression', 0);
        }

        print_image($id, $out_multipart, $current_user);

        die();
    }

}

function image_err($msg)
{
	$width = 352;
	$height = 240;
	$font_size = 5;

	$im = imagecreate($width, $height);
	$gray = imagecolorallocate ($im, 25, 25, 25);

	$bg = imagecolorallocate($im, 255, 255, 255);
	imagefilledrectangle($im, 0, 0,$width ,$height , $bg);

	/* Center the text */
	$text_width = imagefontwidth($font_size) * strlen($msg);
        $center = ceil($width / 2);
        $x = $center - ceil($text_width / 2);
        imagestring($im, $font_size, $x, ceil($height / 2), $msg, $gray);

	/* Output as PNG */
	header('Content-Type: image/png');
	imagepng($im);

	imagedestroy($im);

	exit;
}

// auth check
//$current_user = new user('id', $_SESSION['id']);
//$current_user->checkAccessPermissions('basic');


function print_image($id, $out_multipart, $current_user) {
	global $global_settings;
	$transcode = true;

	if ($transcode) {

		// Generate auth token for DB, insert into DB
		$token = session_id();  // Lazy way of unique token generation
		$user_id = $current_user->info['id'];
		data::query("INSERT INTO RtspAuthTokens (user_id, token, date) VALUES ($user_id, '$token', now()) ON DUPLICATE KEY UPDATE date = now()", true);
		// Cleanup old tokens from DB
		data::query("DELETE FROM RtspAuthTokens WHERE date < now() - INTERVAL 5 MINUTE", true);

		// Launch ffmpeg with token as HTTP parameter

		$hwaccel = "";
		$hwfilter = "";

		$device = data::getObject('Devices', 'id', $id);
		if ($device[0]['protocol'] == 'IP-MJPEG') {
			$codec = ' copy ';
		} else {
			$codec = ' mjpeg ';
			//$transcode_fps = 5;
			//$codec .= ' -r ' . $transcode_fps . ' ';
			$transcode_quality = 5;
			$codec .= ' -q:v ' . $transcode_quality . ' ';

			$vaapi_device = $global_settings->data['G_VAAPI_DEVICE'];
			if (strcasecmp($vaapi_device, "none") != 0)
			{
				$codec = " mjpeg_vaapi -r 5 -global_quality 90 -jfif 1 ";
				$hwaccel = " -init_hw_device vaapi=hwva:$vaapi_device -hwaccel vaapi -hwaccel_output_format vaapi -hwaccel_device hwva ";
				$hwfilter = " -filter_hw_device hwva -vf 'format=nv12|vaapi,hwupload' ";
			}
		}
		$frames = ($out_multipart
				? ''
				: ' -vframes 1 ');

		$cmd = "LD_LIBRARY_PATH=/usr/lib/bluecherry/ /usr/lib/bluecherry/ffmpeg "
			. $hwaccel
			. " -rtsp_transport tcp -i rtsp://127.0.0.1:7002/live/$id?authtoken=$token "
			. " -aspect 1/1 " # forces JFIF marker which is used for longer marker
			. $hwfilter
			. " -an -vcodec " . $codec . $frames ." -f rawvideo - ";

		// Parse out individual frames and insert the boundary
		// to comply Content-Type: multipart/x-mixed-replace
		$start_marker = pack('H*', 'ffd8ffe00010') . 'JFIF';
		$marker_len = strlen($start_marker);

		if ($out_multipart) {
			$boundary = "--boundary";
			header("Content-Type: multipart/x-mixed-replace; boundary=$boundary");
			$stream = popen($cmd, "r");
			$buffer = "";
			if (!$stream)
				die("Failed to open command output stream");
			while (true) {
				while (true) {
					if (connection_aborted()
							|| ($chunk = fread($stream, 1024)) === false
							|| feof($stream))
						break(2);
					$buffer .= $chunk;
					if (strlen($buffer) < $marker_len)
						continue;
					$frame_start = strpos($buffer, $start_marker, $marker_len);
					if ($frame_start !== false)
						break;
				}
				$frame = substr($buffer, 0, $frame_start);
				$buffer = substr($buffer, $frame_start);

				echo("$boundary\r\n"
						."Content-Length: ".strlen($frame)."\r\n"
						."Content-Type: image/jpeg\r\n\r\n"
						.$frame);
				flush();
			}
			pclose($stream);
		} else {
			header("Content-Type: image/jpeg");
			passthru($cmd);
		}
	}
}

