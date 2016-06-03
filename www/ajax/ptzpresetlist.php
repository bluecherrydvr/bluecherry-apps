<?php

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */


class ptzpresetlist extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $onvif_rec = data::query("SELECT * FROM ipPtzCommandPresets WHERE name='ONVIF' AND protocol='onvif' LIMIT 1");
        if (empty($onvif_rec)) {
            data::query("INSERT INTO ipPtzCommandPresets (`name`, `driver`, `mright`, `mleft`, `up`, `down`, `up_right`, `up_left`, `down_right`, `down_left`, `wide`, `tight`, `focus_in`, `focus_out`, `preset_save`, `preset_go`, `stop`, `stop_zoom`, `needs_stop`, `http_auth`, `custom`, `port`, `protocol`) VALUES ('ONVIF', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', 0, 0, 0, '', 'onvif')", true);
        }

        $this->setView('ajax.ptzpresetlist');

        $this->view->presets = data::getObject('ipPtzCommandPresets');

        $camera_id = (!empty($_GET['id'])) ? intval($_GET['id']) : false;
        $this->view->camera = new ipCamera($camera_id);
    }

    public function postData()
    {

    }
}

