<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$id = (!empty($_SESSION['id'])) ? $_SESSION['id'] : false;
$current_user = new user('id', $id);
$current_user->checkAccessPermissions('devices');
#/auth check

class devices{
	public $info;
	public $cards;
	public $ipCameras;
	public function __construct(){
		$this->getCards();
		$this->getIpCameras();
	}
	private function getCards(){
		$cards = data::query("SELECT * FROM AvailableSources GROUP BY card_id");
		$this->info['total_devices'] = 0;
		if (!$cards) { return false; } 
			else {
				foreach ($cards as $key => $card){
					$this->cards[$card['card_id']] = new card($card['card_id']);
					$this->info['total_devices'] += count($this->cards[$card['card_id']]->cameras);
				}
				
			}
	}
	private function getIpCameras(){
		$info = data::query("SELECT * FROM Devices WHERE protocol in ('IP-RTSP', 'IP-MJPEG', 'IP')");
		foreach ($info as $key => $device) {
			$options = (!empty($_GET['XML'])) ? array('no_c_check' => true) : false;
			$this->ipCameras[$key] = new ipCamera($device['id'], $options);
		}
		$this->info['total_devices'] += count($this->ipCameras);
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
		if (!empty($this->ipCameras))
			$devices = array_merge($devices, $this->ipCameras);

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
		
}

$devices = new devices;

if (isset($_GET['XML']))
	$devices->MakeXML();
else
	include_once('../template/ajax/devices.php');

?>

