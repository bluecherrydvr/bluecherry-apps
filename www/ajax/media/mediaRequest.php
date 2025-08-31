<?php

include_once lib_path . 'lib.php';

class mediaRequest extends Controller {
	
    public function __construct()
    {
        parent::__construct();
		$this->chAccess('basic');
    }

    public function getData()
    {
        session_write_close();

        #screenshot API
        if (!empty($_GET['mode']) && $_GET['mode']=='screenshot'){
	
        	$device_id = (!empty($_GET['device_id'])) ? intval($_GET['device_id']) : false;
        	$event_id = (!empty($_GET['id'])) ? intval($_GET['id']) : false;
        	(!empty($device_id) || !empty($event_id)) or die('E: Event ID or camera ID is required to get a screenshot.');
	
        	$event = (empty($event_id)) ? data::query("SELECT id, filepath FROM Media WHERE device_id='{$device_id}' AND filepath IS NOT NULL ORDER BY id DESC LIMIT 1") : data::getObject('Media', 'id', $event_id);
		!empty($event) or requestError('E: Requested event does not exist');
	
        	$event = $event[0];
		!empty($event['filepath']) or requestError('E: No media is associated with this event.');
	
//        	$path_to_image = str_replace('mkv', 'jpg', $event['filepath']);
		$path_to_image = str_replace('mp4', 'jpg', $event['filepath']);
		file_exists($path_to_image) or requestError('E: Screenshot for this event was not found');
        	header('content-type:image/jpeg');
        	readfile($path_to_image);
        	exit();
        }

        if (empty($_GET['id']))
        	requestError('No ID sent');

        $id = intval($_GET['id']);

        mb_http_output("pass");

        
        #	requestError('Could not open database');
        $db = database::getInstance();

        $events = $db->fetchAll("SELECT * FROM Media WHERE id=" . intval($id));
        if (empty($events))
        	requestError('Could not retrieve media for '.$id);

        $item = $events[0];


        dl_file_resumable($item['filepath']);

    }
}


function requestError($message)
{
	header('HTTP/1.1 404 Media request error');
	die('<error>'.$message.'</error>');
}

function dl_file_resumable($file)
{
	if (!is_file($file))
		requestError('File does not exist!');

	$size = filesize($file);
	$fileinfo = pathinfo($file);

	$filename = $fileinfo['basename'];
	#$file_extension = strtolower($path_info['extension']);
	$ctype = 'video/mpeg';

	if (isset($_SERVER['HTTP_RANGE'])) {
		list($size_unit, $range_orig) =
			explode('=', $_SERVER['HTTP_RANGE'], 2);

		if ($size_unit == 'bytes') {
			$ranges = explode(',', $range_orig, 2);
			$range = $ranges[0];
			$extra_ranges = isset($ranges[1]) ? $ranges[1] : null;
		} else {
			$range = '-';
		}
	} else {
		$range = '-';
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

	if (!$fp)
		exit;

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

