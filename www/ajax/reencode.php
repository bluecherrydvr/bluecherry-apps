<?php 


class reencode extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
	global $global_settings;

	$this->setView('ajax.reencode');

	$vaapi_device = $global_settings->data['G_VAAPI_DEVICE'];

	$vaapi_na = "";

	if (strcasecmp($vaapi_device, "none") == 0)
		$vaapi_na = "disabled";

	$this->view->vaapi_na = $vaapi_na;

	$this_camera = device(intval($_GET['id']));

	$this->view->this_camera = $this_camera;
    }

    public function postData()
    {
	    $id = intval($_POST['id']);
	    $enabled = !empty($_POST['reencode_enabled']) ? 1 : 0;
	    $bitrate = intval($_POST['bitrate']);
	    $resolution = $_POST['resolution'];

	    $resolution = explode('x', $resolution);
	    $w = intval($resolution[0]);
	    $h = intval($resolution[1]);

            $status = data::query("UPDATE Devices SET reencode_livestream='{$enabled}', reencode_bitrate='{$bitrate}',
				 reencode_frame_width='{$w}', reencode_frame_height='{$h}' WHERE id='{$id}'", true);
            data::responseJSON($status);
    }
}

