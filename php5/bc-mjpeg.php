<?php

$multi = false;
$dev = 1;
$bch = false;
$boundary = "MJPEGBOUNDARY";

if (isset($_GET['channel']))
	$dev += intval($_GET['channel']);

if (isset($_GET['video']))
	$dev += intval($_GET['video']);

$dev = "/dev/video$dev";

header("Cache-Control: no-cache");
header("Cache-Control: private");
header("Pragma: no-cache");

$bch = bc_handle_get("$dev");
if ($bch == false) {
	print "Failed to open $dev";
	exit;
}

if (bc_set_mjpeg($bch) == false) {
	print "Failed to set MJPEG on $dev";
	exit;
}

if (bc_handle_start($bch) == false) {
	print "Faled to start $dev";
	exit;
}

if (isset($_GET['multipart'])) {
        $multi = true;
        header("Content-type: multipart/x-mixed-replace; boundary=$boundary");
        print "--$boundary\n";
} else {
	header("Content-type: image/jpeg");
}

function print_image() {
	global $multi, $bch, $boundary;

	if (bc_buf_get($bch) == false)
		exit;

	if ($multi) {
		print "Content-type: image/jpeg\n";
		print "Content-size: " . bc_buf_size($bch) . "\n\n";
	}

	print bc_buf_data($bch);

	if ($multi)
		print "--$boundary\n";
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

do {
	print_image();
} while($multi);

bc_handle_free($bch);
?>
