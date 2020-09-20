<?php 


class discoverCameras extends Controller {

    protected $default_pass = Array(
        Array('' => ''),
        Array('admin' => 'admin'),
        Array('admin' => ''),
    );

    // http://ipvm.com/reports/ip-cameras-default-passwords-directory
    protected $default_manuf_pass = Array(
        'ACTI' => Array(
            Array('admin' => '12345'),
            Array('Admin' => '123456'),
        ),
        'American Dynamics' => Array(
            Array('admin' => 'admin'),
            Array('admin' => '9999'),
        ),
        'Arecont Vision' => Array(),
        'Avigilon' => Array(
            Array('admin' => 'admin'),
        ),
        'Axis' => Array(
            Array('root' => 'pass'),
        ),
        'Basler' => Array(
            Array('admin' => 'admin'),
        ),
        'Bosch' => Array(),
        'Brickcom' => Array(
            Array('admin' => 'admin'),
        ),
        'Canon' => Array(
            Array('root' => 'camera'),
        ),
        'Dahua' => Array(
            Array('admin' => 'admin'),
        ),
        'Digital Watchdog' => Array(
            Array('admin' => 'admin'),
        ),
        'DRS' => Array(
            Array('admin' => '1234'),
        ),
        'DVTel' => Array(
            Array('Admin' => '1234'),
        ),
        'DynaColor' => Array(
            Array('Admin' => '1234'),
        ),
        'FLIR' => Array(
            Array('admin' => 'fliradmin'),
        ),
        'Foscam' => Array(
            Array('admin' => ''),
        ),
        'GeoVision' => Array(
            Array('admin' => 'admin'),
        ),
        'Grandstream' => Array(
            Array('admin' => 'admin'),
        ),
        'Hikvision' => Array(
            Array('admin' => '12345'),
        ),
        'Honeywell' => Array(
            Array('admin' => '1234'),
        ),
        'Intellio' => Array(
            Array('admin' => 'admin'),
        ),
        'IQinVision' => Array(
            Array('root' => 'system'),
        ),
        'IPX-DDK' => Array(
            Array('root' => 'admin'),
            Array('root' => 'Admin'),
        ),
        'JVC' => Array(
            Array('admin' => 'jvc'),
        ),
	'Lorex' => Array(
	    Array('admin' => 'admin'),
	),
        'March Networks' => Array(
            Array('admin' => ''),
        ),
        'Mobotix' => Array(
            Array('admin' => 'meinsm'),
        ),
        'Panasonic' => Array(
            Array('admin' => '12345'),
        ),
        'Pelco Sarix' => Array(
            Array('admin' => 'admin'),
        ),
        'Pixord' => Array(
            Array('admin' => 'admin'),
        ),
        'Samsung Electronics' => Array(
            Array('root' => 'root'),
            Array('admin' => '4321'),
        ),
        'Samsung Techwin' => Array(
            Array('admin' => '1111111'),
        ),
        'Samsung' => Array(
            Array('admin' => '4321'),
        ),
        'Sanyo' => Array(
            Array('admin' => 'admin'),
        ),
        'Scallop' => Array(
            Array('admin' => 'password'),
        ),
        'Sentry360' => Array(
            Array('admin' => '1234'),
        ),
        'Sony' => Array(
            Array('admin' => 'admin'),
        ),
        'Speco' => Array(
            Array('admin' => '1234'),
        ),
        'Stardot' => Array(
            Array('admin' => 'admin'),
        ),
        'Starvedia' => Array(
            Array('admin' => ''),
        ),
        'Trendnet' => Array(
            Array('admin' => 'admin'),
        ),
        'Toshiba' => Array(
            Array('root' => 'ikwd'),
        ),
        'VideoIQ' => Array(
            Array('supervisor' => 'supervisor'),
        ),
        'Vivotek' => Array(
            Array('root' => ''),
        ),
        'Ubiquiti' => Array(
            Array('ubnt' => 'ubnt'),
        ),
        'Wodsee' => Array(
            Array('admin' => ''),
        ),
    );

    public function __construct(){
        parent::__construct();
        $this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.discoverCameras');
    }

    public function postData()
    {
        $status = 10;
        $res = Array();
        $ips = Array();
        $checked_ip = Array();
        $onvif_ports = Array(
            80,
            8899,
            34567,
            34599
        );

	//
	$p = @popen("/usr/lib/bluecherry/onvif_tool dummyaddr null null discover", "r");
	if (!$p) {
		data::responseJSON(0);
	}
	$wait_ip_line = true;
	$cams_i = 0;
	while ($str = fgets($p)) {

		//IP
		//scope strings
		//newline

		if ($str[0] == "\n" && !$wait_ip_line) {
			$wait_ip_line = true;
			$cams_i++;
		}

		if ($wait_ip_line){
			if (preg_match('/\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/', $str, $match)) {
				$cam = Array();
				$cam['manufacturer'] = '';
				$cam['model_name'] = '';
				$cam['ipv4_port'] = '';
				$cam['ipv6'] = '';
				$cam['ipv6_path'] = '';
				$cam['onvif'] = true;
				$cam['exists'] = false;

				$cam['ipv4'] = $match[0];
				$ips[] = $match[0];
				$cam['ipv4_path'] = 'http://'.$match[0].'/onvif/device_service';
				$cam['ipv4_port'] = $match[0].':80';

				$wait_ip_line = false;

				$res[$cams_i] = $cam;
			}
		}else{
			//match "name/manufacturer"
			preg_match_all('#name/([\w/%\-]+)#', $str, $match);
			if (!empty($match[1]))
				$res[$cams_i]['manufacturer'] = $match[1][0];
			//match hardware/modelname
			preg_match_all('#hardware/([\w/%\-]+)#', $str, $match);
			if (!empty($match[1]))
				$res[$cams_i]['model_name'] = $match[1][0];
		}
	}
	pclose($p);

        $ips = array_unique($ips);

        $exist_devices = $this->existDevices($ips);
        foreach ($res as $key => $val) {
            if (in_array($val['ipv4'], $exist_devices)) {
                $ips = array_diff($ips, Array($val['ipv4']));
                $res[$key]['exists'] = 1;
            } else {
                $res[$key]['exists'] = 0;
            }
        }
        foreach ($ips as $key => $val) {
            if (in_array($val, $exist_devices)) {
                unset($ips[$key]);
            }
        }
	//
        data::responseJSON($status, 'err', $res);
    }


    protected function curlReq($url) {
        $ch=curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_HEADER, false);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_TIMEOUT, 3);
        $output=curl_exec($ch);

        curl_close($ch);

        return $output;
    }

    protected function getManufPass($manuf)
    {
        $res = Array();
        $manuf = strtolower($manuf);

        foreach ($this->default_manuf_pass as $key => $val) {
            $key = strtolower($key);
            if ($key == $manuf || strpos($manuf, $key) !== false) {
                $res = $val;
                break;
            }
        }

        if (empty($res)) $res = $this->default_pass;

        return $res;
    }

    private function existDevices($ips_arr)
    {
        $res = Array();

        if (!empty($ips_arr)) {
            $ips_str = implode('|', $ips_arr);
            $list_cam = data::query("SELECT device FROM Devices WHERE device REGEXP '{$ips_str}'");
            if (!empty($list_cam)) {
                foreach ($list_cam as $key => $val) {
                    $tmp = explode('|', $val['device']);
                    $res[] = $tmp[0];
                }
            }
        }

        return $res;
    }


    public function postAdd()
    {
        $status = 10;

        $err['ip'] = Array();
        $err['passwd_ip'] = Array();
        $err['rtsp_ip'] = Array();
        $err['onvif_ip'] = Array();

        $added_ip = Array();

        $ipv4 = (Inp::gp_arr('ipv4'));
        $ipv4_port = (Inp::gp_arr('ipv4_port'));
        $ipv4_path = (Inp::gp_arr('ipv4_path'));
        $manufacturer = (Inp::gp_arr('manufacturer'));
        $model_name_arr = (Inp::gp_arr('model_name'));
        $login_arr = (Inp::gp_arr('login'));
        $password_arr = (Inp::gp_arr('password'));

        $exist_devices = $this->existDevices($ipv4);

        foreach(Inp::gp_arr('cameras') as $key => $val) {
            $err_add = true;

            $ip = $ipv4_port[$key];
            if (!in_array($ipv4[$key], $exist_devices)) {
            $manuf = $manufacturer[$key];
            $model_name = $model_name_arr[$key];

            if (!empty($login_arr[$key]) || !empty($password_arr[$key])) {
                $passwords = Array();
                $passwords[] = Array(
                    $login_arr[$key] => $password_arr[$key]
                );
            } else {
                if (!empty($manuf)) {
                    $passwords = $this->getManufPass($manuf);
                } else {
                    $passwords = $this->default_pass;
                }
            }

            $onvif_addr = $ip;

            $password_ch = false;
            // guess login/passwrod
            foreach ($passwords as $pass_arr) {
                foreach ($pass_arr as $login => $password) {
                    $onvif_username = $login;
		    $onvif_password = $password;
                    try {
			 $p = @popen("/usr/lib/bluecherry/onvif_tool \"{$onvif_addr}\" \"{$onvif_username}\" \"{$onvif_password}\" get_stream_urls", "r");
			 if (!$p)
			 	break;

			 $media_service = fgets($p);
			 if (!$media_service)
			 {
				 $err['onvif_ip'][] = $ip;
				 break(2);
			 }
			 $main_stream = fgets($p);
			 $sub_stream = fgets($p);
			 pclose($p);

			if ($main_stream)
                         {
                            $password_ch = true;

			    $media_uri = trim($main_stream);

                            if (empty($media_uri)) {
                                $err['rtsp_ip'][] = $ip;
                            } else {
                                $err_add = false;
                                // add camera to db
                                $media_uri_parse = parse_url($media_uri);

                                if (!isset($media_uri_parse['port'])) {
                                    $media_uri_parse['port'] = 554;
                                }

                                if (isset($media_uri_parse['query'])) $media_uri_parse['path'] .= '?'.$media_uri_parse['query'];

				if ($sub_stream) {
				    $sub_parse = parse_url(trim($sub_stream));
				    $sub_stream = $sub_parse['path'];
				    if (isset($sub_parse['path'])) $sub_stream .= '?'.$sub_parse['query'];
				}
                                $_POST = Array(
                                    'mode' => 'addip',
                                    'models' => 'Generic',
                                    'camName' => $media_uri_parse['host'],
                                    'ipAddr' => $media_uri_parse['host'],
                                    'user' => $login,
                                    'pass' => $password,
                                    'protocol' => 'IP-RTSP',
                                    'rtsp' => $media_uri_parse['path'],
                                    'port' => $media_uri_parse['port'],
				    'substream' => $sub_stream,
                                    'prefertcp' => '1',
                                    'mjpeg' => '',
                                    'portMjpeg' => 80
                                );

                                //print_r($data_camera);
                                $result = ipCamera::create($_POST);
                                if ($result[0]) {
                                    $added_ip[] = $ip;
                                } else {
                                    $err['ip'][] = $ip;
                                }

                            }

                            break(2);
                        }
                    } catch (Exception $e) {
                        $err['ip'][] = $ip;
                        break(2);
                    }
                }

            }

            if (!$password_ch) {
                $err['passwd_ip'][] = $ip;
            }

            } else {
                $added_ip[] = $ip;
            }
        }

        $res['err'] = $err;
        $res['added_ip'] = $added_ip;


        data::responseJSON($status, '', $res);
    }
}
