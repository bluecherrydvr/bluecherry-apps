<?php

/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

include_once lib_path.'Ponvif.php';

class addip extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.addip');


    }

    public function postData()
    {
        $mode = (!empty($_GET['m'])) ? $_GET['m'] : false;
        if ($mode=='model'){
        	if (($_GET['manufacturer'] == 'Generic') || ($_GET['manufacturer'] == 1)){
        		echo "<INPUT type='hidden' name='models' id='models' value='Generic' readonly>";
        		exit();
        	}
        	echo arrayToSelect(array_merge(array(AIP_CHOOSE_MODEL), Cameras::getList($_GET['manufacturer'])), '', 'models', 'change-event', false, 'data-function="cameraChooseModel"');
        	exit;
        };

        if ($mode=='ops') {
            Cameras::getCamDetails($_GET['model']);
            exit;
        };


	    $result = ipCamera::create($_POST);
    	data::responseJSON($result[0], $result[1]);
    	exit;
    }

    public function postCheckOnvifPort()
    {
        $stat = 7;
        $msg = AIP_CHECK_ONVIF_ERROR;

        $ip = Inp::post('ip_addr');
        $port = Inp::post('port');

        $ponvif = new Ponvif();
        if ($ponvif->chOnvif($ip, $port)) {
            $stat = 6;
            $msg = AIP_CHECK_ONVIF_SUCCESS;
        }

        data::responseJSON($stat, $msg);
        exit;
    }
}

