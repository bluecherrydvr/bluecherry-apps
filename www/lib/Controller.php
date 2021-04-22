<?php

require_once('bc_license_wrapper.php');

class Controller {

    protected $core;
    protected $view;  
    protected $ctl;  
    protected $user = Array();
    protected $main_template = true;
    protected $varpub;
    protected $left_menu = true;

    public function __construct ()
    {
        $this->varpub = VarPub::get();
        $this->core = new StdClass();
        $this->ctl = new StdClass();

        $this->core->session = new Session();

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

    public function setView($name, $main_template = true)
    {
        $this->main_template = $main_template;

        $this->view = new View($name);
    }

    public function getStartView()
    {
        $this->view->domain = $this->core->domain;

        // set data from $GLOBALS
        foreach ($this->varpub->globals as $key => $val) {
            $this->view->$key = $val;
        }

        $page_container = $this->view->render();


        if ($this->main_template) {
            $main_page = new View('main_admin');

            $main_page->left_menu = $this->left_menu;
            $main_page->current_user = $this->user;

            $main_page->page_container = $page_container;
            
            foreach ($this->varpub->getVars() as $key => $val) {
                $main_page->$key = $val;
            }

            // check for general errors
            $main_page->general_error = '';
    		$default_password_warning_dismiss = (!empty($_COOKIE['default_password_warning_dismiss'])) ? $_COOKIE['default_password_warning_dismiss'] : false;
    		if ($this->user->info['default_password'] && !empty($_COOKIE['default_password_warning_dismiss'])) {
    			$main_page->general_error = array('type' => 'INFO', 'text' => WARN_DEFAULT_PASSWORD);
    		}

            $ret = bc_license_check_genuine();
            $main_page->general_error = array('type' => 'INFO', 'text' => $ret[1]);

    		$status = data::getObject('ServerStatus');
    		if (!empty($status[0]['message'])) {
    			$m = preg_split( '/\r\n|\r|\n/', $status[0]['message']);
    			$main_page->general_error = array('type' => 'F', 'text' => $m[0]);
    		}


            $res = $main_page->render();

        } else {
            $res = $page_container;
            $res .= getJs();
        }
        

        return $res;
    }

    public function renderView()
    {
        $res = $this->view->render();
        $this->view->flushData();

        return $res;
    }    

    public function start($uri = Array())
    {
        $this->uri = $uri;

        if (Inp::method() == 'POST') {
            if (!empty($uri->two)) {
                $method = ucfirst($uri->two);
                $method = 'post'.$method;

                if (method_exists($this, $method)) {
                    $res = $this->$method();
                } else {
                    $res = $this->postData();
                }
            } else {
                $res = $this->postData();
            }
        } else {
            if (!empty($uri->two)) {
                $method = ucfirst($uri->two);
                $method = 'get'.$method;

                if (method_exists($this, $method)) {
                    $res = $this->$method();
                } else {
                    $res = $this->getData();
                }
            } else {
                $res = $this->getData();
            }

            if ($this->view) {
                if (Inp::get('type') == 'ajax_content') $this->main_template = false;

                foreach ($this->varpub->getVars() as $key => $val) {
                    $this->view->$key = $val;
                }
                $this->view->current_user = $this->user;

                $res = $this->getStartView();
            }
        }

        return $res;
    }

    protected function reqXML()
    {
        if (Inp::get('XML')) return true;
        else return false;
    }

    protected function chAccess($role)
    {
        $this->user = new user('id', $this->core->session->get('id'));
        $this->user->checkAccessPermissions($role);
        
    }

    public function ctl($name)
    {
        $res = null;

        $path = str_replace('.', '/', $name);
        $name_arr = explode('/', $path);
        $name = $name_arr[count($name_arr) - 1];

        $path_class = ctl_path.$path.'.php';

        if (file_exists($path_class)) {
            if (!isset($this->ctl->$name)) {
                include_once($path_class);
                $this->ctl->$name = new $name();
            }
            $res = $this->ctl->$name;
        } else {
			//throw new Exception('can\'t find model'.$name);
        }

        return $res;
    }
}
