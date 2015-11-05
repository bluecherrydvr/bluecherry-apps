<?php

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */


class ptzpresetlist extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.ptzpresetlist');

        $this->view->presets = data::getObject('ipPtzCommandPresets');

        $camera_id = (!empty($_GET['id'])) ? intval($_GET['id']) : false;
        $this->view->camera = new ipCamera($camera_id);
    }

    public function postData()
    {

    }
}

