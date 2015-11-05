<?php 

class editip extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.editip');

        $id = (isset($_GET['id'])) ? intval($_GET['id']) : false;
        $this->view->ipCamera = new ipCamera($id);
    }

    public function postData()
    {
	    $id = intval($_POST['id']);
    	$camera = new ipCamera($id);
    	$result = $camera->edit($_POST);
    	data::responseJSON($result[0], $result[1]);
    	exit();
    }
}


