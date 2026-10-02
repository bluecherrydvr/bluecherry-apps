<?php

//makes an options dropdown
function arrayToSelect($options, $selected='', $id='', $class='', $disabled=false, $data_attr = ''){
    $tmp = "<select id='{$id}' class='{$class} form-control' {$data_attr} name='{$id}'".(($disabled) ? ' disabled="disabled"' : '').">";
    $assoc = is_assoc($options);
    foreach($options as $i => $value){
        if (is_array($value))$value = current($value);
        $tmp .= "<option value='".(($assoc) ? $i : $value)."'".(($value==$selected || ($assoc && $i==$selected)) ? ' selected="selected"' : '').">$value</option>";
    }
    $tmp .= "</select>";
    return $tmp;
}

function convertBytes($size, $precision = 2) {
    $base = log($size, 1024);
    $suffixes = array('', 'k', 'M', 'G', 'T');   

    return round(pow(1024, $base - floor($base)), $precision) . $suffixes[floor($base)];
}

// Returns array(http_code, message) when $file_path cannot be served,
// or null when the file exists and is readable. Separates "missing file"
// (orphaned DB row, pruned recording) from "unreadable file" (permissions)
// so callers can report the real cause.
function downloadFileError($file_path, $file_name = '') {
    $display = ($file_name !== '') ? basename($file_name) : 'temporary transcoded file';

    if (!file_exists($file_path)) {
        return array(404, "Not Found - file does not exist: {$display}");
    }

    if (!is_readable($file_path)) {
        return array(500, "Internal Server Error - file is not readable (check permissions): {$display}");
    }

    return null;
}

function downloadFile($file_path, $file_name = '', $delete_file = true) {

    $error = downloadFileError($file_path, $file_name);
    if ($error !== null) {
        header("HTTP/1.0 {$error[0]} {$error[1]}");
        die();
    }

    $fp = fopen($file_path, 'rb');
    if ($fp === false) {
        $display = ($file_name !== '') ? basename($file_name) : 'temporary transcoded file';
        header("HTTP/1.0 500 Internal Server Error - failed to open file: {$display}");
        die();
    }

    $file_info = new StdClass();
    $file_info->size = filesize($file_path);
    $file_info->name = basename($file_name);

    $range = 0;
    if (isset($_SERVER["HTTP_RANGE"]) && $_SERVER["HTTP_RANGE"]) {
        $range = $_SERVER["HTTP_RANGE"];
        $range = str_replace("bytes=", "", $range);
        $range = str_replace("-", "", $range);
        if ($range) {
            fseek($fp, $range);
        }
    }

    session_write_close();
    if (ob_get_length()) ob_end_clean();
    if ($delete_file) unlink($file_path);

    header('Content-Description: Archive File');
    header('Content-Type: application/octet-stream');
    header('Expires: 0');
    header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
    header('Pragma: public');
    header('Accept-Ranges: bytes');
    if (!empty($file_info->name)) header('Content-Disposition: attachment; filename='.$file_info->name);

    if ($range) {
        header('Content-Range: bytes '.$range.'-'.($file_info->size - 1).'/'.$file_info->size);
        header('Content-Length:' . ($file_info->size - $range));

        header($_SERVER['SERVER_PROTOCOL'].' 206 Partial Content');
    } else {
        header('Content-Length:' . $file_info->size);

        header($_SERVER['SERVER_PROTOCOL'].' 200 OK');
    }


    while(!feof($fp)) {
        print(fread($fp, 8192));
        flush();
    }
    fclose($fp);

    die();
}

// Parse `ffprobe -show_streams -print_format flat` output into
// array(stream_index => array('codec_type' => ..., 'codec_name' => ...)).
// Values are lowercased; unparseable input yields an empty array.
function ffprobeParseStreams($flat_output) {
    $streams = array();
    if (!is_string($flat_output) || $flat_output === '') {
        return $streams;
    }
    foreach (explode("\n", $flat_output) as $line) {
        if (preg_match('/^streams\.stream\.(\d+)\.([A-Za-z0-9_]+)="([^"]*)"/', trim($line), $m)) {
            $idx = intval($m[1]);
            if (!isset($streams[$idx])) {
                $streams[$idx] = array('codec_type' => '', 'codec_name' => '');
            }
            if ($m[2] === 'codec_type' || $m[2] === 'codec_name') {
                $streams[$idx][$m[2]] = strtolower($m[3]);
            }
        }
    }
    ksort($streams);
    return $streams;
}

// First video stream (lowest index) as array('index' => ..., 'codec' => ...),
// or null when the parsed output contains no video stream.
function ffprobeFirstVideoStream($streams) {
    foreach ($streams as $index => $stream) {
        if (isset($stream['codec_type']) && $stream['codec_type'] === 'video') {
            $codec = isset($stream['codec_name']) ? $stream['codec_name'] : '';
            return array('index' => $index, 'codec' => $codec);
        }
    }
    return null;
}

// Whether any parsed stream matches $codec_type and, when given, $codec_name.
function ffprobeHasStream($streams, $codec_type, $codec_name = null) {
    foreach ($streams as $stream) {
        if (!isset($stream['codec_type']) || $stream['codec_type'] !== $codec_type) {
            continue;
        }
        if ($codec_name !== null && (!isset($stream['codec_name']) || $stream['codec_name'] !== $codec_name)) {
            continue;
        }
        return true;
    }
    return false;
}

function localeEn() {
    $res = 1;
    if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
        $lang = strtolower(substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 5));

        if ($lang != 'en-us') $res = 0;
    }

    return $res;
}

function dateFormat($unix_time, $locale_en = true) {
    $res = new StdClass();

    if ($locale_en) {
        $res->format_php = 'm/d/Y h:i A';
        $res->format_js = 'mm/dd/yyyy HH:ii P';
        $res->time = date($res->format_php, $unix_time);
    } else {
        $res->format_php = 'd.m.Y H:i';
        $res->format_js = 'dd.mm.yyyy hh:ii';
        $res->time = date($res->format_php, $unix_time);
    }

    return $res;
}

function dateToUnix($date) {
    $date_format = dateFormat(time(), localeEn());

    $res = \DateTime::createFromFormat($date_format->format_php, $date)->format('U');

    return $res;
}

function addJs($val) {
    $varpub = VarPub::get();
    if (!is_array($varpub->javascript)) {
        $varpub->javascript = Array();
    }
    $js = $varpub->javascript;

    $js[] = $val;

    $varpub->javascript = $js;
}

function getJs() {
    $res = '<script type="text/javascript">';

    $varpub = VarPub::get();
    if ($varpub->javascript) {
        foreach ($varpub->javascript as $val) {
            $res .= $val;
        }
    }

    $res .= '</script>';

    return $res;
}


