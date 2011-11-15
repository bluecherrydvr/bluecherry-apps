<?php DEFINE('INDVR', true);

#lib
include("../lib/lib.php");  #common functions

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

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('basic');

if (!isset($_GET['id']) or !$current_user->camPermission($_GET['id']))
	image_err(str_replace('%ID%', $_GET['id'], MJPEG_NO_PERMISSION));

$this_camera = data::getObject('Devices', 'id', $_GET['id']);
if ($this_camera[0]['disabled'] == true){
	image_err(MJPEG_DISABLED);
} elseif (!$this_camera){
	image_err(str_replace('%ID%', $_GET['id'], MJPEG_DEVICE_NOT_FOUND));
}

function get_boundary($url_full)
{
	global $boundary, $single_url;

	$url = parse_url($url_full);
	if (!$url)
		return;

	if (empty($url['port']))
		$url['port'] = 80;

	if (!empty($url['user']) and !empty($url['pass']))
		$auth = base64_encode($url['user'] . ":" . $url['pass']);
	else
		$auth = false;

	$fh = fsockopen($url['host'], $url['port']);
	if (!$fh)
		return;

	if (empty($url['query']))
		$path = $url['path'];
	else
		$path = $url['path'] . "?" . $url['query'];


	fwrite($fh, "GET ". $path ." HTTP/1.0\r\n");
	fwrite($fh, "Host: ". $url['host'] ."\r\n");
	if ($auth)
		fwrite($fh, "Authorization: Basic $auth\r\n");
	fwrite($fh, "\r\n");

	$myb = "";
	// Read the header to get the Content-Type
	while (($msg = fgets($fh)) != FALSE) {
		if ($msg == "\r\n" || $msg == "\n")
			break;
		$matches = array();
		if (preg_match('/^Content-Type:\s*multipart\/x-mixed-replace\s*;.*boundary=(\S+)/i', $msg, $matches)) {
			$boundary = $matches[1];
			fclose($fh);
			return;
		} else if (stristr($msg, 'Content-Type:'))
			break;
	}
	fclose($fh);

	if ($msg == FALSE)
		return;

	/* The URL only supplies us one JPEG per request, so we make it a feed */
	if (stristr($msg, "Content-Type: image/jpeg"))
		$single_url = TRUE;
}

function get_one_jpeg($url_full)
{
	global $single_url;

	$url = parse_url($url_full);
	if (!$url)
		return;

	if (empty($url['port']))
		$url['port'] = 80;

	if (!empty($url['user']) and !empty($url['pass']))
		$auth = base64_encode($url['user'] . ":" . $url['pass']);
	else
		$auth = false;

	$fh = fsockopen($url['host'], $url['port']);
	if (!$fh)
		return;

	if (empty($url['query']))
		$path = $url['path'];
	else
		$path = $url['path'] . "?" . $url['query'];

	fwrite($fh, "GET ". $path ." HTTP/1.0\r\n");
	fwrite($fh, "Host: ". $url['host'] ."\r\n");
	if ($auth)
		fwrite($fh, "Authorization: Basic $auth\r\n");
	fwrite($fh, "\r\n");

        $myl = 0;
	$myj = FALSE;

	// For multipart/mixed, skip the initial header
	if ($single_url == FALSE) {
		while (($msg = fgets($fh)) != FALSE) {
			if ($msg == "\r\n" || $msg == "\n")
				break;
		}
		// Skip boundary
		fgets($fh);
	}

        // Read the header to get the Content-Length
        while (($msg = fgets($fh)) != FALSE) {
		if ($msg == "\r\n" || $msg == "\n")
			break;
		sscanf($msg, "Content-Length: %d", $myl);
        }

	if ($myl) {
		$myread = $myl;
		$myj = "";
		do {
			$tmp = fread($fh, $myread);
			if ($tmp == FALSE)
				break;
			$myj = $myj . $tmp;
			$myread -= strlen($tmp);
		} while ($myread > 0);
	}

        fclose($fh);

	return $myj;
}

// Checks for in-progress event
function check_active($id)
{
	$list = bc_db_get_table("SELECT length FROM EventsCam WHERE device_id=$id ".
				"ORDER BY id DESC LIMIT 1");
	if (empty($list) || $list[0]['length'] != -1)
		return FALSE;

	return TRUE;
}

if (!isset($_GET['id']))
	image_err("No device ID supplied");

$id = intval($_GET['id']);
$bch = bc_handle_get_byid($id);
if ($bch == false)
	image_err("No such device");

$single_url = FALSE;
$boundary = "BCMJPEGBOUNDARY";

header("Cache-Control: no-cache");
header("Cache-Control: private");
header("Pragma: no-cache");

session_write_close();

if (isset($_GET['multipart']))
	$multi = true;
else
	$multi = false;

# Check to see if this device has a URL to get the MJPEG
$url = bc_get_mjpeg_url($bch);
if (!$url) {
	# Nope, so try to start up the local device
	if (bc_set_mjpeg($bch) == false)
		image_err("Failed to set MJPEG");

	if (bc_handle_start($bch) == false)
		image_err("Faled to start");
} else {
	# Get the boundary as well
	get_boundary($url);
	# Some devices (Axis) add an extra -- on the boundary that is not in the output.
	# Strip it. Bug #992
	if (substr($boundary, 0, 2) == "--")
		$boundary = substr($boundary, 2);
}

if ($multi)
        header("Content-type: multipart/x-mixed-replace; boundary=$boundary");

$intv_low = false;
$intv_time = 0;
$intv_cnt = 0;
$intv = 1;

if (!empty($_GET['interval'])) {
	if ($_GET['interval'] == "low")
		$intv_low = true;
	else
		$intv = intval($_GET['interval']);
}

$is_active = false;
$active_time = -1;

if (isset($_GET['activity']))
	$active_time = 0;

function print_image() {
	global $multi, $bch, $boundary, $url, $intv_low, $intv_time;
	global $intv_cnt, $intv, $id, $is_active, $active_time;

	$myl = 0;
	$myj = FALSE;

	if ($url) {
		$myj = get_one_jpeg($url);
	} else {
		if (bc_buf_get($bch) == false) {
			sleep(1);
			return;
		}
		$myj = bc_buf_data($bch);
	}

	$tm = time();

	if ($intv_low) {
		if ($tm <= $intv_time)
			return;
		$intv_time = $tm;
	} else {
		$intv_cnt++;
		if ($intv_cnt < $intv)
			return;
		$intv_cnt = 0;
	}

	// $active_time < 0 means the peer isn't interested in activity at all
	if ($active_time >= 0) {
		if ($tm > $active_time) {
			$is_active = check_active($id);
			$active_time = $tm;
		}

		$header = "Bluecherry-Active: ".($is_active ? "true" : "false");
		if ($multi)
			print $header."\r\n";
		else
			header($header);
	}

	if ($multi) {
		print "Content-type: image/jpeg\r\n";
		print "Content-size: " . strlen($myj) . "\r\n\r\n";
	} else {
		header("Content-type: image/jpeg");
	}

	print $myj;
	unset($myj);

	if ($multi)
		print "\r\n--$boundary\r\n";

	flush();
}

# For multi, let's do some weird stuff
if ($multi) {
	set_time_limit(0);
	@apache_setenv('no-gzip', 1);
	@ini_set('zlib.output_compression', 0);
	while (ob_get_level())
		ob_end_flush();
}

# Cleanup some unused resources
if ($url) {
	bc_handle_free($bch);

	// For this case, we pass off to curl
	if ($multi and $single_url == FALSE and !$intv_low and $intv == 1) {
		passthru("curl -s " . escapeshellarg($url));
		exit;
	}

	// Other url cases, we handle in the loop
}

do {
	print_image();
} while($multi);

bc_db_close();
if (!$url)
	bc_handle_free($bch);

?>
