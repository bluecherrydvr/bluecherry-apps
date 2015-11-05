<?php 

class videoadj extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.videoadj');

        $this->view->device_data = data::getObject('Devices', 'id', intval($_GET['id']));
    }

    public function postData()
    {

    }
}

