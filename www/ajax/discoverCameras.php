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

        $onvif_service_url = 'http://127.0.0.1:4000/devices/discover';
        $json_str = $this->curlReq($onvif_service_url);
        if ($json_str) {
            $discovery_result = json_decode($json_str, true);
            if ($discovery_result && isset($discovery_result['data'])) {
                foreach ($discovery_result['data'] as $cam) {
                    $ip = '';
                    if (isset($cam['ipv4'])) {
                        $ip = $cam['ipv4'];
                    }
                    if (in_array($ip, $ips)) { continue; }
                    $res[] = $cam;
                    $ips[] = $ip;
                }
            }
        }

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

        // Load previous scan from /var/lib/bluecherry/last_scan.json
        $last_scan_file = '/var/lib/bluecherry/last_scan.json';
        $prev_cameras = file_exists($last_scan_file) ? json_decode(file_get_contents($last_scan_file), true) : array();
        // Index by ipv4
        $index_by = function($arr, $key) {
            $out = array();
            foreach ($arr as $item) {
                if (isset($item[$key])) {
                    $out[$item[$key]] = $item;
                }
            }
            return $out;
        };
        $prev_index = $index_by($prev_cameras, 'ipv4');
        $new_index = $index_by($res, 'ipv4');
        $added = array_diff_key($new_index, $prev_index);
        $removed = array_diff_key($prev_index, $new_index);
        $unchanged = array_intersect_key($new_index, $prev_index);
        // Save new scan
        @file_put_contents($last_scan_file, json_encode($res, JSON_PRETTY_PRINT));
        // Add found_count and comparison to response
        $response = [
            'devices' => $res,
            'found_count' => count($res),
            'added' => array_values($added),
            'removed' => array_values($removed),
            'unchanged' => array_values($unchanged)
        ];
        data::responseJSON($status, '', $response);
    }


    protected function curlReq($url, $post_data = null) {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60); // Increased timeout for discovery

        if ($post_data) {
            curl_setopt($ch, CURLOPT_POST, true);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $post_data);
            curl_setopt($ch, CURLOPT_HTTPHEADER, [
                'Content-Type: application/json',
                'Content-Length: ' . strlen($post_data)
            ]);
        } else {
            curl_setopt($ch, CURLOPT_HEADER, false);
        }

        $output = curl_exec($ch);
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
            $ips_arr = array_filter($ips_arr);
            if (!empty($ips_arr)) {
                $ips_str = implode('|', $ips_arr);
                $list_cam = data::query("SELECT device FROM Devices WHERE CAST(device AS BINARY) REGEXP BINARY '{$ips_str}'");
                if (!empty($list_cam)) {
                    foreach ($list_cam as $key => $val) {
                        $tmp = explode('|', $val['device']);
                        $res[] = $tmp[0];
                    }
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
        $event_types_arr = (Inp::gp_arr('event_types'));

        $exist_devices = $this->existDevices($ipv4);

        foreach(Inp::gp_arr('cameras') as $key => $val) {
            $err_add = true;

            $ip = $ipv4_port[$key];
            if (!in_array($ipv4[$key], $exist_devices)) {
                $model_name = $model_name_arr[$key];
                // Only use user-supplied credentials
                if (!empty($login_arr[$key]) && !empty($password_arr[$key])) {
                    $onvif_username = $login_arr[$key];
                    $onvif_password = $password_arr[$key];
                } else {
                    // No credentials provided, skip and prompt for credentials
                    $err['passwd_ip'][] = $ip;
                    continue;
                }

                // Always construct xaddr as http://<ip>:<port>/onvif/device_service
                $ip_only = $ipv4[$key];
                $port_only = 80;
                if (strpos($ipv4_port[$key], ':') !== false) {
                    list($ip_only, $port_only) = explode(':', $ipv4_port[$key], 2);
                } else if (!empty($ipv4_port[$key])) {
                    $port_only = $ipv4_port[$key];
                }
                $onvif_addr = "http://{$ip_only}:{$port_only}/onvif/device_service";

                try {
                    $main_stream = null;
                    $sub_stream = null;
                    $onvif_service_url = 'http://127.0.0.1:4000/devices/rtsp';
                    $post_data = json_encode([
                        'xaddr' => $onvif_addr,
                        'username' => $onvif_username,
                        'password' => $onvif_password
                    ]);
                    $json_out = $this->curlReq($onvif_service_url, $post_data);

                    if ($json_out) {
                        $urls = json_decode($json_out, true);
                        if (isset($urls[0]['rtspUri']) || isset($urls[0]['rtspUrl']) || isset($urls[0]['url'])) {
                            $main_stream = isset($urls[0]['rtspUri']) ? $urls[0]['rtspUri'] : (isset($urls[0]['rtspUrl']) ? $urls[0]['rtspUrl'] : $urls[0]['url']);
                            $sub_stream = isset($urls[1]['rtspUri']) ? $urls[1]['rtspUri'] : (isset($urls[1]['rtspUrl']) ? $urls[1]['rtspUrl'] : (isset($urls[1]['url']) ? $urls[1]['url'] : null));
                        }
                    }

                    if ($main_stream)
                    {
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
                                'models' => $model_name,
                                'camName' => $media_uri_parse['host'],
                                'ipAddr' => $media_uri_parse['host'],
                                'user' => $onvif_username,
                                'pass' => $onvif_password,
                                'protocol' => 'IP-RTSP',
                                'rtsp' => $media_uri_parse['path'],
                                'port' => $media_uri_parse['port'],
                                'substream' => $sub_stream,
                                'prefertcp' => '0',
                                'mjpeg' => '',
                                'portMjpeg' => 80,
                                'event_subscriptions' => isset($event_types_arr[$key]) ? $event_types_arr[$key] : ''
                            );

                            $result = ipCamera::create($_POST);
                            if ($result[0]) {
                                $device_id = false;
                                if (isset($result[1]['id'])) {
                                    $device_id = $result[1]['id'];
                                } else {
                                    $device_row = data::getObject('Devices', 'device', "{$media_uri_parse['host']}|{$media_uri_parse['port']}|{$media_uri_parse['path']}");
                                    if ($device_row && isset($device_row[0]['id'])) {
                                        $device_id = $device_row[0]['id'];
                                    }
                                }
                                $added_ip[] = array('ip' => $ip, 'id' => $device_id);
                            } else {
                                $err['ip'][] = $ip;
                                // Always include the error message from ipCamera::create
                                if (isset($result[1]) && $result[1]) {
                                    $err['add_error'][$ip] = $result[1];
                                }
                            }
                        }
                    } else {
                        $err['rtsp_ip'][] = $ip;
                    }
                } catch (Exception $e) {
                    $err['ip'][] = $ip;
                    $err['add_error'][$ip] = $e->getMessage();
                }
            } else {
                $added_ip[] = $ip;
            }
        }

        $res['err'] = $err;
        $res['added_ip'] = $added_ip;

        // Use status 6 for success (any added), 7 for error (none added)
        $status = count($added_ip) > 0 ? 6 : 7;
        data::responseJSON($status, '', $res);
    }
}
