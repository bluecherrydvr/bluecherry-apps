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

	$p = @popen("/usr/lib/bluecherry/onvif_tool \"{$onvif_addr}\" \"{$user}\" \"{$pass}\" get_stream_urls", "r");

	if (!$p){
		data::responseJSON($stat, $msg);
		exit;
	}

	$media_service = fgets($p);
	$main_stream = fgets($p);
	$sub_stream = fgets($p);
	pclose($p);
	if ($media_service && $main_stream){
	$stat = 6;
	$msg = AIP_CHECK_ONVIF_SUCCESS;

	$media_uri_parse = parse_url(trim($main_stream));
	if (!isset($media_uri_parse['port']))
		$media_uri_parse['port'] = 554;
	if (isset($media_uri_parse['query'])) $media_uri_parse['path'] .= '?'.$media_uri_parse['query'];

	if ($sub_stream) {
		$sub_parse = parse_url(trim($sub_stream));
		$sub_stream = $sub_parse['path'];
		if (isset($sub_parse['path'])) $sub_stream .= '?'.$sub_parse['query'];
	}

        $data_r = Array(
            //'camName' => (isset($data['Model']) ? $data['Model'] : ''),
            'rtspPath' => $media_uri_parse['path'],
            'rtspPort' => $media_uri_parse['port'],
	    'substream' => $sub_stream,
            //'user' => (isset($data['Default username']) ? $data['Default username'] : ''),
            //'pass' => (isset($data['Default password']) ? $data['Default password'] : ''),
                );
	}
	//

        data::responseJSON($stat, $msg, $data_r);
        exit;
    }
}

