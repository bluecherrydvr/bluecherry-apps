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

function downloadFile($file_path, $file_name = '', $delete_file = true) {

    if ($fp = fopen($file_path, 'rb')) {
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

    } else {
        header("HTTP/1.0 500 Internal Server Error - weird: cannot read just-written file");
    }

    die();
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


