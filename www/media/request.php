<?php

DEFINE(INDVR, true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('viewer');
session_write_close();
#/auth check


function requestError($message)
{
	header('HTTP/1.1 550 Media request error');
	die('<error>'.$message.'</error>');
}

function dl_file_resumable($file)
{
	if (!is_file($file))
		requestError('File does not exist!');

	$size = filesize($file);
	$fileinfo = pathinfo($file);

	$filename = $fileinfo['basename'];
	$file_extension = strtolower($path_info['extension']);
	$ctype = 'video/mpeg';

	if (isset($_SERVER['HTTP_RANGE'])) {
		list($size_unit, $range_orig) =
			explode('=', $_SERVER['HTTP_RANGE'], 2);

		if ($size_unit == 'bytes')
			list($range, $extra_ranges) =
				explode(',', $range_orig, 2);
		else
			$range = '';
	} else {
		$range = '';
	}

	// Figure out download piece from range (if set)
	list($seek_start, $seek_end) = explode('-', $range, 2);

	// Set start and end based on range (if set), else set defaults.
	// Also check for invalid ranges.
	$seek_end = (empty($seek_end)) ? ($size - 1) :
		min(abs(intval($seek_end)),($size - 1));
	$seek_start = (empty($seek_start) ||
		$seek_end < abs(intval($seek_start))) ? 0 :
			max(abs(intval($seek_start)),0);

        // Only send partial content header if downloading a piece of the file
	// (IE workaround)
	if ($seek_start > 0 || $seek_end < ($size - 1))
		header('HTTP/1.1 206 Partial Content');

	header('Accept-Ranges: bytes');
	header('Content-Range: bytes '.$seek_start.'-'.$seek_end.'/'.$size);
	header('Content-Type: ' . $ctype);
	header('Content-Disposition: attachment; filename="' . $filename . '"');
	header('Content-Length: '.($seek_end - $seek_start + 1));

	// Open the file
	$fp = fopen($file, 'rb');
	// Seek to start of missing part
	fseek($fp, $seek_start);

	// Start buffered download
	while(!feof($fp)) {
		// Reset time limit for big files
		set_time_limit(0);
		print(fread($fp, 1024*8));
		flush();
		ob_flush();
	}

	fclose($fp);
	exit;
}

if (empty($_GET['id']))
	requestError('No ID sent');

$id = $_GET['id'];

mb_http_output("pass");

if (!bc_db_open())
	requestError('Could not open database');

$events = bc_db_get_table("SELECT * FROM Media WHERE id=" . intval($id));
if (empty($events))
	requestError('Could not retrieve media for '.$id);

$item = $events[0];

bc_db_close();

dl_file_resumable($item['filepath']);

?>
