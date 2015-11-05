<?php 


class ptzsettings extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.ptzsettings');

	    $this_camera = new camera($_GET['id']);
        if (!isset($this_camera->info['ptz_config'])) {
            $this_camera->info['ptz_config']['protocol'] = '';
            $this_camera->info['ptz_config']['path'] = '';
            $this_camera->info['ptz_config']['addr'] = '';
            $this_camera->info['ptz_config']['baud'] = '';
            $this_camera->info['ptz_config']['bit'] = '';
            $this_camera->info['ptz_config']['stop_bit'] = '';
            $this_camera->info['ptz_config']['parity'] = '';
        }

        $this->view->this_camera = $this_camera;
    }

    public function postData()
    {
	    camera::updatePtzSettings($_POST['path'], $_POST['baud'], $_POST['bit'], $_POST['parity'], $_POST['stop_bit'], $_POST['protocol'], $_POST['addr'], $_POST['id']);
    }
}

