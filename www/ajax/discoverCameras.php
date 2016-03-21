<?php 

include_once lib_path.'Ponvif.php';

class discoverCameras extends Controller {

    protected $default_pass = Array(
        Array('' => ''),
        Array('admin' => 'admin'),
        Array('admin' => ''),
    );

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

        $ips = Array();
        $ips[] = '192.168.0.1';
        $ips[] = '192.168.0.2';
        $ips[] = '192.168.0.3';
        $ips[] = '192.168.0.4';
        $ips[] = '192.168.0.5';
        $ips[] = '192.168.0.6';
        $ips[] = '192.168.0.7';
        $ips[] = '192.168.0.8';
        $ips[] = '192.168.0.9';
        $ips[] = '192.168.0.10';
        $ips[] = '192.168.0.11';
        $ips[] = '192.168.0.21';
        $ips[] = '192.168.0.31';
        $ips[] = '192.168.0.41';
        $ips[] = '192.168.0.51';
        $ips[] = '192.168.0.61';
        $ips[] = '192.168.0.71';
        $ips[] = '192.168.0.81';
        $ips[] = '192.168.0.84:8899';
        $ips[] = '192.168.0.64';
        $start = microtime(true);

    $test=new Ponvif();
        //$info = new ipCamera(1);
        //$info = data::query("SELECT * FROM Devices WHERE device LIKE '%222%'");
        //print_r($info);
        //$tmp = explode('|', $info[0]['device']);
        //print_r($info);
        //print_r($tmp);

        //foreach ($ips as $ip) {
        //$ip = '172.16.1.118';
        //if ($test->chOnvif($ip, '80')) echo 'ok';
        //else echo 'err';

    //$test->setIPAddress($ip);
    //$test->setUsername('admin');
    //$test->setPassword('admin');
    //$init = $test->initialize();
    //if ($init['datetime']) echo 'timeok<br>';
    //else echo 'time err<br>';
    //if ($init['profile']) echo 'profile ok <br>';
    //else echo 'profile err<br>';
    //try {


    //print_r($test->core_GetSystemDateAndTime());
    ////die();
    //$sources=$test->getNetworkProtocols();
                        //$sources = $test->getSources();
    ////if (empty($sources) || $test->isFault($sources)) echo 'err';
    //print_r($sources);
    ////die();

    //$hz = $test->core_GetDeviceInformation();
    //print_r($hz);
    //$profileToken =$sources[0][0]['profiletoken'];
    //$profileToken_name =$sources[0][0]['profiletoken_name'];
    //$mediaUri=$test->media_GetStreamUri($profileToken);
    //if (empty($mediaUri)) $mediaUri=$test->media_GetStreamUri($profileToken_name);
    //print_r($mediaUri);

	//} catch (Exception $e) {
		//echo 'Caught exception: ',  $e->getMessage(), "\n";
	//}
			////$profileToken=$sources[0][0]['profiletoken'];
			////$ptzNodeToken=$sources[0][0]['ptz']['nodetoken'];
		////$mediaUri=$test->media_GetStreamUri($profileToken);
		////echo $mediaUri."\n";
    ////print_r($test->core_GetSystemDateAndTime());
        //}
        $end = microtime(true) - $start;
        //echo '<hr>'.$end;
    //die();


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

        $p = @popen("/usr/bin/nmap --script=broadcast-wsdd-discover", "r");
        if (!$p) {
            data::responseJSON(0);
        }
        while ($str = fgets($p)) {
            // check right onivf device
            if (strpos($str, 'Address:') !== false) {
                preg_match_all('#\bhttps?://[^\s()<>]+(?:\([\w\d]+\)|([^[:punct:]\s]|/))#', $str, $match);
                if (!empty($match[0])) {
                    $data_ip_cam = $this->dataIpCam($match[0]);
                    $checked_ip[] = $data_ip_cam['ipv4'];
                    $res[] = $data_ip_cam;
                }
            }

            if (preg_match('/\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/', $str, $match)) {
                $ips[] = $match[0];
            }
        }
        pclose($p);

        $ips = array_unique($ips);

        $exist_devices = $this->existDevices($ips);
        // don't explore exist devices
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
                $res[] = $this->dataIpCam(Array('http://'.$val.'/onvif/device_service'), 1);
                unset($ips[$key]);
            }
        }


        //$ips[] = '192.168.0.1';
        //$ips[] = '192.168.0.2';
        //$ips[] = '192.168.0.3';
        //$ips[] = '192.168.0.4';
        //$ips[] = '192.168.0.5';
        //$ips[] = '192.168.0.6';
        //$ips[] = '192.168.0.7';
        //$ips[] = '192.168.0.8';
        //$ips[] = '192.168.0.9';
        //$ips[] = '192.168.0.10';
        //$ips[] = '192.168.0.11';
        //$ips[] = '192.168.0.21';
        //$ips[] = '192.168.0.31';
        //$ips[] = '192.168.0.41';
        //$ips[] = '192.168.0.51';
        //$ips[] = '192.168.0.61';
        //$ips[] = '192.168.0.71';
        //$ips[] = '192.168.0.81';
        //$ips[] = '192.168.0.91';
        //$ips[] = '192.168.0.110';

        $start = microtime(true);


        $onvif_ports_str = implode(',', $onvif_ports);
        $ponvif = new Ponvif();
        // check chines devices
        foreach ($ips as $ip) {
            if (!in_array($ip, $checked_ip)) {
                $checked_ip[] = $ip;

                $p = @popen("/usr/bin/nmap -p ".$onvif_ports_str." -oX - ".$ip." 2>&1", "r");
                if ($p) {
                    $xml = '';
                    while ($str = fgets($p)) {
                        $xml .= trim($str);
                    }
                    pclose($p);

                    $xml = simplexml_load_string($xml);
                    if (isset($xml->host->ports->port)) {
                        foreach ($xml->host->ports->port as $key => $val) {
                            $state_port = (string) $val->state->attributes()->state;
                            $port = (string) $val->attributes()->portid;
                            if ($state_port == 'open') {
                                if ($ponvif->chOnvif($ip, $port)) {
                                    $res[] = $this->dataIpCam(Array('http://'.$ip.':'.$port.'/onvif/device_service'));
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }



        // use upnp to get model/manufacturer
        $p = @popen("/usr/bin/nmap -sV --script=broadcast-upnp-info", "r");
        if ($p) {
            while ($str = fgets($p)) {
                if (strpos($str, 'Location: http://') !== false) {
                    preg_match('#\bhttps?://[^\s()<>]+(?:\([\w\d]+\)|([^[:punct:]\s]|/))#', $str, $match);

                    if (!empty($match[0])) {
                        $match_parse = parse_url($match[0]);
                        if (in_array($match_parse['host'], $ips)) {
                            $xml = $this->curlReq($match[0]);
                            $xml = simplexml_load_string($xml);
                            if ($xml) {
                                $manuf = '';
                                $model = '';
                                if (isset($xml->device->manufacturer)) {
                                    $manuf = (string) $xml->device->manufacturer;
                                }
                                if (isset($xml->device->modelNumber)) {
                                    $model = (string) $xml->device->modelNumber;
                                }

                                $res = $this->modifIps($match_parse['host'], $manuf, $model, $res);
                            }
                        }

                    }
                }
            }
            pclose($p);
        }


        //$xml = simplexml_load_string($xml);
        //$hz = (string) $xml->host->ports->port[0]->state->attributes()->state;
        //echo $hz;

        $end = microtime(true) - $start;
        //echo '<hr>'.$end;

        //print_r($res);

        //$tmp_arr = Array();
        //$tmp_arr['ipv4'] = '192.168.0.100';
        //$tmp_arr['ipv4_path'] = 'http://192.168.0.100/onvif/device_service';
        //$tmp_arr['ipv6'] = '111:222:222:0:333:444:123:33';
        //$tmp_arr['ipv6_path'] = 'http://[111:222:222:0:333:444:123:33]/onvif/device_service';
        //$res[] = $tmp_arr;

        data::responseJSON($status, 'err', $res);
    }

    protected function dataIpCam($data, $exists = 0)
    {
        $res = Array();
        $res['manufacturer'] = '';
        $res['model_name'] = '';
        $res['ipv4'] = '';
        $res['ipv4_port'] = '';
        $res['ipv4_path'] = '';
        $res['ipv6'] = '';
        $res['ipv6_path'] = '';
        $res['exists'] = $exists;

            foreach ($data as $key => $val) {
                $val_parse = parse_url($val);
                $port = '';
                if (isset($val_parse['port'])) $port = ':'.$val_parse['port'];

                if (strpos($val_parse['host'], ':') === false) {
                    // ip4v
                    $res['ipv4'] = $val_parse['host'];
                    $res['ipv4_port'] = $val_parse['host'].$port;
                    $res['ipv4_path'] = $val;

                } else {
                    // ip6v
                    $res['ipv6_path'] = $val;
                    $res['ipv6'] = $val_parse['host'];
                }
            }

        return $res;
    }

    protected function modifIps($ipv4, $manuf, $model_name, $data)
    {
        foreach ($data as $key => $val) {
            if (($ipv4 == $val['ipv4_port']) || ($ipv4 == $val['ipv4']))  {
                $data[$key]['manufacturer'] = $manuf;
                $data[$key]['model_name'] = $model_name;
            }
        }

        return $data;
    }


    protected function curlReq($url) {
        $ch=curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_HEADER, false);
        //curl_setopt($ch, CURLOPT_NOBODY,true);
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
            if ($key == $manuf) {
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

        $exist_devices = $this->existDevices($ipv4);

        $ponvif = new Ponvif();
        foreach(Inp::gp_arr('cameras') as $key => $val) {
            $err_add = true;

            $ip = $ipv4_port[$key];
            if (!in_array($ipv4[$key], $exist_devices)) {
            $manuf = $manufacturer[$key];
            $model_name = $model_name_arr[$key];

            if (!empty($manuf)) {
                $passwords = $this->getManufPass($manuf);
            } else {
                $passwords = $this->default_pass;
            }

            $ponvif->setIPAddress($ip);

            $password_ch = false;
            // guess login/passwrod
            foreach ($passwords as $pass_arr) {
                foreach ($pass_arr as $login => $password) {
                    $ponvif->setUsername($login);
                    $ponvif->setPassword($password);
                    try {
                        $init = $ponvif->initialize();
                        if (!$init['datetime']) break;
                        if (!$init['profile']) {
                            // wrong onvif version
                            $password_ch = true;
                            $err['onvif_ip'][] = $ip;
                            break(2);
                        }
                        //if (!$init['profile']) {
                            //break;
                        //}

                        $sources = $ponvif->getSources();
                        if (empty($sources) || $ponvif->isFault($sources)) {
                            // error
                        } else {
                            $password_ch = true;

                            $profileToken = $sources[0][0]['profiletoken'];
                            $profileToken_name = $sources[0][0]['profiletoken_name'];
                            $media_uri = $ponvif->media_GetStreamUri($profileToken);
                            if (empty($media_uri)) $media_uri = $ponvif->media_GetStreamUri($profileToken_name);

                            if (empty($media_uri)) {
                                $err['rtsp_ip'][] = $ip;
                            } else {
                                $err_add = false;
                                // add camera to db
                                $media_uri_parse = parse_url($media_uri);

                                if (!isset($media_uri_parse['port'])) {
                                    $media_uri_parse['port'] = 554;

                                    $net_protocols = $ponvif->getNetworkProtocols();
                                    foreach ($net_protocols as $net_key => $net_val) {
                                        if (isset($net_val['Name']) && ($net_val['Name'] == 'RTSP')) {
                                            $media_uri_parse['port'] = $net_val['Port'];
                                            break;
                                        }
                                    }
                                }
                                if (isset($media_uri_parse['query'])) $media_uri_parse['path'] .= '?'.$media_uri_parse['query'];

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
                        //echo 'Caught exception: ',  $e->getMessage(), "<br>";
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
