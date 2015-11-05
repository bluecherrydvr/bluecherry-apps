<?php

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */


class ptzpresetedit extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.ptzpresetedit');

        $camera_id = (!empty($_GET['ref'])) ? intval($_GET['ref']) : false;
        $this->view->camera = new ipCamera($camera_id);

        $preset_id = (!empty($_GET['id'])) ? intval($_GET['id']) : 'new';
        $this->view->preset = new ipPtzPreset($preset_id);
    }

    public function postData()
    {
	    $id = $_POST['presetId']; unset($_POST['presetId']); unset($_POST['ref']);
    	$type = ($id!='new') ? 'update' : 'insert';
    	$_POST['custom'] = ($id == 'new' || $id >=9000) ? 1 : 0;
    	$query = data::formQueryFromArray($type, 'ipPtzCommandPresets', $_POST, 'id', $id);
    	$result = data::query($query, true);
    	data::responseJSON($result);
    	exit();
    }
}

