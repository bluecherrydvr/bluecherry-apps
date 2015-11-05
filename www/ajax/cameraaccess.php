<?php 

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

class cameraaccess extends Controller {
	
    public function __construct()
    {
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $user_data = new DVRUser('id', intval($_GET['id']));
    }

    public function postData()
    {
        $user_data = new DVRUser('id', intval($_GET['id']));
    }
}
	

