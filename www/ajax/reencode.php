<?php 


class reencode extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.reencode');

	$this_camera = device(intval($_GET['id']));
        
        $this->view->this_camera = $this_camera;
    }

    public function postData()
    {
	    //camera::updatePtzSettings($_POST['path'], $_POST['baud'], $_POST['bit'], $_POST['parity'], $_POST['stop_bit'], $_POST['protocol'], $_POST['addr'], $_POST['id']);
    }
}

