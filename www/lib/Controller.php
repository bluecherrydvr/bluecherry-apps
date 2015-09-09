<?php

class Controller {

    protected $core;
    protected $view;  

    public function __construct ()
    {
        $this->core = new StdClass();

        if (isset($_SERVER['HTTPS']) && ($_SERVER['HTTPS']=='on')) {
            $prot = 'https://';
        } else $prot = 'http://';
        if (isset($_SERVER['SERVER_NAME'])) $this->core->domain = $prot . $_SERVER['SERVER_NAME'];
        else $this->core->domain = '';

    }

    public function ajaxView($res, $add = '')
    {
        if (is_array($res)) {
            $res = Reply::ajax($res);

            die($res);
        } else {
            $res = Reply::ajaxDie($res, $add);
        }
    }    

    public function setView($name)
    {
        $this->view = new View($name);
    }

    public function getStartView()
    {
        $this->view->domain = $this->core->domain;

        $res = $this->view->render();

        return $res;
    }

    public function renderView()
    {
        $res = $this->view->render();
        $this->view->flushData();

        return $res;
    }    
}
