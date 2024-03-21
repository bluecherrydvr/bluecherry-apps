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
    	$camera = new ipCamera($id); // Reinstance the object to update information
        $camera->checkConnection(); // Test the connection
    	data::responseJSON($result[0], $result[1],  array(
            json_encode($camera->info['connection_status']),
            str_replace("%TYPE%", str_replace("IP-", "", $camera->info['protocol']), 
                $camera->info['connection_status']['success'] ? AIP_CONNECTION_SUCCESS : AIP_CONNECTION_FAIL)
        ));
    	exit();
    }

    public function postTest() 
    {
	    $id = intval($_POST['id']);
    	$camera = new ipCamera($id);
        $camera->checkConnection();
        data::responseJSON(10, "", array(
            json_encode($camera->info['connection_status']),
            str_replace("%TYPE%", str_replace("IP-", "", $camera->info['protocol']), 
                $camera->info['connection_status']['success'] ? AIP_CONNECTION_SUCCESS : AIP_CONNECTION_FAIL)
        )); 
        exit();
    }
}