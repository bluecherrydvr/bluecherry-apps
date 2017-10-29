<?php 

include_once lib_path.'Ponvif.php';

class editip extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.editip');

        $id = (isset($_GET['id'])) ? intval($_GET['id']) : false;

	//$ponvif = new Ponvif();


	$cam = new ipCamera($id);
        $this->view->ipCamera = $cam;

	$w = $cam->info['resolutionX'];
	$h = $cam->info['resolutionY'];

	//$ponvif->setIPAddress($cam->info['ipAddr'] . ':' . $cam->info['onvif_port']);
	//$ponvif->setUsername($cam->info['rtsp_username']);
	//$ponvif->setPassword($cam->info['rtsp_password']);

	//$init = $ponvif->initialize();
	//$sources = $ponvif->getSources();

	$onvifResolutions = array();
	$ret = 0;

	exec("/usr/lib/bluecherry/onvif_tool ".
		$cam->info['ipAddr'] . ':' . $cam->info['onvif_port'].
		" ".$cam->info['rtsp_username'].
		" ".$cam->info['rtsp_password'].
		" resolutions",
		$onvifResolutions, $ret);

	if ($ret != 0) {
		$onvifResolutions[0] = $w."x".$h;
	}

	/*if (empty($sources) || $ponvif->isFault($sources)) {
		$onvifResolutions[0] = $w."x".$h;
	} else {
		$r = 0;
		for ($i = 0; $i < count($sources); $i++) {
			for ($j = 0; $j < count($sources[$i]); $j++) {
				$onvifResolutions[$r++] = $sources[$i][$j]['width']."x".$sources[$i][$j]['height'];
			}
		}
	}*/


	$this->view->onvifResolutions = $onvifResolutions;
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


