<?php 

class ipcamerasetup extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        //$this->setView('ajax.ipcamerasetup');

        $id = intval(Inp::get('id'));
        $camera = new ipCamera($id);
    }

    public function postData()
    {
    }
}

