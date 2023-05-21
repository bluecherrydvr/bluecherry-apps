<?php

class Reply
{
    public static function ajax($data)
    {
        if (!isset($data['data'])) $data['data'] = '';
        if (!is_array($data['msg'])) $data['msg'] = Array($data['msg']);
        
        $res = json_encode($data);

        return $res;
    }

    public static function ajaxDie($stat, $msg, $data_tr = '')
    {
        $data['status'] = $stat;
        $data['msg'] = $msg;
        $data['data'] = $data_tr;

        http_response_code(503);

        die(self::ajax($data));
    }
}
