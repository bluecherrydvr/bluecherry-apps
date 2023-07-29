<?php 

class devices extends Controller {
	public $info;
	public $cards;
    public $ipCameras;

    public function __construct(){
        parent::__construct();
		$this->chAccess('devices');
    }

    public function getData()
    {

        $this->ipCameras = new StdClass();
        $this->ipCameras->arr = array();
        $this->ipCameras->ok = Array();
        $this->ipCameras->disabled = Array();
		$this->getCards();
        $this->getIpCameras();


        $this->setView('ajax.devices');
        $devices = new StdClass();

        $devices->info = $this->info;
        $devices->cards = $this->cards;
        $devices->ipCameras = $this->ipCameras;

        $this->view->devices = $devices;

        if ($this->reqJSON()) {
	        $this->Makejson();
            die();
        }

        if ($this->reqXML()) {
            $this->MakeXML();
            die();
        }
    }

    public function postPortName()
    {
        $stat = false;
        $msg = '';

        $id = Inp::post('id');
        if ($id) {
            $device_name = Inp::post('device_name');
            if (empty($device_name)) {
                $stat = false;
                $msg = NO_DEVICE_NAME;
            } else {
                $ch_name = data::query("SELECT * FROM Devices WHERE device_name='".$device_name."' AND id<>'".$id."'");
                if (empty($ch_name)) {
                    $stat = data::query("UPDATE Devices SET device_name='".$device_name."' WHERE id='{$id}'", true);
                    if ($stat) {
                        $stat = 3;
                        $msg = Array();
                        $msg[] = Array(
                            'name' => 'deviceChangeNameSuccess',
                        );
                        Reply::ajaxDie($stat, $msg);
                    }
                } else {
                    $stat = false;
                    $msg = EXIST_NAME;
                }
            }
        }

        data::responseJSON($stat, $msg);
    }

    private function getCards()
    {
		$cards = data::query("SELECT card_id FROM AvailableSources GROUP BY card_id");
		$this->info['total_devices'] = 0;
		if (!$cards) {
            return false;
        } else {
            foreach ($cards as $key => $card){
                $this->cards[$card['card_id']] = new card($card['card_id']);
                $this->info['total_devices'] += count($this->cards[$card['card_id']]->cameras);
            }

        }
	}
	private function getIpCameras(){
		$info = data::query("SELECT * FROM Devices WHERE protocol in ('IP-RTSP', 'IP-MJPEG', 'IP')");

        $this->setView('devices.camera');

		foreach ($info as $key => $device) {
			$options = (!empty($_GET['XML'])) ? array('no_c_check' => true) : false;
            $item = new ipCamera($device['id'], $options);

            $this->view->device = $item;
            $this->ipCameras->arr[$key] = $item;

            $status = strtolower($item->info['status']);
            array_push($this->ipCameras->$status, $this->renderView());
		}
		$this->info['total_devices'] += count($this->ipCameras->ok);
		$this->info['total_devices'] += count($this->ipCameras->disabled);
	}
	public function MakeXML(){
		$this_user = new user('id', $_SESSION['id']);
		// The \063 is a '?'. Used decimal so as not to confuse vim
		$xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" \x3f>\n<devices>\n";
		$devices = array();
		if (!empty($this->cards)) {
			foreach($this->cards as $card_id => $card) {
				$devices = array_merge($devices, $card->cameras);
			}
		}
		if (!empty($this->ipCameras->arr))
			$devices = array_merge($devices, $this->ipCameras->arr);

		$short_props = array('protocol', 'device_name', 'resolutionX', 'resolutionY');
		$block_props = array('rtsp_password', 'rtsp_username');

		foreach($devices as $device) {
			if (isset($device->info['id']) and
			    !$this_user->camPermission($device->info['id']))
				continue;

			$xml .= '  <device';
			if (!empty($device->info['id']))
				$xml .= ' id="' . $device->info['id'].'"';
			$xml .= ">\n";

			foreach ($device->info as $prop => $val) {
				if (isset($_GET['short']) and
				    !in_array($prop, $short_props))
					continue;

				if (in_array($prop, $block_props) || is_array($prop) || is_array($val))
					continue;
				$xml .= "    <$prop>".htmlspecialchars($val)."</$prop>\n";
			}
			$xml .= "  </device>\n";
		}
		$xml .= "</devices>";
		header('Content-type: text/xml');
		print $xml;
	}
    public function MakeJSON() {
        $this_user = new user('id', $_SESSION['id']);
        $devices = array();
        if (!empty($this->cards)) {
            foreach ($this->cards as $card_id => $card) {
                $devices = array_merge($devices, $card->cameras);
            }
        }
        if (!empty($this->ipCameras->arr))
            $devices = array_merge($devices, $this->ipCameras->arr);

        $json_data = array('devices' => array());

        $short_props = array('protocol', 'device_name', 'resolutionX', 'resolutionY');
        $block_props = array('rtsp_password', 'rtsp_username');

        foreach ($devices as $device) {
            if (isset($device->info['id']) && !$this_user->camPermission($device->info['id']))
                continue;

            $device_data = array();
            if (!empty($device->info['id']))
                $device_data['id'] = $device->info['id'];

            foreach ($device->info as $prop => $val) {
                if (isset($_GET['short']) && !in_array($prop, $short_props))
                    continue;

                if (in_array($prop, $block_props) || is_array($prop) || is_array($val))
                    continue;

                $device_data[$prop] = $val;
            }

            $json_data['devices'][] = $device_data;
        }

        header('Content-type: application/json');
        echo json_encode($json_data, JSON_PRETTY_PRINT);
    }


}


