<?php

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

class addip extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.addip');


    }

    public function postData()
    {
        $mode = (!empty($_GET['m'])) ? $_GET['m'] : false;
        if ($mode=='model'){
        	if (($_GET['manufacturer'] == 'Generic') || ($_GET['manufacturer'] == 1)){
        		echo "<INPUT type='hidden' name='models' id='models' value='Generic' readonly>";
        		exit();
        	}
        	echo arrayToSelect(array_merge(array(AIP_CHOOSE_MODEL), Cameras::getList($_GET['manufacturer'])), '', 'models', 'change-event', false, 'data-function="cameraChooseModel"');
        	exit;
        };

        if ($mode=='ops') {
            Cameras::getCamDetails($_GET['model']);
            exit;
        };


	    $result = ipCamera::create($_POST);
    	data::responseJSON($result[0], $result[1]);
    	exit;
    }
}

