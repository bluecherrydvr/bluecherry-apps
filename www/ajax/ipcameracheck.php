<?php 

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

class ipcameracheck extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->initProc();
    }

    public function postData()
    {
        $this->initProc();
    }

    private function initProc()
    {
        init();

        $id = (!empty($_GET['id'])) ? intval($_GET['id']) : false;

        $camera = new ipCamera($id);

        $camera->checkConnection();

        $status_message = '';
        foreach($camera->info['connection_status'] as $type => $status){
        	if ($status!='OK'){
        		$status_message .= str_replace('%TYPE%', $type, constant('IP_ACCESS_STATUS_'.$status)).'<br /><br />';
        	}
        };

        echo $status_message;
    }
}

