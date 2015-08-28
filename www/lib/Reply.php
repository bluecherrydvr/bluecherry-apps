<?php

class Reply
{
    public static function ajax($data)
    {
        $stat = $data['status'];
        $msg = $data['msg'];

        if (!is_array($msg)) $msg = Array($msg);

        $response['status'] = $stat;
        $response['msg'] = $msg;
        
        $res = json_encode($response);

        //die($res);
        return $res;
    }

    public static function ajaxDie($stat, $msg)
    {
        $data['status'] = $stat;
        $data['msg'] = $msg;

        die(self::ajax($data));
    }
}
