<?php  


class newversion extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.newversion', false);
    }

    public function postData()
    {
        $view = new View('ajax.newversion');
        echo $view->render();
    }
}

