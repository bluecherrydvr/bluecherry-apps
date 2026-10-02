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
	$data_r = Array();

        $ip = Inp::post('ip_addr');
        $port = Inp::post('port');

	//
	$user = Inp::post('user');
	$pass = Inp::post('pass');
	$onvif_addr = $ip.":".$port;

	$json_out = shell_exec("node /usr/share/bluecherry/onvif/getRtspUrls.js " . escapeshellarg($onvif_addr) .' '. escapeshellarg($user) .' '. escapeshellarg($pass));
	if ($json_out) {
	    $urls = json_decode($json_out, /*associative=*/true);
	    list($main_stream, $sub_stream) = ipCamera::rankOnvifStreams($urls);
	} else {
	    $p = @popen("/usr/lib/bluecherry/onvif_tool " . escapeshellarg($onvif_addr) .' '. escapeshellarg($user) .' '. escapeshellarg($pass). " get_stream_urls", "r");

	    if (!$p){
		    data::responseJSON($stat, $msg);
		    exit;
	    }

	    $media_service = fgets($p);
	    $main_stream = trim((string)fgets($p));
	    $sub_stream = trim((string)fgets($p));
	    pclose($p);
	    list($main_stream, $sub_stream) = ipCamera::rankOnvifStreams(array($main_stream, $sub_stream));
	}
	if ($main_stream) {
	$stat = 6;
	$msg = AIP_CHECK_ONVIF_SUCCESS;

	list($rtsp_path, $rtsp_port) = ipCamera::splitRtspUri($main_stream);
	list($sub_path) = ipCamera::splitRtspUri($sub_stream);

        $data_r = Array(
            //'camName' => (isset($data['Model']) ? $data['Model'] : ''),
            'rtspPath' => $rtsp_path,
            'rtspPort' => $rtsp_port,
	    'substream' => $sub_path,
            //'user' => (isset($data['Default username']) ? $data['Default username'] : ''),
            //'pass' => (isset($data['Default password']) ? $data['Default password'] : ''),
                );
	}
	//

        data::responseJSON($stat, $msg, $data_r);
        exit;
    }
}

