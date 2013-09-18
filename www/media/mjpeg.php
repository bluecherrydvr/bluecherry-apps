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
$coder = coder_handle_get_byid($id);
if ($coder == false)
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

# Nope, so try to start up the local device
if (coder_handle_start($coder) == false)
	image_err("Faled to start");

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

$start_time = time();

function print_image() {
	global $multi, $coder, $boundary, $intv_low, $intv_time;
	global $intv_cnt, $intv, $id, $is_active, $active_time;
	global $start_time;

	$myl = 0;
	$myj = FALSE;

	$myj = coder_buf_data($coder);

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

	if ($tm - $start_time > 3600*2) {
		/* Bug #914: Work around bad httpd memory allocation strategies by
		 * dropping the connection every two hours to force a new request.
		 * Won't be relevant after RTP anyway. */
		$multi = false;
	}
}

# For multi, let's do some weird stuff
if ($multi) {
	set_time_limit(0);
	@apache_setenv('no-gzip', 1);
	@ini_set('zlib.output_compression', 0);
	while (ob_get_level())
		ob_end_flush();
}

do {
	print_image();
	usleep(20000);
} while($multi);

bc_db_close();

coder_handle_free($coder);

?>
