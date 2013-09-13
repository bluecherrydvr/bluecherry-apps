<?php DEFINE('INDVR', true);

include("../lib/lib.php");

function mkreq($url)
{
	$purl = parse_url($url);
	if (!$purl)
		return null;

	$port = empty($purl['port']) ? 80 : $purl['port'];

	$path = $purl['path'];
	if (!empty($purl['query']))
		$path .= "?" . $purl['query'];

	$auth = (empty($purl['user']) or empty($purl['pass'])) ? "" :
		"Authorization: Basic " .
		base64_encode($purl['user'] . ":" . $purl['pass']) .
		"\r\n";

	$req = "GET " . $path . " HTTP/1.0\r\n"
		. "Host: " . $purl['host'] . "\r\n"
		. $auth . "\r\n";

	$fh = fsockopen($purl['host'], $port);
	if (!$fh)
		return null;

	fwrite($fh, $req);

	// FIXME: check response
	fgets($fh);

	return $fh;
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

function hdr_parse($fh)
{
	$hdr = array();

	while (($tmp = fgets($fh)) !== FALSE) {
		if ($tmp[0] == '\r' or $tmp[0] == '\n')
			break;

		if ($tmp[0] != ' ') {
			$h = explode(':', $tmp);
			$k = strtolower($h[0]);
			$hdr[$k] = ltrim($h[1]);
		} else {
			$hdr[$k] .= ltrim($tmp);
		}
	}

	return $hdr;
}

function get_boundary($url)
{
	global $boundary, $src_single;

	$fh = mkreq($url);
	if (!$fh)
		return;

	$hdr = hdr_parse($fh);

	$matches = array();
	$src_single = !preg_match('/^multipart\/.*;\s*boundary=(\S+)/i',
				 $hdr['content-type'], $matches);
	if (!$src_single)
		$boundary = $matches[1];

	fclose($fh);
}

function get_one_jpeg($url)
{
	global $src_single, $boundary;

	$fh = mkreq($url);
	if (!$fh)
		return;

	// For multipart/mixed, skip the initial header
	if (!$src_single) {
		hdr_parse($fh);

		// Skip boundary
		fgets($fh);
	}

	// Read the part header to get the Content-Length
	$hdr = hdr_parse($fh);
	$len = 0;
	if (!empty($hdr['content-length']))
		sscanf($hdr['content-length'], '%d', $len);

	$chunksz = max($len, 512);
	$payload = '';
	$boundarystr = "\n--" . $boundary;
	do {
		$tmp = fread($fh, $chunksz);
		if ($tmp === FALSE)
			break;

		$payload .= $tmp;

		if ($len) {
			$len -= strlen($tmp);
		} else {
			$bpos = strpos($payload, $boundarystr, strlen($payload) - strlen($tmp) - strlen($boundarystr) - 1);
			if ($bpos !== FALSE) {
				if ($bpos > 0 && $tmp[$bpos-1] == '\r')
					$bpos--;
				$payload = substr($payload, 0, $bpos);
				break;
			}
		}
	} while ($len > 0);

        fclose($fh);

	return $payload;
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

$src_single = FALSE;
$boundary = "BCMJPEGBOUNDARY";

header("Cache-Control: no-cache");
header("Cache-Control: private");
header("Pragma: no-cache");

session_write_close();

$out_multipart = isset($_GET['multipart']);

// Check to see if this device has a URL to get the MJPEG
$url = bc_get_mjpeg_url($bch);
if (!$url) {
	// Nope, so try to start up the local device
	if (bc_set_mjpeg($bch) == false)
		image_err("Failed to set MJPEG");

	if (bc_handle_start($bch) == false)
		image_err("Faled to start");
} else {
	// Get the boundary as well
	get_boundary($url);
	/* Some devices (Axis) add an extra -- on the boundary that is not in
	 * the output. Strip it. Bug #992 */
	if (substr($boundary, 0, 2) == "--")
		$boundary = substr($boundary, 2);
}

if ($out_multipart)
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

function print_image($url) {
	global $out_multipart, $bch, $boundary, $intv_low, $intv_time;
	global $intv_cnt, $intv, $id, $is_active, $active_time;
	global $start_time;

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
		if ($out_multipart)
			print $header."\r\n";
		else
			header($header);
	}

	if ($out_multipart) {
		print "Content-type: image/jpeg\r\n";
		print "Content-size: " . strlen($myj) . "\r\n\r\n";
	} else {
		header("Content-type: image/jpeg");
	}

	print $myj;
	unset($myj);

	if ($out_multipart)
		print "\r\n--$boundary\r\n";

	flush();

	if ($tm - $start_time > 3600*2) {
		/* Bug #914: Work around bad httpd memory allocation strategies
		 * by dropping the connection every two hours to force a new
		 * request. Won't be relevant after RTP anyway. */
		$out_multipart = false;
	}
}

// For multi, let's do some weird stuff
if ($out_multipart) {
	set_time_limit(0);
	@apache_setenv('no-gzip', 1);
	@ini_set('zlib.output_compression', 0);
	while (ob_get_level())
		ob_end_flush();
}

// Cleanup some unused resources
if ($url) {
	bc_handle_free($bch);

	// For this case, we pass off to curl
	if ($out_multipart and $src_single == FALSE and !$intv_low and $intv == 1) {
		/* -m is for the bug #914 workaround; see print_image */
		passthru("curl -s -m 7200 " . escapeshellarg($url));
		exit;
	}

	// Other url cases, we handle in the loop
}

do {
	print_image($url);
} while($out_multipart);

bc_db_close();
if (!$url)
	bc_handle_free($bch);

?>
