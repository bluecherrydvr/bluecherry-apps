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
	image_err("No permission");

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
		if ($msg == "\r\n")
			break;
		if (stristr($msg, "Content-Type: "))
			break;
		if (sscanf($msg, "Content-Type: multipart/".
			   "x-mixed-replace; boundary=%s",
			   $myb) == 1)
			break;
	}
	fclose($fh);

	if ($msg == FALSE)
		return;

	if (stristr($msg, "Content-Type: ") == FALSE)
		return;

	if (sscanf($msg, "Content-Type: multipart/x-mixed-replace; boundary=%s",
		   $myb) == 1) {
		$boundary = $myb;
		return;
	}

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
			if ($msg == "\r\n")
				break;
		}
		// Skip boundary
		fgets($fh);
	}

        // Read the header to get the Content-Length
        while (($msg = fgets($fh)) != FALSE) {
		if ($msg == "\r\n")
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
	$list = bc_db_get_table("SELECT id FROM EventsCam WHERE device_id=$id ".
				"AND length=-1");
	if (empty($list))
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
}

if ($multi)
        header("Content-type: multipart/x-mixed-replace; boundary=$boundary");

$intv_low = false;
$intv_time = 0;
$intv_cnt = 0;
$invt = 1;

if (!empty($_GET['interval'])) {
	if ($_GET['interval'] == "low")
		$intv_low = true;
	else
		$invt = intval($_GET['interval']);
}

function print_image() {
	global $multi, $bch, $boundary, $url, $intv_low, $intv_time;
	global $intv_cnt, $intv, $id;

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

	$is_active = check_active($id);

	if ($intv_low) {
		$tm = time();
		if ($tm <= $intv_time)
			return;
		$intv_time = $tm;
	} else {
		$intv_cnt++;
		if ($intv_cnt <= $intv)
			return;
		$intv_cnt = 0;
	}

	if ($is_active) {
		if ($multi)
			print "Bluecherry-Active: true\r\n";
		else
			header("Bluecherry-Active: true");
	}

	if ($multi) {
		print "Content-type: image/jpeg\r\n";
		print "Content-size: " . strlen($myj) . "\r\n\r\n";
	} else {
		header("Content-type: image/jpeg");
	}

	print $myj;

	if ($multi)
		print "\r\n--$boundary\r\n";
}

# For multi, let's do some weird stuff
if ($multi) {
	set_time_limit(0);
	@apache_setenv('no-gzip', 1);
	@ini_set('zlib.output_compression', 0);
	@ini_set('implicit_flush', 1);
	for ($i = 0; $i < ob_get_level(); $i++)
		ob_end_flush();
	ob_implicit_flush(1);
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
bc_handle_free($bch);

?>
