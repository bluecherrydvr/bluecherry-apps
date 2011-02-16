<?php DEFINE('INDVR', true);

#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('mjpeg');
#/auth check

function get_boundary($url_full)
{
	global $boundary, $single;

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

	fwrite($fh, "GET ". $url['path'] ." HTTP/1.0\r\n");
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
		$single = TRUE;
}

function get_one_jpeg($url_full)
{
	global $single;

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

	fwrite($fh, "GET ". $url['path'] ." HTTP/1.0\r\n");
	fwrite($fh, "Host: ". $url['host'] ."\r\n");
	if ($auth)
		fwrite($fh, "Authorization: Basic $auth\r\n");
	fwrite($fh, "\r\n");

        $myl = 0;
	$myj = FALSE;

	// For multipart/mixed, skip the initial header
	if ($single == FALSE) {
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

if (!isset($_GET['id'])) {
	print "No device supplied";
	exit;
}

$bch = bc_handle_get_byid(intval($_GET['id']));
if ($bch == false) {
	print "No such device";
	exit;
}

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
	if (bc_set_mjpeg($bch) == false) {
		print "Failed to set MJPEG";
		exit;
	}

	if (bc_handle_start($bch) == false) {
		print "Faled to start";
		exit;
	}
} else {
	# Get the boundary as well
	get_boundary($url);
}

if ($multi) {
        header("Content-type: multipart/x-mixed-replace; boundary=$boundary");
        print "\r\n--$boundary\r\n";
} else {
	header("Content-type: image/jpeg");
}

function print_image() {
	global $multi, $bch, $boundary, $url;

	$myl = 0;
	$myj = FALSE;

	if ($url) {
		$myj = get_one_jpeg($url);
		$myl = strlen($myj);
	} else {
		if (bc_buf_get($bch) == false)
			exit;
		$myl = bc_buf_size($bch);
		$myj = bc_buf_data($bch);
	}

	if ($multi) {
		print "Content-type: image/jpeg\r\n";
		print "Content-size: " . $myl . "\r\n\r\n";
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

# For URLs, we just pass through to curl
if ($url) {
	bc_handle_free($bch);
	bc_db_close();

	// For this case, we pass off to curl
	if ($multi and $single == FALSE) {
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
