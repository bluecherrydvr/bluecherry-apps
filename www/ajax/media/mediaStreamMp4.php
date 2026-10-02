<?php 

class mediaStreamMp4 extends Controller {

    public function __construct()
    {
        parent::__construct();
        $this->chAccess('basic');
    }

    public function getData()
    {
        if (!isset($_GET['id']) || !is_numeric($_GET['id'])) {
            header('HTTP/1.0 400 Bad request - set id arg');
            die();
        }

        $media_id = $_GET['id'];

        # resolve local mediafile name and path from id parameter, using Media table;
        $query_res = data::query("SELECT filepath FROM Media WHERE id={$media_id} LIMIT 1");
        if (empty($query_res)) {
            header("HTTP/1.0 404 Not Found - failed to locate the file");
            die();
        }
        $filename = $query_res[0]['filepath'];

        if ($filename === '' || !file_exists($filename)) {
            header("HTTP/1.0 404 Not Found - recording file does not exist");
            die();
        }
        if (!is_readable($filename)) {
            header("HTTP/1.0 500 Internal Server Error - recording file is not readable (check permissions)");
            die();
        }

        # check video codec, using ffprobe and its parsable output (see -show-format -show-streams);
        # the video stream may sit at any index (some cameras mux audio first).
        $escaped_file = escapeshellarg($filename);
        $ffprobe_output = shell_exec("LD_LIBRARY_PATH=/usr/lib/bluecherry/ /usr/lib/bluecherry/ffprobe -show_format -show_streams -print_format flat {$escaped_file} 2>&1");
        $streams = ffprobeParseStreams($ffprobe_output);
        $video = ffprobeFirstVideoStream($streams);
        if ($video === null) {
            header("HTTP/1.0 500 Internal Server Error - no video stream found in file");
            die("No video stream found in file.<br><br>ffprobe output:<br><br>".nl2br(htmlspecialchars((string)$ffprobe_output)));
        }

        $outfile = tempnam('/tmp', 'bluecherry_streaming__');
        if ($outfile === false) {
            header("HTTP/1.0 500 Internal Server Error - failed to create temporary file");
            die();
        }
        $escaped_out = escapeshellarg($outfile);

        if (!ffprobeHasStream($streams, 'audio')) {
            # If no audio streams detected
            $audio_options = ' -an ';
        } else if (ffprobeHasStream($streams, 'audio', 'aac')) {
            # If AAC audio stream detected
            $audio_options = ' -acodec copy ';
        } else {
            # If audio stream in codec other than AAC detected
            $audio_options = ' -acodec aac -strict -2 ';
        }

	$vcodec = "libx264";
	$hwaccel = "";
	$hwfilter = "";

	$vaapi_device = $this->varpub->global_settings->data['G_VAAPI_DEVICE'];
	if (strcasecmp($vaapi_device, "none") != 0)
	{
		$vcodec = "h264_vaapi";
		$hwaccel = "-init_hw_device vaapi=hwva:$vaapi_device -hwaccel vaapi -hwaccel_output_format vaapi -hwaccel_device hwva";
		$hwfilter = "-filter_hw_device hwva -vf 'format=nv12|vaapi,hwupload'";
	}

        $video_codec = $video['codec'];
        if ($video_codec === 'h264' || $video_codec === 'mpeg4' || $video_codec === 'hevc') {
            # -- if codec is MPEG4, H264 or HEVC, remux the file into MP4 container format;
            # -faststart option must be used for MP4 file to enable instant playback start.
            # Note: HEVC in MP4 plays in Safari but not in most other browsers.
            $ffmpeg_cmd = "LD_LIBRARY_PATH=/usr/lib/bluecherry/ /usr/lib/bluecherry/ffmpeg -i {$escaped_file} {$audio_options} -vcodec copy  -movflags faststart -f mp4 -y {$escaped_out} 2>&1 && echo SUCCEED";
        } else if ($video_codec === 'mjpeg') {
            # -- otherwise, if codec is MJPEG, reencode the video stream to MPEG4 or H264 codec;
            # Lower framerate on request, but forbid making it unreasonably high to avoid DoS attack
            if (isset($_GET['fps']) && is_numeric($_GET['fps']) && $_GET['fps'] > 0 && $_GET['fps'] <= 30)
                $fps = floatval($_GET['fps']);
            else
                $fps = 0;
            if ($fps > 0)
                $ffmpeg_cmd = "LD_LIBRARY_PATH=/usr/lib/bluecherry/ /usr/lib/bluecherry/ffmpeg {$hwaccel} -i {$escaped_file} {$audio_options} {$hwfilter} -vcodec {$vcodec} -r {$fps} -movflags faststart -f mp4 -y {$escaped_out} 2>&1 && echo SUCCEED";
            else
                $ffmpeg_cmd = "LD_LIBRARY_PATH=/usr/lib/bluecherry/ /usr/lib/bluecherry/ffmpeg {$hwaccel} -i {$escaped_file} {$audio_options} {$hwfilter} -vcodec {$vcodec} -movflags faststart -f mp4 -y {$escaped_out} 2>&1 && echo SUCCEED";
        } else {
            unlink($outfile);
            header("HTTP/1.0 500 Internal Server Error - unsupported codec in video file: {$video_codec}");
            die();
        }

        $ffmpeg_output = shell_exec($ffmpeg_cmd);
        if (strstr($ffmpeg_output, 'SUCCEED') === false) {
            unlink($outfile);
            header("HTTP/1.0 500 Internal Server Error - MP4 file preparation failed");
            die("MP4 file preparation failed:<br><br>".nl2br($ffmpeg_output));
        }

        # After saving resulting recording to temporary file (which is necessary
        # for MP4 format), output the file contents to stdout, indicating content type
        # in HTTP headers.
        if (Inp::get('download')) {
            $filename = str_replace('.mkv', '.mp4', $filename);
            downloadFile($outfile, $filename);
        } else downloadFile($outfile);

    }
}

