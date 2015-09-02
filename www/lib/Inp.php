<?php

class Inp
{
    public static function gp ($gp, $method='')
    {
        $gp_det = self::detMethod($gp, $method);        

        if ($gp_det == 'post') {
            $gp_res = filter_input(INPUT_POST, $gp, FILTER_SANITIZE_SPECIAL_CHARS); 
        } else {
            $gp_res = filter_input(INPUT_GET, $gp, FILTER_SANITIZE_SPECIAL_CHARS); 
        }

        if (!$gp_res) $gp_res = '';

        return trim($gp_res);
    }

    public static function gp_arr($gp)
    {
        $gp_det = self::detMethod($gp);
        if ($gp_det == 'post') {
            $gp_res =  filter_input_array(INPUT_POST, Array ($gp => Array( 'filter' => FILTER_SANITIZE_SPECIAL_CHARS, 'flags'  => FILTER_REQUIRE_ARRAY)));
        } else {
            $gp_res =  filter_input_array(INPUT_GET, Array ($gp => Array( 'filter' => FILTER_SANITIZE_SPECIAL_CHARS, 'flags'  => FILTER_REQUIRE_ARRAY)));
        }

        if (!$gp_res || empty($gp_res[$gp])) $gp_res = Array();
        else $gp_res = $gp_res[$gp];

        return $gp_res;
    }

    public static function has($gp, $method='')
    {
        $gp_det = self::detMethod($gp, $method);        
        return filter_has_var($gp_det, $gp);
    }

    public static function post($val, $res = null)
    {
        $gp_res = filter_input(INPUT_POST, $val, FILTER_SANITIZE_SPECIAL_CHARS); 
        if (!$gp_res) $gp_res = $res;

        return trim($gp_res);
    }

    public static function get($val, $res = null)
    {
        $gp_res = filter_input(INPUT_GET, $val, FILTER_SANITIZE_SPECIAL_CHARS); 
        if (!$gp_res) $gp_res = $res;

        return trim($gp_res);
    }

    public static function method()
    {
        $res = null;
        if ($_SERVER['REQUEST_METHOD'] == 'POST') $res = 'POST';

        return $res;
    }

    private static function detMethod ($gp, $method = '')
    {
        switch ($method) {
            case 'get':
                $gp_det = 'get';
                break;
            case 'post':
                $gp_det = 'post';
                break;
            default:
                $gp_det =  filter_has_var(INPUT_POST, $gp);        
                if ($gp_det) {
                    $gp_det = 'post';
                } else {
                    $gp_det =  filter_has_var(INPUT_GET, $gp);        
                    if ($gp_det) $gp_det = 'get';        
                }
                break;
        }
        
        return $gp_det;
    }
}
