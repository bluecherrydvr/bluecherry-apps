<?php 

class motionmap extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.motionmap');

        $this->view->camera = device(intval($_GET['id']));
    }

    public function postData()
    {

    }
}

