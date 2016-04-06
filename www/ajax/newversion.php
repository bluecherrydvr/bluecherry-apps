<?php  


class newversion extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.newversion');

        $this->view->version = new softwareVersion($this->varpub->global_settings);
    }

    public function postData()
    {
    }
}

