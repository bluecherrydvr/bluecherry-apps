<?php 

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

class activeusers extends Controller {
    public $users;

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.activeusers');

		$tmp = data::query("SELECT * FROM ActiveUsers");
		foreach($tmp as $k => $user){
			$this->users[$k] = new user('id', $user['id']);
			$this->users[$k]->info['ip'] = $user['ip'];
			$this->users[$k]->info['kick'] = $user['kick'];
			$this->users[$k]->info['from_client'] = $user['from_client'];
        }

        $this->view->this_users = $this->users;

		//var_dump_pre($this->users);
        if ($this->reqXML()) {
            $this->MakeXML(); 
            die();
        }
    }

    private function MakeXML()
    {
		$xml = '<?xml version="1.0" encoding="UTF-8" ?><activeusers>';
		foreach($this->users as $k => $user){
			$xml .= "<user id='{$user->info['id']}'>";
				$xml .= "<name>{$user->info['name']}</name>
						<ip>{$user->info['ip']}</ip>
						<kick>{$user->info['kick']}</kick>";
			$xml .= "</user>";
		}
		$xml .='</activeusers>';
		header('Content-type: text/xml');
		print_r($xml);
	}
}

