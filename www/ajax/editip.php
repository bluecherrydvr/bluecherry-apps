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
        $camera->checkConnection();
    	data::responseJSON($result[0], array(
            $result[1], 
            str_replace(array("%TYPE%", "%TIME%"), array("RTSP", $camera->info['connection_status']['lastTimeChecked']), 
                $camera->info['connection_status']['success'] ? AIP_CONNECTION_SUCCESS : AIP_CONNECTION_FAIL)
        ), json_encode($camera->info['connection_status']));
    	exit();
    }

    public function postTest() 
    {
	    $id = intval($_POST['id']);
    	$camera = new ipCamera($id);
        $camera->checkConnection();
        data::responseJSON(10, array( 1 => str_replace( //-> 10 as status to avoid the "changes saved successfully" popup
            array("%TYPE%", "%TIME%"), 
            array("RTSP", $camera->info['connection_status']['lastTimeChecked']), 
            $camera->info['connection_status']['success'] ? AIP_CONNECTION_SUCCESS : AIP_CONNECTION_FAIL)
        ), json_encode($camera->info['connection_status'])); 
        exit();
    }
}