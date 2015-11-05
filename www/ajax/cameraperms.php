<?php

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */


class cameraperms extends Controller {
	public $camera_list;
	public $user;

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.cameraperms');

		$id = intval($_GET['id']);
		$this->user = new user('id', $id);
		$type = (Inp::get('t') == 'PTZ') ? 'access_ptz_list' : 'access_device_list';
		$this->camera_list = $this->matchCams($this->user->info[$type]);


        $p = new StdClass();
        $p->camera_list = $this->camera_list;
        $p->user = $this->user;

        $this->view->p = $p;
    }

    private function matchCams($access_list)
    {
		$camera_list = data::query("SELECT id, device_name FROM Devices");
		foreach($camera_list as $key => $camera){
			$camera_list[$key]['allowed'] = (in_array($camera_list[$key]['id'], $access_list)) ? false : true;
		}
		return $camera_list;
	}

}

