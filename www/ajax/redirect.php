<?php 

class redirect extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('basic');
    }

    public function getData()
    {
        $camera = new ipCamera(intval($_GET['id']));

        header('Location: http://'.$camera->info['ipAddr']);
    }

    public function postData()
    {
    }
}
