<?php

class VarPub
{
    private $vars = Array ();
    private static $instance;

    private function __construct() {}
    private function __clone() {}
    public  function __wakeup() {}
    
    public static function get () 
    { 
        if ( empty ( self::$instance ) ) {
            self::$instance = new self ();
        }

        return self::$instance;
    }

    public function getVars()
    {
        return $this->vars;
    }

    public function __set ( $key, $val ) 
    { 
        $this->vars [$key] = $val;
    }

    public function __get( $key ) 
    { 
        if (isset($this->vars[$key])) {
            return $this->vars[$key];
        }
        return false;
    }

    public function __isset($key)
    {
        if (isset($this->vars[$key])) {
            return true;
        }
        return false;
    }

}


