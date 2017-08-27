<?php 
	
class gpio extends Controller {
	public $schedule_data;
    public $device_data;

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    private function getCards()
    {
                $cards = data::query("SELECT card_id FROM AvailableSources WHERE driver LIKE 'solo%' GROUP BY card_id");
                $this->info['total_devices'] = 0;
                if (!$cards) { return false; }
                        else {
                                foreach ($cards as $key => $card){
                                        $this->cards[$card['card_id']] = new card($card['card_id']);
                                        $this->info['total_devices'] += count($this->cards[$card['card_id']]->cameras);
                                }

                        }
     }

    private function readPinVal($pin_id)
    {
	$v = '-';

	if (file_exists("/sys/class/gpio/gpio".$pin_id."/value"))
	{
		$h = fopen("/sys/class/gpio/gpio".$pin_id."/value", 'r');

		if ($h)
			$v = intval(fread($h, 1));

		fclose($h);
	}

	return $v;
    }

    private function getPinStates()
    {
	$pinstates = array();

	$pin_basenums = data::query("SELECT MIN(input_pin_id)-8 as basenum, card_id FROM GpioConfig GROUP BY card_id");

	foreach ($pin_basenums as $key => $basenum)
	{
		$basepin = $basenum['basenum'];

		$relays = array();
		$sensors = array();

		for ($pin_id = $basepin; $pin_id < $basepin + 8; $pin_id++)
			$relays[$pin_id - $basepin] = $this->readPinVal($pin_id);

		for ($pin_id = $basepin + 8; $pin_id < $basepin + 24; $pin_id++)
			$sensors[$pin_id - $basepin - 8] = $this->readPinVal($pin_id);

		$pinstates[$basenum['card_id']]['relays'] = $relays;
		$pinstates[$basenum['card_id']]['sensors'] = $sensors;

	}

	return $pinstates;
    }

    private function resetRelays()
    {
	$pin_basenums = data::query("SELECT MIN(input_pin_id)-8 as basenum, card_id FROM GpioConfig GROUP BY card_id");

	foreach ($pin_basenums as $key => $basenum)
	{
		$basepin = $basenum['basenum'];
		$output = array();
	        $ret = 0;

		for ($relay_pin = $basepin; $relay_pin < $basepin + 8; $relay_pin++)
		{
			if (!file_exists("/sys/class/gpio/gpio".$relay_pin."/value"))
				exec("/usr/lib/bluecherry/gpiocmd export_pin ".$relay_pin, $output, $ret);

			exec("/usr/lib/bluecherry/gpiocmd write ".$relay_pin." 0", $output, $ret);
			//TODO check for errors, return ajax error
		}
	}
    }

    public function postData()
    {

	if (!empty($_GET['mode']) && $_GET['mode'] == 'reset')
		$this->resetRelays();

	data::responseJSON(true);
    }

    public function getData()
    {
        if (!empty($_GET['mode']) && $_GET['mode'] == 'reset')
                $this->resetRelays();

        $this->setView('ajax.gpio');

	$this->view->pinstates = $this->getPinStates();
	$this->getCards();
	$this->view->cards = $this->cards;

        $id = (isset($_GET['id']) and $_GET['id'] != '') ? intval($_GET['id']) : 'global';

		$q = ($id != 'global') ? "SELECT id, device_name, protocol, schedule, schedule_override_global FROM Devices WHERE id='$id'" : "SELECT value as schedule FROM GlobalSettings WHERE parameter='G_DEV_SCED'";
		$tmp =  data::query($q);
		#if there is no schedule copy global
		if ($id != 'global' && empty($tmp[0]['schedule'])){
			$global_schedule = data::query("SELECT value as schedule FROM GlobalSettings WHERE parameter='G_DEV_SCED'");
			data::query("UPDATE Devices SET schedule='{$global_schedule[0]['schedule']}' WHERE id='$id'");
			$tmp = data::query($q);
		}
		$this->schedule_data = $tmp;
		if ($id=='global') $this->device_data[0]['id'] = 'global';

        $device_schedule = new StdClass();
        $device_schedule->schedule_data = $this->schedule_data;
        $device_schedule->device_data = $this->device_data;

        $this->view->global = ($device_schedule->device_data[0]['id']=='global') ? true : false;
        $this->view->og = (!isset($device_schedule->schedule_data[0]['schedule_override_global'])) ? false : $device_schedule->schedule_data[0]['schedule_override_global'];

        $this->view->device_schedule = $device_schedule;
    }

}


